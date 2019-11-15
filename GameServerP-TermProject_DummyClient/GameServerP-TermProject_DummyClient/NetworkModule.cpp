#define _WINSOCK_DEPRECATED_NO_WARNINGS

// 콘솔 창을 띄울 것이므로 
#define USE_CONSOLE_WINDOW_PRINT
#ifdef USE_CONSOLE_WINDOW_PRINT
#include <stdio.h>
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console") 
#endif

#include <random>
#include <WinSock2.h>
#include <winsock.h>
#include <Windows.h>
#include <iostream>
#include <thread>
#include <vector>
#include <unordered_set>
#include <mutex>
#include <atomic>
#include <chrono>
#include <queue>
#include <array>
#include <unordered_map>

using namespace std;
using namespace chrono;

extern HWND		hWnd;

#define USE_2019_TT_PROTOCOL

#ifdef USE_2019_TT_PROTOCOL
#include "../../GameServerP-TermProject_Server/GameServerP-TermProject_Server/2019_화목_protocol.h"
#else
#include "../../GameServerP-TermProject_Server/GameServerP-TermProject_Server/temporary_protocol.h"
#endif

// const static int MAX_TEST = 500;
const static int MAX_TEST = MAX_USER;
const static int INVALID_ID = -1;
const static int MAX_PACKET_SIZE = 255;
const static int MAX_BUFF_SIZE = 255;

#pragma comment (lib, "ws2_32.lib")



HANDLE g_hiocp;

enum OPTYPE { OP_SEND, OP_RECV, OP_DO_MOVE };

high_resolution_clock::time_point last_connect_time;

struct OverlappedEx {
	WSAOVERLAPPED over;
	WSABUF wsabuf;
	unsigned char IOCP_buf[MAX_BUFF_SIZE];
	OPTYPE event_type;
	int event_target;
};

struct CLIENT {
	int id;
	int x;
	int y;
	high_resolution_clock::time_point last_move_time;
	bool connect;

	SOCKET client_socket;
	OverlappedEx recv_over;
	unsigned char packet_buf[MAX_PACKET_SIZE];
	int prev_packet_data;
	int curr_packet_size;
};

array<CLIENT, MAX_USER> g_clients;
atomic_int num_connections;

vector <thread *> worker_threads;
thread test_thread;

float point_cloud[MAX_USER * 2];

// 나중에 NPC까지 추가 확장 용
struct ALIEN {
	int id;
	int x, y;
	int visible_count;
};

void error_display(const char *msg, int err_no)
{
	WCHAR *lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
#ifdef USE_CONSOLE_WINDOW_PRINT
	std::cout << msg << ' ' << err_no;
	std::wcout << L"에러" << lpMsgBuf << std::endl;
#endif
	MessageBox(hWnd, lpMsgBuf, L"ERROR", 0);
	LocalFree(lpMsgBuf);
	while (true);
}

void DisconnectClient(int ci)
{
	closesocket(g_clients[ci].client_socket);
	g_clients[ci].connect = false;
#ifdef USE_CONSOLE_WINDOW_PRINT
	//std::cout << "Client [" << ci << "] Disconnected!\n";
#endif
}

void ProcessPacket(int ci, unsigned char packet[])
{
	switch (packet[1]) {
	case SC_POS: 
	{
		sc_packet_pos *pos_packet = reinterpret_cast<sc_packet_pos *>(packet);
		//g_clients[ci].id = pos_packet->id;
		if (INVALID_ID == g_clients[ci].id) g_clients[ci].id = ci;
		
		/* 패킷으로 받아온 실제 ID값이 실제 클라이언트가 실행된 대수만큼 ci(더미 클라이언트의 인덱스) 값보다 커진다.
			 (예시) 더미가 아닌 클라이언트의 ID가 10번, 190번, 333번으로 3개 있다면
			 0~9번은 ci와 pos_packet->id 값이 같다.
			 10~189번은 ci 값보다 pos_packet->id 값이 1 많다.
			 190~332번은 ci 값보다 pos_packet->id 값이 2 많다.
			 333번부터는 pos_packet->id 값이 ci보다 3 많다.	 */
		if (pos_packet->id < MAX_USER && pos_packet->id >= ci) {
			g_clients[ci].x = pos_packet->x;
			g_clients[ci].y = pos_packet->y;	
#ifdef USE_CONSOLE_WINDOW_PRINT
			// printf("dummy ID [%d] / real packet Id [%d]: (%d, %d)\n", ci, pos_packet->id, g_clients[ci].x, g_clients[ci].y);
#endif
		}	
	} break;
	case SC_PUT_PLAYER: break;
	case SC_REMOVE_PLAYER: break;
	case SC_CHAT: break;
	case SC_LOGIN_OK: break;
	case SC_LOGIN_FAIL: break;
	default: 
		//MessageBox(hWnd, L"Unknown Packet Type", L"ERROR", 0);
		//while (true);
		break;
	}
}

void Worker_Thread()
{
	while (true) {
		DWORD io_size;
		unsigned long long ci;
		OverlappedEx *over;
#ifdef _WIN64	// x64 환경인 경우
		BOOL ret = GetQueuedCompletionStatus(g_hiocp, &io_size, &ci, reinterpret_cast<LPWSAOVERLAPPED *>(&over), INFINITE);
#else
		BOOL ret = GetQueuedCompletionStatus(g_hiocp, &io_size, reinterpret_cast<PULONG_PTR>(&ci), reinterpret_cast<LPWSAOVERLAPPED *>(&over), INFINITE);
#endif
		
#ifdef USE_CONSOLE_WINDOW_PRINT
		// std::cout << "GQCS :";
#endif
		if (FALSE == ret) {
			int err_no = WSAGetLastError();
			if (64 == err_no) DisconnectClient(static_cast<int>(ci));
			else error_display("GQCS : ", WSAGetLastError());
		}
		if (0 == io_size) {
			DisconnectClient(static_cast<int>(ci));
			continue;
		}
		if (OP_RECV == over->event_type) {
#ifdef USE_CONSOLE_WINDOW_PRINT
			// std::cout << "RECV from Client :" << ci;
			// std::cout << "  IO_SIZE : " << io_size << std::endl;
#endif
			unsigned char *buf = g_clients[ci].recv_over.IOCP_buf;
			unsigned psize = g_clients[ci].curr_packet_size;
			unsigned pr_size = g_clients[ci].prev_packet_data;
			while (io_size > 0) {
				if (0 == psize) psize = buf[0];
				if (io_size + pr_size >= psize) {
					// 지금 패킷 완성 가능
					unsigned char packet[MAX_PACKET_SIZE];
					memcpy(packet, g_clients[ci].packet_buf, pr_size);
					memcpy(packet + pr_size, buf, psize - pr_size);
					ProcessPacket(static_cast<int>(ci), packet);
					io_size -= psize - pr_size;
					buf += psize - pr_size;
					psize = 0; pr_size = 0;
				}
				else {
					memcpy(g_clients[ci].packet_buf + pr_size, buf, io_size);
					pr_size += io_size;
					io_size = 0;
				}
			}
			g_clients[ci].curr_packet_size = psize;
			g_clients[ci].prev_packet_data = pr_size;
			DWORD recv_flag = 0;
			int ret = WSARecv(g_clients[ci].client_socket,
				&g_clients[ci].recv_over.wsabuf, 1,
				NULL, &recv_flag, &g_clients[ci].recv_over.over, NULL);
			if (SOCKET_ERROR == ret) {
				int err_no = WSAGetLastError();
				if (err_no != WSA_IO_PENDING)
				{
					error_display("RECV ERROR", err_no);
				}
			}
		}
		else if (OP_SEND == over->event_type) {
			if (io_size != over->wsabuf.len) {
#ifdef USE_CONSOLE_WINDOW_PRINT
				std::cout << "Send Incomplete Error!\n";
#endif
				closesocket(g_clients[ci].client_socket);
				g_clients[ci].connect = false;
			}
			delete over;
		}
		else if (OP_DO_MOVE == over->event_type) {
			// Not Implemented Yet
			delete over;
		}
		else {
#ifdef USE_CONSOLE_WINDOW_PRINT
			std::cout << "Unknown GQCS event!\n";
#endif
			while (true);
		}
	}
}

void SendPacket(int cl, void *packet);

void Adjust_Number_Of_Client(int command)
{
	if (num_connections >= MAX_TEST) return;
	if (high_resolution_clock::now() < last_connect_time + 100ms) return;

	g_clients[num_connections].client_socket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	
	SOCKADDR_IN ServerAddr;
	ZeroMemory(&ServerAddr, sizeof(SOCKADDR_IN));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(SERVER_PORT);
	ServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");


	int Result = WSAConnect(g_clients[num_connections].client_socket, (sockaddr *)&ServerAddr, sizeof(ServerAddr), NULL, NULL, NULL, NULL);
	if (0 != Result) {
		error_display("WSAConnect : ", GetLastError());
	}

	cs_packet_move dummypacket;
	dummypacket.size = sizeof(dummypacket);
	dummypacket.type = 111;
	SendPacket(num_connections, &dummypacket);

	if (command)
	{
		cs_packet_move teleportPacket;
		teleportPacket.size = sizeof(teleportPacket);
		teleportPacket.type = CS_MOVE;
		teleportPacket.direction = TELEPORT;
		SendPacket(num_connections, &teleportPacket);
	}

	g_clients[num_connections].curr_packet_size = 0;
	g_clients[num_connections].prev_packet_data = 0;
	ZeroMemory(&g_clients[num_connections].recv_over, sizeof(g_clients[num_connections].recv_over));
	g_clients[num_connections].recv_over.event_type = OP_RECV;
	g_clients[num_connections].recv_over.wsabuf.buf =
		reinterpret_cast<CHAR *>(g_clients[num_connections].recv_over.IOCP_buf);
	g_clients[num_connections].recv_over.wsabuf.len = sizeof(g_clients[num_connections].recv_over.IOCP_buf);

	DWORD recv_flag = 0;
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(g_clients[num_connections].client_socket), g_hiocp, num_connections, 0);
	int ret = WSARecv(g_clients[num_connections].client_socket, &g_clients[num_connections].recv_over.wsabuf, 1,
		NULL, &recv_flag, &g_clients[num_connections].recv_over.over, NULL);
	if (SOCKET_ERROR == ret) {
		int err_no = WSAGetLastError();
		if (err_no != WSA_IO_PENDING)
		{
			error_display("RECV ERROR", err_no);
		}
	}
	g_clients[num_connections].connect = true;
	num_connections++;
}

void SendPacket(int cl, void *packet)
{
	int psize = reinterpret_cast<unsigned char *>(packet)[0];
	int ptype = reinterpret_cast<unsigned char *>(packet)[1];
	OverlappedEx *over = new OverlappedEx;
	over->event_type = OP_SEND;
	memcpy(over->IOCP_buf, packet, psize);
	ZeroMemory(&over->over, sizeof(over->over));
	over->wsabuf.buf = reinterpret_cast<CHAR *>(over->IOCP_buf);
	over->wsabuf.len = psize;
	int ret = WSASend(g_clients[cl].client_socket, &over->wsabuf, 1, NULL, 0,
		&over->over, NULL);
	if (0 != ret) {
		int err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no)
			error_display("Error in SendPacket:", err_no);
	}
#ifdef USE_CONSOLE_WINDOW_PRINT
	// std::cout << "Send Packet [" << ptype << "] To Client : " << cl << std::endl;
#endif
}

void Test_Thread()
{
	int command = 0;
	std::cout << "서버는 기본적으로 localhost 127.0.0.1에 접속합니다.\nHotSpot 테스트를 할 것이라면 0을, 동시접속 테스트를 할 것이라면 1을 입력하세요. (scanf 함수 사용) : ";
	std::cin >> command;

	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_int_distribution<> randomDir(0, 7);

	while (true) {
		Sleep(100);
		Adjust_Number_Of_Client(command);

		for (int i = 0; i < num_connections; ++i) {
			if (false == g_clients[i].connect) continue;
			if (g_clients[i].last_move_time + 1s > high_resolution_clock::now()) continue;
			g_clients[i].last_move_time = high_resolution_clock::now();

			cs_packet_move my_packet;
			my_packet.size = sizeof(my_packet);
			my_packet.type = CS_MOVE;
			my_packet.direction = randomDir(dre);


			SendPacket(i, &my_packet);
		}
	}
}

void InitializeNetwork()
{
	for (int i = 0; i < MAX_USER; ++i) {
		g_clients[i].connect = false;
		g_clients[i].id = INVALID_ID;
	}
	num_connections = 0;
	last_connect_time = high_resolution_clock::now();

	WSADATA	wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	g_hiocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, NULL, 0);

	for (int i = 0; i < 6; ++i)
		worker_threads.push_back(new std::thread{ Worker_Thread });

	test_thread = thread{ Test_Thread };
}

void ShutdownNetwork()
{
	test_thread.join();
	for (auto pth : worker_threads) {
		pth->join();
		delete pth;
	}
}

void Do_Network()
{
	return;
}

void GetPointCloud(int *size, float **points)
{
	for (int i = 0; i < num_connections; ++i) {
		point_cloud[i * 2] = static_cast<float>(g_clients[i].x);
		point_cloud[i * 2 + 1] = static_cast<float>(g_clients[i].y);
	}
	*size = num_connections;
	*points = point_cloud;
}

