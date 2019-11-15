#include "stdafx.h"
#include "ServerManager_2013180006JaeHwanKim.h"


ServerManager::ServerManager()
{
	hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);	// IOCP 객체 생성
	if (!NetworkInitialize()) {
		serverPrint("NetworkInitialize Error\n");
		ServerManager::~ServerManager();
		if (this) delete this;
	}
}


ServerManager::~ServerManager()
{
	workerThreads.clear();
	closesocket(listenSocket);
	WSACleanup();
	CloseHandle(hIOCP);

	for (auto & object : objects) if (object) delete object;
}

void ServerManager::Run()
{
	_wsetlocale(LC_ALL, L"korean");
	std::wcout.imbue(std::locale("korean"));

	ObjectInitialize();

	workerThreads.reserve(4);	// 쿼드 코어 기준으로 쓰레드 4개
	for (int i = 0; i < 4; ++i)
		workerThreads.emplace_back(std::thread{ RegisterWorkerThread, (LPVOID)this });
	std::thread acceptThread{ RegisterAcceptThread, (LPVOID)this };
	std::thread timerThread{ RegisterTimerThread, (LPVOID)this };
	
	timerThread.join();
	acceptThread.join();
	for (auto & thread : workerThreads) thread.join();
}

bool ServerManager::NetworkInitialize()
{
	WSADATA	wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		serverPrint("Error - Can't load 'winsock.dll' file\n");
		return false;
	}

	// 1. 소켓생성  
	// listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (listenSocket == INVALID_SOCKET) {
		serverPrint("Error - Invalid socket\n");
		return false;
	}

	// 서버정보 객체설정
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = PF_INET; // AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	// 2. 소켓설정
	// bind가 기본 C++ 키워드가 되었으므로 ::을 붙여서 함수임을 표기한다.
	if (::bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR) {
		serverPrint("Error - Fail bind\n");
		return false;
	}

	// 3. 수신대기열생성
	if (::listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		serverPrint("Error - Fail listen\n");
		return false;
	}

	serverPrint("Server Start\n");
	return true;
}

DWORD WINAPI ServerManager::RegisterAcceptThread(LPVOID self)
{
	ServerManager* mySelf = static_cast<ServerManager*>(self);
	mySelf->AcceptThread();

	return 0;
}

void ServerManager::AcceptThread()
{
	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(SOCKADDR_IN);
	memset(&clientAddr, 0, addrLen);
	SOCKET clientSocket;
	DWORD flags;

	while (true) {
		clientSocket = accept(listenSocket, (struct sockaddr *)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET) {
			serverPrint("Error - Accept Failure\n");
			return;
			// continue;
		}

		unsigned int new_id = GetNewID();

		// 클라이언트 구조체 초기화
		if (new_id < MAX_USER && new_id >= 0) {
			clients[new_id]->socket = clientSocket;
			// ZeroMemory(&clients[new_id]->over.overlapped, sizeof(WSAOVERLAPPED));	// 초기화 안하면 제대로 오버랩I/O가 동작 안함
			clients[new_id]->over.init(); // 초기화 동작을 합쳐놓음
			clients[new_id]->over.type = OVER_EX::Type::RECV;
			clients[new_id]->x = 5;//WORLD_WIDTH / 2;
			clients[new_id]->y = 5;//WORLD_HEIGHT / 2;
			clients[new_id]->viewlist_lock.lock();
			clients[new_id]->viewlist.clear();
			clients[new_id]->viewlist_lock.unlock();
			clients[new_id]->prev_size = 0;		
		}		
		flags = 0;
	
		// IOCP 연결
		CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSocket), hIOCP, new_id, 0);

		RecvPacket(new_id);
	}
}

DWORD WINAPI ServerManager::RegisterWorkerThread(LPVOID self)
{
	ServerManager* mySelf = static_cast<ServerManager*>(self);
	mySelf->WorkerThread();

	return 0;
}

void ServerManager::WorkerThread()
{
	while (true) {
		DWORD io_byte;
		ULONG key;
		OVER_EX *lpover_ex;

#ifdef _WIN64	// x64 환경인 경우
		bool is_error = GetQueuedCompletionStatus(hIOCP, &io_byte, reinterpret_cast<PULONG_PTR>(&key), reinterpret_cast<LPWSAOVERLAPPED *>(&lpover_ex), INFINITE);
#else
		bool is_error = GetQueuedCompletionStatus(hIOCP, &io_byte, &key, reinterpret_cast<LPWSAOVERLAPPED *>(&lpover_ex), INFINITE);
#endif

		if (!is_error) serverPrint("GetQueueCompletionStatus ERROR : %d\n", WSAGetLastError());
		if (!io_byte && key < MAX_USER) DisConnect(key);

		switch (lpover_ex->type) {
		case OVER_EX::Type::NONE: // SEND
			delete lpover_ex;
			break;
		case OVER_EX::Type::RECV:
		{
			int rest_size = io_byte;
			char *ptr = lpover_ex->buffer;
			char packetSize = 0;
			if (0 < clients[key]->prev_size) packetSize = clients[key]->packet_buf[0];
			while (rest_size > 0) {
				if (packetSize == 0)
					packetSize = ptr[0];
				int required = packetSize - clients[key]->prev_size;
				if (rest_size >= required) {
					memcpy(clients[key]->packet_buf + clients[key]->prev_size, ptr, required);
					ProcessPacket(key, clients[key]->packet_buf);
					rest_size -= required;
					ptr += required;
					packetSize = 0; // 패킷 처리가 끝남
				}
				else {
					// 패킷을 더 이상 만들 수가 없다
					memcpy(clients[key]->packet_buf + clients[key]->prev_size, ptr, rest_size); // 포인터에 들어있는 내용을 몽땅 다 쏟아붓는다
					rest_size = 0;
				}
			}
			RecvPacket(key);
		}
			break;
		case OVER_EX::Type::EVENT:
		{
			TimerEvent* pEvent = reinterpret_cast<TimerEvent*>(lpover_ex->buffer);
			pEvent->objectID = key;
			ProcessEvent(*pEvent);
			delete lpover_ex;
		}
			break;
		default:
			serverPrint("Unknown Packet Type\n");
			// 오버랩구조체 delete
			delete lpover_ex;
			break;
		}
	}
}

DWORD ServerManager::RegisterTimerThread(LPVOID self)
{
	ServerManager* mySelf = static_cast<ServerManager*>(self);
	mySelf->TimerThread();

	return 0;
}

void ServerManager::TimerThread()
{
	do {
		Sleep(10); // 1ms 강제로 쉬기
		do {
			timerQueue_Lock.lock();
			if (!timerQueue.empty()) {
				TimerEvent peek = timerQueue.top();
				timerQueue_Lock.unlock();
				std::chrono::duration<double> currentTime = std::chrono::high_resolution_clock::now().time_since_epoch();
				if (peek.startTime > currentTime.count()) break; 
				timerQueue_Lock.lock();
				timerQueue.pop();
				timerQueue_Lock.unlock();

				OVER_EX* lpover_ex = new OVER_EX;
				memcpy(lpover_ex->buffer, &peek, sizeof(TimerEvent));
				lpover_ex->init();
				lpover_ex->dataBuffer.len = sizeof(TimerEvent);
				lpover_ex->type = OVER_EX::Type::EVENT;

				PostQueuedCompletionStatus(hIOCP, 0, peek.objectID, reinterpret_cast<WSAOVERLAPPED*>(lpover_ex));
			}
			else
				timerQueue_Lock.unlock();
		} while (true);
	} while (true);
}

void ServerManager::ObjectInitialize()
{
	for (int i = 0; i < MAX_USER; ++i) {
		clients[i] = new Player;
		clients[i]->connected = false;
		clients[i]->viewlist.clear();
		objects[i] = clients[i];	// object 포인터를 player로 맞춰주고 업캐스팅으로 공유한다. 나중에 객체 삭제할 때 object만 삭제하면 된다.
	}
	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_int_distribution<> startX(0, WORLD_WIDTH - 1);
	std::uniform_int_distribution<> startY(0, WORLD_HEIGHT - 1);
	std::uniform_int_distribution<> startDir(0, 7);
	for (int i = NPC_ID_START; i < NPC_ID_START + NUM_NPC; ++i) {
		objects[i] = new NPC;
		NPC* npc = reinterpret_cast<NPC*>(objects[i]);
		npc->is_sleeping = true;
		npc->start_x = npc->x = startX(dre);
		npc->start_y = npc->y = startY(dre);

		// area Lv 1
		if (npc->start_x < WORLD_WIDTH / 2 && npc->start_y < WORLD_HEIGHT / 2) {
			npc->Type = NPC::State::Peace;
			npc->CharcterType = FLYBUG;
			npc->dir = startDir(dre);
			npc->LEVEL = 1;
			npc->EXP = 10;
			npc->HP = npc->MAX_HP = 15;
			npc->data.MP = npc->data.MAX_MP = 5;
			npc->data.SP = npc->data.MAX_SP = 10;
			npc->data.ATK = 15;
			npc->data.DEF = 1;
			npc->data.MAG = 25;
			npc->data.MR = 1;		
		}
		// area Lv 2-1
		else if (npc->start_x >= WORLD_WIDTH / 2 && npc->start_y < WORLD_HEIGHT / 2) {
			npc->Type = NPC::State::Peace;
			npc->CharcterType = SKULLGOLEM;
			npc->dir = startDir(dre);
			npc->LEVEL = 6;
			npc->EXP = 255;
			npc->HP = npc->MAX_HP = 75;
			npc->data.MP = npc->data.MAX_MP = 35;
			npc->data.SP = npc->data.MAX_SP = 50;
			npc->data.ATK = 25;
			npc->data.DEF = 15;
			npc->data.MAG = 3;
			npc->data.MR = 5;
		}
		// arear Lv 2-2
		else if (npc->start_x < WORLD_WIDTH / 2 && npc->start_y >= WORLD_HEIGHT / 2) {
			npc->Type = NPC::State::War;
			npc->CharcterType = CRAB;
			npc->dir = startDir(dre);
			npc->LEVEL = 3;
			npc->EXP = 75;
			npc->HP = npc->MAX_HP = 50;
			npc->data.MP = npc->data.MAX_MP = 35;
			npc->data.SP = npc->data.MAX_SP = 25;
			npc->data.ATK = 20;
			npc->data.DEF = 5;
			npc->data.MAG = 25;
			npc->data.MR = 10;
		}
		// area Lv Final
		else if (npc->start_x >= WORLD_WIDTH * 3 / 4 && npc->start_y >= WORLD_HEIGHT * 3 / 4) {
			npc->Type = NPC::State::War;
			npc->CharcterType = SLASHER;
			npc->dir = startDir(dre);
			npc->LEVEL = 10;
			npc->EXP = 5000;
			npc->HP = npc->MAX_HP = 175;
			npc->data.MP = npc->data.MAX_MP = 100;
			npc->data.SP = npc->data.MAX_SP = 150;
			npc->data.ATK = 40;
			npc->data.DEF = 25;
			npc->data.MAG = 40;
			npc->data.MR = 25;
		}
		// area Lv 3
		else if (npc->start_x >= WORLD_WIDTH / 2&& npc->start_y >= WORLD_HEIGHT / 2) {
			npc->Type = NPC::State::War;
			npc->CharcterType = SPEAR;
			npc->dir = startDir(dre);
			npc->LEVEL = 5;
			npc->EXP = 150;
			npc->HP = npc->MAX_HP = 50;
			npc->data.MP = npc->data.MAX_MP = 35;
			npc->data.SP = npc->data.MAX_SP = 25;
			npc->data.ATK = 25;
			npc->data.DEF = 10;
			npc->data.MAG = 15;
			npc->data.MR = 20;
		}
	}
	for (int i = SMART_NPC_START; i < SMART_NPC_START + NUM_SMART_NPC; ++i)
	{
		objects[i] = new SMART_NPC;
		SMART_NPC* npc = reinterpret_cast<SMART_NPC*>(objects[i]);
		npc->is_sleeping = true;
		npc->start_x = npc->x = startX(dre);
		npc->start_y = npc->y = startY(dre);
		npc->L = luaL_newstate();
		luaL_openlibs(npc->L);
		int error = luaL_loadfile(npc->L, "monster_ai.lua");
		if (error) error_display(npc->L);
		error = lua_pcall(npc->L, 0, 0, 0);
		if (error) error_display(npc->L);

		lua_register(npc->L, "API_get_x", API_get_x);
		lua_register(npc->L, "API_get_y", API_get_y);
		lua_register(npc->L, "API_SendMessage", API_Send_Message);
		lua_register(npc->L, "API_AddFleeEvent", API_AddFleeEvent);
		lua_register(npc->L, "API_SetPosition", API_SetPosition);

		lua_getglobal(npc->L, "set_uid");
		lua_pushnumber(npc->L, i);
		error = lua_pcall(npc->L, 1, 0, 0);
		if (error) error_display(npc->L);
	}
	serverPrint("All NPCs Lua Initialize Complete\n");
}

void ServerManager::SendPacket(unsigned int id, char * packet)
{
	SOCKET client_s = clients[id]->socket;

	OVER_EX *over = reinterpret_cast<OVER_EX *>(malloc(sizeof(OVER_EX)));
	
	over->dataBuffer.len = packet[0];
	over->dataBuffer.buf = over->buffer;
	memcpy(over->buffer, packet, packet[0]);
	::ZeroMemory(&(over->overlapped), sizeof(WSAOVERLAPPED));
	over->type = OVER_EX::Type::NONE;
	if (WSASend(client_s, &over->dataBuffer, 1, NULL, 0, &(over->overlapped), NULL) == SOCKET_ERROR)
		if (WSAGetLastError() != WSA_IO_PENDING)
			serverPrint("Send Error = Fail WSASend(error_code : %d)\n", WSAGetLastError());
}

void ServerManager::RecvPacket(unsigned int id)
{
	DWORD flags = 0;

	SOCKET client_s = clients[id]->socket;
	OVER_EX *over = &clients[id]->over;

	over->dataBuffer.len = BUFSIZE;
	over->dataBuffer.buf = over->buffer;
	memset(&(over->overlapped), 0x00, sizeof(WSAOVERLAPPED));

	if (WSARecv(client_s, &over->dataBuffer, 1, NULL, &flags, &(over->overlapped), NULL) == SOCKET_ERROR)
		if (WSAGetLastError() != WSA_IO_PENDING)
			serverPrint("Recv Error = Fail WSARecv(error_code : %d)\n", WSAGetLastError());
}

void ServerManager::SendLoginOKPacket(unsigned int to)
{
	sc_packet_login_ok packet;
	packet.id = to;
	packet.size = sizeof(packet);
	packet.type = SC_LOGIN_OK;
	packet.playerType = clients[to]->CharcterType;
	packet.LEVEL = clients[to]->LEVEL;
	packet.HP = clients[to]->HP;
	packet.MAX_HP = clients[to]->MAX_HP;
	packet.MP = clients[to]->status.MP;
	packet.MAX_MP = clients[to]->status.MAX_MP;
	packet.SP = clients[to]->status.SP;
	packet.MAX_SP = clients[to]->status.MAX_SP;
	packet.EXP = clients[to]->EXP;
	packet.MAX_EXP = clients[to]->status.MAX_EXP;
	packet.ATK = clients[to]->status.ATK;
	packet.DEF = clients[to]->status.DEF;
	packet.MAG = clients[to]->status.MAG;
	packet.MR = clients[to]->status.MR;
	packet.Ability_Point = clients[to]->status.Ability_Point;
	packet.Skill_Point = clients[to]->status.Skill_Point;
	SendPacket(to, reinterpret_cast<char *>(&packet));
}

void ServerManager::SendAddObjectPacket(unsigned int to, unsigned int obj)
{
	sc_packet_add_object packet;
	packet.id = obj;
	packet.size = sizeof(packet);
	packet.type = SC_PUT_PLAYER;
	packet.LEVEL = objects[obj]->LEVEL;
	packet.EXP = objects[obj]->EXP;
	packet.obj_class = objects[obj]->CharcterType;
	packet.x = objects[obj]->x;
	packet.y = objects[obj]->y;
	packet.HP = objects[obj]->HP;
	packet.MAX_HP = objects[obj]->MAX_HP;
	SendPacket(to, reinterpret_cast<char *>(&packet));
}

void ServerManager::SendPositionPacket(unsigned int to, unsigned int obj)
{
	// obj가 움직였다고 to 소켓에다 보내줘야 한다.
	sc_packet_pos packet;
	packet.id = obj;
	packet.size = sizeof(packet);
	packet.type = SC_POS;
	packet.x = objects[obj]->x;
	packet.y = objects[obj]->y;
	SendPacket(to, reinterpret_cast<char *>(&packet));
}

void ServerManager::SendRemoveObjectPacket(unsigned int to, unsigned int id)
{
	sc_packet_remove_object packet;
	packet.id = id;
	packet.size = sizeof(packet);
	packet.type = SC_REMOVE_OBJECT;
	SendPacket(to, reinterpret_cast<char *>(&packet));
}

void ServerManager::SendStatChangePacket(unsigned int to)
{
	sc_packet_stat_change packet;
	packet.id = to;
	packet.size = sizeof(packet);
	packet.type = SC_STAT_CHANGE;
	packet.LEVEL = clients[to]->LEVEL;
	packet.HP = clients[to]->HP;
	packet.MAX_HP = clients[to]->MAX_HP;
	packet.MP = clients[to]->status.MP;
	packet.MAX_MP = clients[to]->status.MAX_MP;
	packet.SP = clients[to]->status.SP;
	packet.MAX_SP = clients[to]->status.MAX_SP;
	packet.EXP = clients[to]->EXP;
	packet.MAX_EXP = clients[to]->status.MAX_EXP;
	packet.ATK = clients[to]->status.ATK;
	packet.DEF = clients[to]->status.DEF;
	packet.MAG = clients[to]->status.MAG;
	packet.MR = clients[to]->status.MR;
	packet.Ability_Point = clients[to]->status.Ability_Point;
	packet.Skill_Point = clients[to]->status.Skill_Point;
	SendPacket(to, reinterpret_cast<char *>(&packet));
}

void ServerManager::SendSetDBData(unsigned int id)
{
	if (QueryServer >= 0) {
		SD_SetDBDataPacket packet;
		wcsncpy_s(packet.user_id, clients[id]->user_id, NCHAR_LENGTH);
		serverPrint("%LS <- %LS\n", packet.user_id, clients[id]->user_id);
		packet.id = id;
		packet.size = sizeof(packet);
		packet.type = SD_Type::SetDBData;
		packet.PlayerType = clients[id]->CharcterType;
		packet.LEVEL = clients[id]->LEVEL;
		packet.HP = clients[id]->HP;
		packet.MAX_HP = clients[id]->MAX_HP;
		packet.MP = clients[id]->status.MP;
		packet.MAX_MP = clients[id]->status.MAX_MP;
		packet.SP = clients[id]->status.SP;
		packet.MAX_SP = clients[id]->status.MAX_SP;
		packet.EXP = clients[id]->EXP;
		packet.MAX_EXP = clients[id]->status.MAX_EXP;
		packet.ATK = clients[id]->status.ATK;
		packet.DEF = clients[id]->status.DEF;
		packet.MAG = clients[id]->status.MAG;
		packet.MR = clients[id]->status.MR;
		packet.Ability_Point = clients[id]->status.Ability_Point;
		packet.Skill_Point = clients[id]->status.Skill_Point;
		packet.Position_x = clients[id]->x;
		packet.Position_y = clients[id]->y;
		SendPacket(QueryServer, reinterpret_cast<char *>(&packet));
	}
}

void ServerManager::SendChatPacket(unsigned int to, unsigned int from, wchar_t * message)
{
	sc_packet_chat packet;
	packet.id = from;
	packet.size = sizeof(packet);
	packet.type = SC_CHAT;
	wcsncpy_s(packet.message, message, MAX_STR_LEN);
	SendPacket(to, reinterpret_cast<char *>(&packet));
}

void ServerManager::ProcessPacket(unsigned int id, char * buf)
{
	cs_packet_logout * packet = reinterpret_cast<cs_packet_logout *>(buf);

	short x = objects[id]->x;
	short y = objects[id]->y;
	switch (packet->type) {
	case CS_LOGIN:
	{
		cs_packet_login *myPacket = reinterpret_cast<cs_packet_login *>(buf);

		for (int i = 0; i < MAX_USER; ++i) {
			if (!clients[i]->connected) continue;
			if (!wcsncmp(clients[i]->user_id, myPacket->player_id, NCHAR_LENGTH)) {
				sc_packet_login_fail loginFailPacket;
				loginFailPacket.size = sizeof(loginFailPacket);
				loginFailPacket.type = SC_LOGIN_FAIL;
				SendPacket(id, reinterpret_cast<char *>(&loginFailPacket));
				return;
			}
		}
		wcsncpy_s(clients[id]->user_id, myPacket->player_id, NCHAR_LENGTH);

		DBUserKeyPacket db_send_packet;
		wcsncpy_s(db_send_packet.user_id, myPacket->player_id, NCHAR_LENGTH);
		db_send_packet.id = id;
		db_send_packet.size = sizeof(db_send_packet);
		db_send_packet.type = SD_Type::Login;
		if (QueryServer >= 0)
			SendPacket(QueryServer, reinterpret_cast<char *>(&db_send_packet));
	}
	break;
	case CS_MOVE:
	{
		cs_packet_move * move_packet = reinterpret_cast<cs_packet_move *>(buf);

		switch (move_packet->direction) {
		case DIR_UP:
			if (y < WORLD_HEIGHT - 1) ++y;
			break;
		case DIR_DOWN:
			if (y > 0) --y;
			break;
		case DIR_LEFT:
			if (x > 0) --x;
			break;
		case DIR_RIGHT:
			if (x < WORLD_WIDTH - 1) ++x;
			break;
		case DIR_LEFT_UP:
			if (x > 0) --x;
			if (y < WORLD_HEIGHT - 1) ++y;
			break;
		case DIR_LEFT_DOWN:
			if (x > 0) --x;
			if (y > 0) --y;
			break;
		case DIR_RIGHT_UP:
			if (x < WORLD_WIDTH - 1) ++x;
			if (y < WORLD_HEIGHT - 1) ++y;
			break;
		case DIR_RIGHT_DOWN:
			if (x < WORLD_WIDTH - 1) ++x;
			if (y > 0) --y;
			break;
		case TELEPORT: {
			std::random_device rd;
			std::default_random_engine dre(rd());
			std::uniform_int_distribution<> startX(0, WORLD_WIDTH - 1);
			std::uniform_int_distribution<> startY(0, WORLD_HEIGHT - 1);
			x = startX(dre);
			y = startY(dre);
		}
			break;
		default:
			serverPrint("Unknown Move Direction\n");
			break;
		}


		objects[id]->x = x;
		objects[id]->y = y;

		// View Process
		if (isPlayer(id)) {
			clients[id]->viewlist_lock.lock();
			std::unordered_set <unsigned int> old_myViewlist = clients[id]->viewlist;	// 나의 이전 뷰 리스트
			clients[id]->viewlist_lock.unlock();
			std::unordered_set <unsigned int> new_myViewlist; // 갱신된 현재 시야의 뷰 리스트
			for (unsigned int i = 0; i < MAX_USER; ++i)
				if (clients[i]->connected && isNearObject(id, i) && i != id)// 나의 새로운 뷰 리스트에 일단 주위에 있는 플레이어들만 싹 다 추가한다. 애초에 주위에 없으면 검사할 필요가 없으니까!
					new_myViewlist.insert(i);
			for (unsigned int i = 0; i < NUM_NPC; ++i) {
				unsigned int npc_id = i + NPC_ID_START;
				if (isNearObject(id, npc_id))
					new_myViewlist.insert(npc_id); // 나의 시야에 NPC 추가
			}

			for (auto other_client : new_myViewlist) {
				if (old_myViewlist.count(other_client)) {	// old vl, 즉 나의 뷰 리스트(현재 시야)내에 클라이언트가 존재하고 있다. old, new 동시 존재
					if (!isPlayer(other_client)) continue;
					clients[other_client]->viewlist_lock.lock();
					if (clients[other_client]->viewlist.count(id)) { // 상대방 뷰리스트에 내가 존재하고 있나?
						clients[other_client]->viewlist_lock.unlock();
						SendPositionPacket(other_client, id); // 이미 존재하면 Put 할 필요가 없다. 그냥 이동하자.
					}
					else {
						clients[other_client]->viewlist.insert(id); // 존재하지 않으면 내 ID를 상대방의 뷰 리스트에 알려주자.
						clients[other_client]->viewlist_lock.unlock();
						SendAddObjectPacket(other_client, id);	// 상대방 뷰리스트에 내가 없었으면 추가해야지.
					}
				}
				else {	// 나의 뷰 리스트에 상대방이 없었다. 즉 새로 상대방이 시야에 들어왔다.
					clients[id]->viewlist_lock.lock();
					clients[id]->viewlist.insert(other_client); // 나의 시야에 상대방이 새로 들어왔으니 뷰 리스트에 상대방을 추가한다.
					clients[id]->viewlist_lock.unlock();
					SendAddObjectPacket(id, other_client);	// 새로 들어온 상대방 정보를 클라이언트(나)에게 전송한다.
					if (!isPlayer(other_client)) {
						WakeUpNPC(other_client);
						continue;
					}
					clients[other_client]->viewlist_lock.lock();
					if (clients[other_client]->viewlist.count(id)) {	// 나는 상대방을 몰랐는데, 상대방은 이미 나를 알고 있는가?
						clients[other_client]->viewlist_lock.unlock();
						SendPositionPacket(other_client, id); // 그럼 그냥 이동 패킷만 보낸다.
					}
					else {	// 상대방이 날 몰랐으면 상대방에게 날 알려줘야지.
						clients[other_client]->viewlist.insert(id); // 상대방의 시야 리스트에 내 ID를 추가한다.
						clients[other_client]->viewlist_lock.unlock();
						SendAddObjectPacket(other_client, id); // 상대방에게 나의 정보를 보낸다.
					}
				}
			}

			for (auto other_client : old_myViewlist) { 	// 상대방이 내 시야에서 사라졌다.		
				if (new_myViewlist.count(other_client)) continue;	// 이미 위에서 처리를 했으니 할 게 없다. 컨티뉴
				clients[id]->viewlist_lock.lock();
				clients[id]->viewlist.erase(other_client);	// 위의 두 조건문을 통과했다면 나의 뷰 리스트에서 사라진 상대방을 제거한다.
				clients[id]->viewlist_lock.unlock();
				SendRemoveObjectPacket(id, other_client);
				if (!isPlayer(other_client)) continue;
				clients[other_client]->viewlist_lock.lock();
				if (clients[other_client]->viewlist.count(id)) {	// 상대방 뷰 리스트에 내가 있었는가?
					clients[other_client]->viewlist.erase(id);	// 내가 있었으면 내 정보를 상대방 시야에서 지우자..
					clients[other_client]->viewlist_lock.unlock();
					SendRemoveObjectPacket(other_client, id); // 상대방에게 내 정보를 지웠다고 알려주자.
				}
				else
					clients[other_client]->viewlist_lock.unlock();
			}
			SendPositionPacket(id, id);	// 나한테 내 이동 정보를 보낸다.
			
										
			// 나의 시야 안에 있는 NPC들의 ID를 검사한다.
			for (auto npc_id : new_myViewlist) { 
				if (isPlayer(npc_id)) continue;
				std::chrono::duration<double> currentTime = std::chrono::high_resolution_clock::now().time_since_epoch();
				TimerEvent ev(npc_id, TimerEvent::Command::Player_Moved, currentTime.count());
				ev.targetID = id;
				AddTimerEvent(ev);
			}
		}
	}
	break;
	case CS_CHAT:
	{
		cs_packet_chat * my_chat_packet = reinterpret_cast<cs_packet_chat *>(buf);
		wchar_t message[MAX_STR_LEN];
		wsprintf(message, TEXT("[%d] : %s"),id, my_chat_packet->message);
		for (int i = 0; i < MAX_USER; ++i) {
			if (!clients[i]->connected) continue;
			// SendChatPacket(i, id, message);
			SendChatPacket(i, id, my_chat_packet->message);
		}
	}
	break;
	case CS_ATTACK:
	{
		if (isPlayer(id)) {
			clients[id]->viewlist_lock.lock();
			std::unordered_set <unsigned int> myViewlist = clients[id]->viewlist;	// 나의 이전 뷰 리스트
			clients[id]->viewlist_lock.unlock();
			cs_packet_attack * attack_packet = reinterpret_cast<cs_packet_attack *>(buf);

			switch (attack_packet->direction) {
			case DIR_UP:
				for (auto obj : myViewlist) {
					if (isPlayer(obj)) continue;
					if (objects[obj]->y == clients[id]->y + 1 && objects[obj]->x == clients[id]->x) {
						NPC* npc = reinterpret_cast<NPC*> (objects[obj]);
						int dmg = max(1, clients[id]->status.ATK - npc->data.DEF);
						npc->HP -= dmg;
						wchar_t system_message[MAX_STR_LEN];
						wsprintf(system_message, TEXT("[%d]상대는 %d의 피해를 입었다! "), obj, dmg);
						SendChatPacket(id, id, system_message);

						if (npc->Type == NPC::State::Peace) {
							npc->BeforeType = npc->Type;
							std::chrono::duration<double> currentTime = std::chrono::high_resolution_clock::now().time_since_epoch();
							TimerEvent ev(obj, TimerEvent::Command::Chase, currentTime.count());
							ev.targetID = id;
							AddTimerEvent(ev);
							npc->Type = NPC::State::Chase;
						}
						if (npc->HP <= 0) {
							clients[id]->EXP += npc->EXP;
							npc->HP = npc->MAX_HP;
							npc->x = npc->start_x;
							npc->y = npc->start_y;
							wchar_t system_message2[MAX_STR_LEN];
							wsprintf(system_message2, TEXT("적을 처치하여 EXP %d를 획득하였다!"), npc->EXP);
							SendChatPacket(id, id, system_message2);
							SendSetDBData(id);
						}
					}
				}
				break;
			case DIR_DOWN:
				for (auto obj : myViewlist) {
					if (isPlayer(obj)) continue;
					if (objects[obj]->y == clients[id]->y - 1 && objects[obj]->x == clients[id]->x) {
						NPC* npc = reinterpret_cast<NPC*> (objects[obj]);
						int dmg = max(1, clients[id]->status.ATK - npc->data.DEF);
						npc->HP -= dmg;
						wchar_t system_message[MAX_STR_LEN];
						wsprintf(system_message, TEXT("[%d]상대는 %d의 피해를 입었다! "), obj, dmg);
						SendChatPacket(id, id, system_message);

						if (npc->Type == NPC::State::Peace) {
							npc->BeforeType = npc->Type;
							std::chrono::duration<double> currentTime = std::chrono::high_resolution_clock::now().time_since_epoch();
							TimerEvent ev(obj, TimerEvent::Command::Chase, currentTime.count());
							ev.targetID = id;
							AddTimerEvent(ev);
							npc->Type = NPC::State::Chase;
						}
						if (npc->HP <= 0) {
							clients[id]->EXP += npc->EXP;
							npc->HP = npc->MAX_HP;
							npc->x = npc->start_x;
							npc->y = npc->start_y;
							wchar_t system_message2[MAX_STR_LEN];
							wsprintf(system_message2, TEXT("적을 처치하여 EXP %d를 획득하였다!"), npc->EXP);
							SendChatPacket(id, id, system_message2);
							SendSetDBData(id);
						}
					}
				}
				break;
			case DIR_LEFT:
				for (auto obj : myViewlist) {
					if (isPlayer(obj)) continue;
					if (objects[obj]->y == clients[id]->y && objects[obj]->x == clients[id]->x - 1) {
						NPC* npc = reinterpret_cast<NPC*> (objects[obj]);
						int dmg = max(1, clients[id]->status.ATK - npc->data.DEF);
						npc->HP -= dmg;
						wchar_t system_message[MAX_STR_LEN];
						wsprintf(system_message, TEXT("[%d]상대는 %d의 피해를 입었다! "), obj, dmg);
						SendChatPacket(id, id, system_message);

						if (npc->Type == NPC::State::Peace) {
							npc->BeforeType = npc->Type;
							std::chrono::duration<double> currentTime = std::chrono::high_resolution_clock::now().time_since_epoch();
							TimerEvent ev(obj, TimerEvent::Command::Chase, currentTime.count());
							ev.targetID = id;
							AddTimerEvent(ev);
							npc->Type = NPC::State::Chase;
						}
						if (npc->HP <= 0) {
							clients[id]->EXP += npc->EXP;
							npc->HP = npc->MAX_HP;
							npc->x = npc->start_x;
							npc->y = npc->start_y;
							wchar_t system_message2[MAX_STR_LEN];
							wsprintf(system_message2, TEXT("적을 처치하여 EXP %d를 획득하였다!"), npc->EXP);
							SendChatPacket(id, id, system_message2);
							SendSetDBData(id);
						}
					}
				}
				break;
			case DIR_RIGHT:
				for (auto obj : myViewlist) {
					if (isPlayer(obj)) continue;
					if (objects[obj]->y == clients[id]->y && objects[obj]->x == clients[id]->x + 1) {
						NPC* npc = reinterpret_cast<NPC*> (objects[obj]);
						int dmg = max(1, clients[id]->status.ATK - npc->data.DEF);
						npc->HP -= dmg;
						wchar_t system_message[MAX_STR_LEN];
						wsprintf(system_message, TEXT("[%d]상대는 %d의 피해를 입었다! "), obj, dmg);
						SendChatPacket(id, id, system_message);

						if (npc->Type == NPC::State::Peace) {
							npc->BeforeType = npc->Type;
							std::chrono::duration<double> currentTime = std::chrono::high_resolution_clock::now().time_since_epoch();
							TimerEvent ev(obj, TimerEvent::Command::Chase, currentTime.count());
							ev.targetID = id;
							AddTimerEvent(ev);
							npc->Type = NPC::State::Chase;
						}
						if (npc->HP <= 0) {
							clients[id]->EXP += npc->EXP;
							npc->HP = npc->MAX_HP;
							npc->x = npc->start_x;
							npc->y = npc->start_y;
							wchar_t system_message2[MAX_STR_LEN];
							wsprintf(system_message2, TEXT("적을 처치하여 EXP %d를 획득하였다!"), npc->EXP);
							SendChatPacket(id, id, system_message2);
							SendSetDBData(id);
						}
					}
				}
				break;
			case DIR_LEFT_UP:
				for (auto obj : myViewlist) {
					if (isPlayer(obj)) continue;
					if (objects[obj]->y == clients[id]->y + 1 && objects[obj]->x == clients[id]->x - 1) {
						NPC* npc = reinterpret_cast<NPC*> (objects[obj]);
						int dmg = max(1, clients[id]->status.ATK - npc->data.DEF);
						npc->HP -= dmg;
						wchar_t system_message[MAX_STR_LEN];
						wsprintf(system_message, TEXT("[%d]상대는 %d의 피해를 입었다! "), obj, dmg);
						SendChatPacket(id, id, system_message);

						if (npc->Type == NPC::State::Peace) {
							npc->BeforeType = npc->Type;
							std::chrono::duration<double> currentTime = std::chrono::high_resolution_clock::now().time_since_epoch();
							TimerEvent ev(obj, TimerEvent::Command::Chase, currentTime.count());
							ev.targetID = id;
							AddTimerEvent(ev);
							npc->Type = NPC::State::Chase;
						}
						if (npc->HP <= 0) {
							clients[id]->EXP += npc->EXP;
							npc->HP = npc->MAX_HP;
							npc->x = npc->start_x;
							npc->y = npc->start_y;
							wchar_t system_message2[MAX_STR_LEN];
							wsprintf(system_message2, TEXT("적을 처치하여 EXP %d를 획득하였다!"), npc->EXP);
							SendChatPacket(id, id, system_message2);
							SendSetDBData(id);
						}
					}
				}
				break;
			case DIR_LEFT_DOWN:
				for (auto obj : myViewlist) {
					if (isPlayer(obj)) continue;
					if (objects[obj]->y == clients[id]->y - 1 && objects[obj]->x == clients[id]->x - 1) {
						NPC* npc = reinterpret_cast<NPC*> (objects[obj]);
						int dmg = max(1, clients[id]->status.ATK - npc->data.DEF);
						npc->HP -= dmg;
						wchar_t system_message[MAX_STR_LEN];
						wsprintf(system_message, TEXT("[%d]상대는 %d의 피해를 입었다! "), obj, dmg);
						SendChatPacket(id, id, system_message);

						if (npc->Type == NPC::State::Peace) {
							npc->BeforeType = npc->Type;
							std::chrono::duration<double> currentTime = std::chrono::high_resolution_clock::now().time_since_epoch();
							TimerEvent ev(obj, TimerEvent::Command::Chase, currentTime.count());
							ev.targetID = id;
							AddTimerEvent(ev);
							npc->Type = NPC::State::Chase;
						}
						if (npc->HP <= 0) {
							clients[id]->EXP += npc->EXP;
							npc->HP = npc->MAX_HP;
							npc->x = npc->start_x;
							npc->y = npc->start_y;
							wchar_t system_message2[MAX_STR_LEN];
							wsprintf(system_message2, TEXT("적을 처치하여 EXP %d를 획득하였다!"), npc->EXP);
							SendChatPacket(id, id, system_message2);
							SendSetDBData(id);
						}
					}
				}
				break;
			case DIR_RIGHT_UP:
				for (auto obj : myViewlist) {
					if (isPlayer(obj)) continue;
					if (objects[obj]->y == clients[id]->y + 1 && objects[obj]->x == clients[id]->x + 1) {
						NPC* npc = reinterpret_cast<NPC*> (objects[obj]);
						int dmg = max(1, clients[id]->status.ATK - npc->data.DEF);
						npc->HP -= dmg;
						wchar_t system_message[MAX_STR_LEN];
						wsprintf(system_message, TEXT("[%d]상대는 %d의 피해를 입었다! "), obj, dmg);
						SendChatPacket(id, id, system_message);

						if (npc->Type == NPC::State::Peace) {
							npc->BeforeType = npc->Type;
							std::chrono::duration<double> currentTime = std::chrono::high_resolution_clock::now().time_since_epoch();
							TimerEvent ev(obj, TimerEvent::Command::Chase, currentTime.count());
							ev.targetID = id;
							AddTimerEvent(ev);
							npc->Type = NPC::State::Chase;
						}
						if (npc->HP <= 0) {
							clients[id]->EXP += npc->EXP;
							npc->HP = npc->MAX_HP;
							npc->x = npc->start_x;
							npc->y = npc->start_y;
							wchar_t system_message2[MAX_STR_LEN];
							wsprintf(system_message2, TEXT("적을 처치하여 EXP %d를 획득하였다!"), npc->EXP);
							SendChatPacket(id, id, system_message2);
							SendSetDBData(id);
						}
					}
				}
				break;
			case DIR_RIGHT_DOWN:
				for (auto obj : myViewlist) {
					if (isPlayer(obj)) continue;
					if (objects[obj]->y == clients[id]->y - 1 && objects[obj]->x == clients[id]->x + 1) {
						NPC* npc = reinterpret_cast<NPC*> (objects[obj]);
						int dmg = max(1, clients[id]->status.ATK - npc->data.DEF);
						npc->HP -= dmg;
						wchar_t system_message[MAX_STR_LEN];
						wsprintf(system_message, TEXT("[%d]상대는 %d의 피해를 입었다! "), obj, dmg);
						SendChatPacket(id, id, system_message);

						if (npc->Type == NPC::State::Peace) {
							npc->BeforeType = npc->Type;
							std::chrono::duration<double> currentTime = std::chrono::high_resolution_clock::now().time_since_epoch();
							TimerEvent ev(obj, TimerEvent::Command::Chase, currentTime.count());
							ev.targetID = id;
							AddTimerEvent(ev);
							npc->Type = NPC::State::Chase;
						}
						if (npc->HP <= 0) {
							clients[id]->EXP += npc->EXP;
							npc->HP = npc->MAX_HP;
							npc->x = npc->start_x;
							npc->y = npc->start_y;
							wchar_t system_message2[MAX_STR_LEN];
							wsprintf(system_message2, TEXT("적을 처치하여 EXP %d를 획득하였다!"), npc->EXP);
							SendChatPacket(id, id, system_message2);
							SendSetDBData(id);
						}
					}
				}
				break;
			default:
				serverPrint("Unknown Attack Direction\n");
				break;
			}
			LevelUp(id);
			
		}
	}
		break;
	case DS_Type::DBConnect:
	{
		QueryServer = id;

		clients[id]->x = - 1000;
		clients[id]->y = - 1000;
		clients[id]->connected = false;	// DB 서버는 다른 클라이언트와 다르다.
	}
		break;	
	case DS_Type::LoginSuccess:	// Login Success
	{
		volatile DS_GetUserDataPacket* user_data_packet = reinterpret_cast<DS_GetUserDataPacket*>(buf);
		unsigned int new_id = user_data_packet->id;

		clients[new_id]->CharcterType = user_data_packet->PlayerType;
		clients[new_id]->LEVEL = user_data_packet->LEVEL;
		clients[new_id]->HP = user_data_packet->HP;
		clients[new_id]->MAX_HP = user_data_packet->MAX_HP;
		clients[new_id]->status.MP = user_data_packet->MP;
		clients[new_id]->status.MAX_MP = user_data_packet->MAX_MP;
		clients[new_id]->status.SP = user_data_packet->SP;
		clients[new_id]->status.MAX_SP = user_data_packet->MAX_SP;
		clients[new_id]->EXP = user_data_packet->EXP;
		clients[new_id]->status.MAX_EXP = user_data_packet->MAX_EXP;
		clients[new_id]->status.ATK = user_data_packet->ATK;
		clients[new_id]->status.DEF = user_data_packet->DEF;
		clients[new_id]->status.MAG = user_data_packet->MAG;
		clients[new_id]->status.MR = user_data_packet->MR;
		clients[new_id]->status.Ability_Point = user_data_packet->Ability_Point;
		clients[new_id]->status.Skill_Point = user_data_packet->Skill_Point;
		clients[new_id]->x = user_data_packet->Position_x;
		clients[new_id]->y = user_data_packet->Position_y;

		SendLoginOKPacket(new_id);
		SendAddObjectPacket(new_id, new_id); // 나 자신에게 나의 등장을 먼저 미리 알려준다.

		// 다른 플레이어에게 나의 등장을 알려줌
		for (int i = 0; i < MAX_USER; ++i) {
			if (!clients[i]->connected) continue;
			if (i == new_id) continue; // 나 자신에게 나를 알려줄 필요는 없다.
			if (isNearObject(i, new_id)) {
				clients[i]->viewlist_lock.lock();
				clients[i]->viewlist.insert(new_id);
				clients[i]->viewlist_lock.unlock();
				SendAddObjectPacket(i, new_id);
			}
		}

		// 다른 플레이어 추가
		std::unordered_set<unsigned int> other_vl;
		for (unsigned int i = 0; i < MAX_USER; ++i) {
			if (!clients[i]->connected) continue;
			if (i == new_id) continue;
			if (isNearObject(i, new_id)) {
				other_vl.insert(i);
				SendAddObjectPacket(new_id, i);
			}
		}

		//플레이어의 시야에 NPC 추가
		std::unordered_set<unsigned int> npc_vl;
		for (unsigned int i = 0; i < NUM_NPC; ++i) {
			int npc_id = i + NPC_ID_START;
			if (isNearObject(npc_id, new_id)) {
				WakeUpNPC(npc_id);
				npc_vl.insert(npc_id);
				SendAddObjectPacket(new_id, npc_id);
			}
		}


		// 락 언락 횟수를 줄이기 위함
		clients[new_id]->viewlist_lock.lock();
		for (auto other_id : other_vl)
			clients[new_id]->viewlist.insert(other_id);
		for (auto npc_id : npc_vl)
			clients[new_id]->viewlist.insert(npc_id);
		clients[new_id]->viewlist_lock.unlock();

		clients[new_id]->connected = true;
		serverPrint("Client [%d] Connected\n", new_id);

		
	}
		break;
	case DS_Type::LoginFail:
	{
		DBUserKeyPacket *myPacket = reinterpret_cast<DBUserKeyPacket *>(buf);

		// 클라이언트가 캐릭터를 선택하게 해줘야 하는 통신 코드 생략
		int select_player_type = PlayerType::SWORDMAN;


		SD_Packet_NewPlayerData db_send_packet;
		db_send_packet.size = sizeof(db_send_packet);
		db_send_packet.type = SD_Type::NewPlayer;
		wcsncpy_s(db_send_packet.user_id, clients[myPacket->id]->user_id, NCHAR_LENGTH);
		db_send_packet.id = myPacket->id;


		switch (select_player_type) {
		case PlayerType::SWORDMAN:
			db_send_packet.playerType = PlayerType::SWORDMAN; // 임의 지정, 소드맨만 구현
			db_send_packet.hp = 75;
			db_send_packet.mp = 20;
			db_send_packet.stamina = 40;
			db_send_packet.max_exp = 100;
			db_send_packet.atk = 7;
			db_send_packet.def = 5;
			db_send_packet.mag = 3;
			db_send_packet.mr = 5;
			break;
		case PlayerType::WITCH:
			db_send_packet.playerType = PlayerType::WITCH; // 임의 지정, 소드맨만 구현
			db_send_packet.hp = 45;
			db_send_packet.mp = 65;
			db_send_packet.stamina = 25;
			db_send_packet.max_exp = 100;
			db_send_packet.atk = 2;
			db_send_packet.def = 3;
			db_send_packet.mag = 8;
			db_send_packet.mr = 7;
			break;
		case PlayerType::ARCHER:
			db_send_packet.playerType = PlayerType::ARCHER; // 임의 지정, 소드맨만 구현
			db_send_packet.hp = 65;
			db_send_packet.mp = 35;
			db_send_packet.stamina = 35;
			db_send_packet.max_exp = 100;
			db_send_packet.atk = 5;
			db_send_packet.def = 3;
			db_send_packet.mag = 3;
			db_send_packet.mr = 5;
			break;
		default:
			db_send_packet.playerType = PlayerType::SWORDMAN; // 임의 지정, 소드맨만 구현
			db_send_packet.hp = 100;
			db_send_packet.mp = 10;
			db_send_packet.stamina = 25;
			db_send_packet.max_exp = 100;
			db_send_packet.atk = 5;
			db_send_packet.def = 5;
			db_send_packet.mag = 5;
			db_send_packet.mr = 5;
			break;
		}
		

		SendPacket(QueryServer, reinterpret_cast<char *>(&db_send_packet));
	}
		break;
	case DS_Type::NewPlayerFail:
	{
		DBUserKeyPacket* myPacket = reinterpret_cast<DBUserKeyPacket *>(buf);
		sc_packet_login_fail loginFailPacket;
		loginFailPacket.size = sizeof(loginFailPacket);
		loginFailPacket.type = SC_LOGIN_FAIL;
		SendPacket(myPacket->id, reinterpret_cast<char *>(&loginFailPacket));
	}
		break;
	case 111: // Dummy
	{
		unsigned int new_id = GetNewID();

		SendLoginOKPacket(new_id);
		SendAddObjectPacket(new_id, new_id); // 나 자신에게 나의 등장을 먼저 미리 알려준다.

		// 다른 플레이어에게 나의 등장을 알려줌
		for (int i = 0; i < MAX_USER; ++i) {
			if (!clients[i]->connected) continue;
			if (i == new_id) continue; // 나 자신에게 나를 알려줄 필요는 없다.
			if (isNearObject(i, new_id)) {
				clients[i]->viewlist_lock.lock();
				clients[i]->viewlist.insert(new_id);
				clients[i]->viewlist_lock.unlock();
				SendAddObjectPacket(i, new_id);
			}
		}

		// 다른 플레이어 추가
		std::unordered_set<unsigned int> other_vl;
		for (unsigned int i = 0; i < MAX_USER; ++i) {
			if (!clients[i]->connected) continue;
			if (i == new_id) continue;
			if (isNearObject(i, new_id)) {
				other_vl.insert(i);
				SendAddObjectPacket(new_id, i);
			}
		}

		//플레이어의 시야에 NPC 추가
		std::unordered_set<unsigned int> npc_vl;
		for (unsigned int i = 0; i < NUM_NPC; ++i) {
			int npc_id = i + NPC_ID_START;
			if (isNearObject(npc_id, new_id)) {
				WakeUpNPC(npc_id);
				npc_vl.insert(npc_id);
				SendAddObjectPacket(new_id, npc_id);
			}
		}


		// 락 언락 횟수를 줄이기 위함
		clients[new_id]->viewlist_lock.lock();
		for (auto other_id : other_vl)
			clients[new_id]->viewlist.insert(other_id);
		for (auto npc_id : npc_vl)
			clients[new_id]->viewlist.insert(npc_id);
		clients[new_id]->viewlist_lock.unlock();

		clients[new_id]->connected = true;
		serverPrint("Client [%d] Connected\n", new_id);


	}
		break;
	default:
		serverPrint("Unknown Packet Type Error\n");
		while (true);
	}
}

void ServerManager::DisConnect(unsigned int id)
{
	if (!isPlayer(id))
		return;
	for (int i = 0; i < MAX_USER; ++i) {
		if (!clients[i]->connected) continue;
		clients[i]->viewlist_lock.lock();
		if (clients[i]->viewlist.count(id)) { // 뷰리스트에 내가 있으면
			clients[i]->viewlist_lock.unlock();
			SendRemoveObjectPacket(i, id);
		}
		else
			clients[i]->viewlist_lock.unlock();
	}
	if (id == QueryServer)
		QueryServer = -1;
	if (clients[id]->connected) {
		closesocket(clients[id]->socket);
		SendSetDBData(id);
		clients[id]->viewlist_lock.lock();
		clients[id]->viewlist.clear();
		clients[id]->viewlist_lock.unlock();
		clients[id]->connected = false;	
		serverPrint("client [%d] : %LS is disconnected.\n", id, clients[id]->user_id);
		wcsncpy_s(clients[id]->user_id, L"", NCHAR_LENGTH);
	}
}

const unsigned int ServerManager::GetNewID()
{
	while (true)
		for (int i = 0; i < MAX_USER; ++i)	// 플레이어에 새로운 ID를 할당
			if (!clients[i]->connected && i != QueryServer) {
				clients[i]->connected = true;
				return i;
			}
}

bool ServerManager::isPlayer(unsigned int id)
{
	if ((id >= 0) && (id < MAX_USER)) return true;
	return false;
}

bool ServerManager::isNearObject(unsigned int a, unsigned  int b)
{
	if (VIEW_RADIUS < abs(objects[a]->x - objects[b]->x))
		return false;
	if (VIEW_RADIUS < abs(objects[a]->y - objects[b]->y))
		return false;
	return true;
}

bool ServerManager::isSleepingNPC(unsigned int npc_id)
{
	if (npc_id >= NPC_ID_START && npc_id < NPC_ID_START + NUM_NPC) {
		NPC* npc = reinterpret_cast<NPC*>(objects[npc_id]);
		return npc->is_sleeping;
	}
	return false;
}

void ServerManager::WakeUpNPC(unsigned int id)
{
	if (isSleepingNPC(id)) {
		NPC* targetNPC = reinterpret_cast<NPC*>(objects[id]);
		targetNPC->is_sleeping = false;
		AddTimerEvent(id);
	}
}

void ServerManager::RandomMove(unsigned int object_id)
{
	int x = objects[object_id]->x;
	int y = objects[object_id]->y;

	std::unordered_set <int> old_vl;	// 현재 오브젝트를 보고 있는 플레이어 리스트
	for (unsigned int i = 0; i < MAX_USER; ++i) {
		if (!clients[i]->connected) continue;
		if (!isNearObject(object_id, i)) continue;
		old_vl.insert(i);
	}

	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_int_distribution<> randomDir(0, 3);
	int dir = randomDir(dre);
	switch (dir) {
	case 0: // Up
		if (y < WORLD_HEIGHT - 1) ++y;
		break;
	case 1: // Down
		if (y > 0) --y;
		break;
	case 2: // Left
		if (x > 0) --x;
		break;
	case 3: // Right
		if (x < WORLD_WIDTH - 1) ++x;
		break;
	}
	objects[object_id]->x = x;
	objects[object_id]->y = y;


	std::unordered_set <int> new_vl; // 이동한 뒤의 오브젝트를 보고 있는 플레이어 리스트
	for (unsigned int i = 0; i < MAX_USER; ++i) {
		if (!clients[i]->connected) continue;
		if (!isNearObject(object_id, i)) continue;
		new_vl.insert(i);
	}

	for (auto user : old_vl) {
		if (new_vl.count(user)) {
			clients[user]->viewlist_lock.lock();
			if (clients[user]->viewlist.count(object_id)) {
				clients[user]->viewlist_lock.unlock();
				SendPositionPacket(user, object_id);
			}
			else {
				clients[user]->viewlist.insert(object_id);
				clients[user]->viewlist_lock.unlock();
				SendAddObjectPacket(user, object_id);
			}
		}
		else {
			clients[user]->viewlist_lock.lock();
			if (clients[user]->viewlist.count(object_id)) {
				clients[user]->viewlist.erase(object_id);
				clients[user]->viewlist_lock.unlock();
				SendRemoveObjectPacket(user, object_id);
			}
			else clients[user]->viewlist_lock.unlock();
		}
	}

	for (auto user : new_vl) {
		if (!old_vl.count(object_id)) {
			clients[user]->viewlist_lock.lock();
			if (!clients[user]->viewlist.count(object_id)) {
				clients[user]->viewlist.insert(object_id);
				clients[user]->viewlist_lock.unlock();
				SendAddObjectPacket(user, object_id);
			}
			else {
				clients[user]->viewlist_lock.unlock();
				SendPositionPacket(user, object_id);
			}
		}
	}
}

void ServerManager::CoordMove(unsigned int object_id, int x, int y)
{
	std::unordered_set <int> old_vl;	// 현재 오브젝트를 보고 있는 플레이어 리스트
	for (unsigned int i = 0; i < MAX_USER; ++i) {
		if (!clients[i]->connected) continue;
		if (!isNearObject(object_id, i)) continue;
		old_vl.insert(i);
	}

	// BorderLine Check
	if (y >= WORLD_HEIGHT - 1) y = WORLD_HEIGHT - 1;
	else if (y <= 0) y = 0;
	if (x <= 0) x = 0;
	else if (x >= WORLD_WIDTH - 1) x = WORLD_WIDTH - 1;

	// 갱신
	objects[object_id]->x = x;
	objects[object_id]->y = y;

	std::unordered_set <int> new_vl; // 이동한 뒤의 오브젝트를 보고 있는 플레이어 리스트
	for (unsigned int i = 0; i < MAX_USER; ++i) {
		if (!clients[i]->connected) continue;
		if (!isNearObject(object_id, i)) continue;
		new_vl.insert(i);
	}

	for (auto user : old_vl) {
		if (new_vl.count(user)) {
			clients[user]->viewlist_lock.lock();
			if (clients[user]->viewlist.count(object_id)) {
				clients[user]->viewlist_lock.unlock();
				SendPositionPacket(user, object_id);
			}
			else {
				clients[user]->viewlist.insert(object_id);
				clients[user]->viewlist_lock.unlock();
				SendAddObjectPacket(user, object_id);
			}
		}
		else {
			clients[user]->viewlist_lock.lock();
			if (clients[user]->viewlist.count(object_id)) {
				clients[user]->viewlist.erase(object_id);
				clients[user]->viewlist_lock.unlock();
				SendRemoveObjectPacket(user, object_id);
			}
			else clients[user]->viewlist_lock.unlock();
		}
	}

	for (auto user : new_vl) {
		if (!old_vl.count(object_id)) {
			clients[user]->viewlist_lock.lock();
			if (!clients[user]->viewlist.count(object_id)) {
				clients[user]->viewlist.insert(object_id);
				clients[user]->viewlist_lock.unlock();
				SendAddObjectPacket(user, object_id);
			}
			else {
				clients[user]->viewlist_lock.unlock();
				SendPositionPacket(user, object_id);
			}
		}
	}
}

void ServerManager::ChaseMove(unsigned int id, unsigned int target)
{
	int x = objects[id]->x;
	int y = objects[id]->y;

	std::unordered_set <int> old_vl;	// 현재 오브젝트를 보고 있는 플레이어 리스트
	for (unsigned int i = 0; i < MAX_USER; ++i) {
		if (!clients[i]->connected) continue;
		if (!isNearObject(id, i)) continue;
		old_vl.insert(i);
	}

	if (x < objects[target]->x)
		++x;
	if (x > objects[target]->x)
		--x;
	if (y < objects[target]->y)
		++y;
	if (y > objects[target]->y)
		--y;
	
	objects[id]->x = x;
	objects[id]->y = y;

	if (objects[id]->x == objects[target]->x && objects[id]->y == objects[target]->y) {
		NPC* npc = reinterpret_cast<NPC*> (objects[id]);
		int dmg = max(1, npc->data.ATK - clients[target]->status.DEF);
		objects[target]->HP = max(0, objects[target]->HP - dmg);
		SendStatChangePacket(target);
		wchar_t system_message[MAX_STR_LEN];
		wsprintf(system_message, TEXT("당신은 적과의 충돌로 %d의 피해를 입었다! 어서 자리를 벗어나야 한다!"), dmg);
		SendChatPacket(target, target, system_message);
		if (objects[target]->HP <= 0) {
			clients[target]->EXP /= 2;
			clients[target]->HP = clients[target]->MAX_HP;
			clients[target]->x = 10;
			clients[target]->y = 10;
			SendChatPacket(target, target, L"사망하여서 초기 위치로 귀환하였다...");
			SendChatPacket(target, target, L"보유했던 경험치의 반을 잃었다...");
			SendStatChangePacket(target);
		}
		SendSetDBData(target);
	}

	std::unordered_set <int> new_vl; // 이동한 뒤의 오브젝트를 보고 있는 플레이어 리스트
	for (unsigned int i = 0; i < MAX_USER; ++i) {
		if (!clients[i]->connected) continue;
		if (!isNearObject(id, i)) continue;
		new_vl.insert(i);
	}

	for (auto user : old_vl) {
		if (new_vl.count(user)) {
			clients[user]->viewlist_lock.lock();
			if (clients[user]->viewlist.count(id)) {
				clients[user]->viewlist_lock.unlock();
				SendPositionPacket(user, id);
			}
			else {
				clients[user]->viewlist.insert(id);
				clients[user]->viewlist_lock.unlock();
				SendAddObjectPacket(user, id);
			}
		}
		else {
			clients[user]->viewlist_lock.lock();
			if (clients[user]->viewlist.count(id)) {
				clients[user]->viewlist.erase(id);
				clients[user]->viewlist_lock.unlock();
				SendRemoveObjectPacket(user, id);
			}
			else clients[user]->viewlist_lock.unlock();
		}
	}

	for (auto user : new_vl) {
		if (!old_vl.count(id)) {
			clients[user]->viewlist_lock.lock();
			if (!clients[user]->viewlist.count(id)) {
				clients[user]->viewlist.insert(id);
				clients[user]->viewlist_lock.unlock();
				SendAddObjectPacket(user, id);
			}
			else {
				clients[user]->viewlist_lock.unlock();
				SendPositionPacket(user, id);
			}
		}
	}
}

void ServerManager::AddTimerEvent(unsigned int id, TimerEvent::Command cmd, double second)
{
	std::chrono::duration<double> currentTime = std::chrono::high_resolution_clock::now().time_since_epoch(); // 현재 시간을 double 형태로 가져온다.
	timerQueue_Lock.lock();
	timerQueue.push(TimerEvent(id, cmd, currentTime.count() + second));
	timerQueue_Lock.unlock();
}

void ServerManager::AddTimerEvent(TimerEvent & timerEvent)
{
	timerQueue_Lock.lock();
	timerQueue.push(timerEvent);
	timerQueue_Lock.unlock();
}

void ServerManager::ProcessEvent(TimerEvent & timerEvent)
{
	switch (timerEvent.command) {
	case TimerEvent::Command::Random_Move:
	{
		RandomMove(timerEvent.objectID);
		bool have_to_sleep = true;
		NPC* npc = reinterpret_cast<NPC*> (objects[timerEvent.objectID]);
		for (int i = 0; i < MAX_USER; ++i) {
			if (!clients[i]->connected) continue;
			if (!isNearObject(i, timerEvent.objectID)) continue;
			if (npc->Type != NPC::State::Chase)
				AddTimerEvent(timerEvent.objectID);
			have_to_sleep = false;
			break;
		}
		
		npc->is_sleeping = have_to_sleep;
	}
		break;
	case TimerEvent::Command::Player_Moved:
		/*if (timerEvent.objectID >= SMART_NPC_START) {
		SMART_NPC* npc = reinterpret_cast<SMART_NPC*> (objects[timerEvent.objectID]);
		lua_State *L = npc->L;
		npc->script_lock.lock();
		lua_getglobal(L, "isMove");
		int isMove = (int)lua_tonumber(L, -1);
		lua_pop(L, 1);
		if (isMove == 0) {
			lua_getglobal(L, "event_player_move");
			lua_pushnumber(L, timerEvent.targetID);
			int err = lua_pcall(L, 1, 0, 0);
			if (err) error_display(L);
		}
		npc->script_lock.unlock();
	}*/
		if (Encounter(timerEvent.objectID, timerEvent.targetID)) {	
			NPC* npc = reinterpret_cast<NPC*> (objects[timerEvent.objectID]);
			if (npc->Type == NPC::State::War) {
				npc->BeforeType = NPC::State::War;
				std::chrono::duration<double> currentTime = std::chrono::high_resolution_clock::now().time_since_epoch();
				TimerEvent ev(timerEvent.objectID, TimerEvent::Command::Chase, currentTime.count());
				ev.targetID = timerEvent.targetID;
				AddTimerEvent(ev);
				npc->Type = NPC::State::Chase;
				wchar_t system_message[MAX_STR_LEN];
				wsprintf(system_message, TEXT("[%d]상대가 당신을 인식하고 쫓아옵니다!"), timerEvent.objectID);
				SendChatPacket(timerEvent.targetID, timerEvent.targetID, system_message);
			}
		}
		break;
	case TimerEvent::Command::Chase:
	{
		NPC* npc = reinterpret_cast<NPC*> (objects[timerEvent.objectID]);
		if (npc->Type == NPC::State::Chase) {
			ChaseMove(timerEvent.objectID, timerEvent.targetID);
			if (isNearObject(timerEvent.objectID, timerEvent.targetID)) {
				std::chrono::duration<double> currentTime = std::chrono::high_resolution_clock::now().time_since_epoch();
				TimerEvent ev(timerEvent.objectID, TimerEvent::Command::Chase, currentTime.count() + 0.75);
				ev.targetID = timerEvent.targetID;
				AddTimerEvent(ev);
			}
			else {
				// 자신이 쫓던 대상이 없어지면 초기 위치로 돌아감
				npc->x = npc->start_x;
				npc->y = npc->start_y;
				npc->Type = npc->BeforeType;
				wchar_t system_message[MAX_STR_LEN];
				wsprintf(system_message, TEXT("[%d]상대의 추격을 따돌렸다! "), timerEvent.targetID);
				SendChatPacket(timerEvent.targetID, timerEvent.targetID, system_message);
				AddTimerEvent(timerEvent.objectID);
			}
		}
	}
	
		break;
	case TimerEvent::Command::Flee:
		if (timerEvent.objectID >= SMART_NPC_START) {
		SMART_NPC* npc = reinterpret_cast<SMART_NPC*> (objects[timerEvent.objectID]);
		lua_State *L = npc->L;
		npc->script_lock.lock();
		lua_getglobal(L, "isMove");
		int isMove = (int)lua_tonumber(L, -1);
		lua_pop(L, 1);
		if (isMove == 1) {
			lua_getglobal(L, "flee");
			lua_pushnumber(L, timerEvent.targetID);
			int err = lua_pcall(L, 1, 0, 0);
			if (err) error_display(L);
		}
		npc->script_lock.unlock();
	}
		break;
	default:
		serverPrint("Unknown Event!!!\n");
		while (true);
	}
}

bool ServerManager::isObstacle(int x, int y)
{
	if (x == 3 && y == 3)
		return false;
	if (x == 15 && y == 19)
		return true;
	if (x == 31 && y == 73)
		return true;
	if (x == 93 && y == 65)
		return true;
	if (x == 213 && y == 73)
		return true;
	if (x == 38 && y == 165)
		return true;
	return false;
}

void ServerManager::LevelUp(int id)
{
	while (clients[id]->EXP >= clients[id]->status.MAX_EXP) {
		clients[id]->EXP -= clients[id]->status.MAX_EXP;
		wchar_t system_message[MAX_STR_LEN];
		wsprintf(system_message, TEXT("%d -> %d Level Up!"), clients[id]->LEVEL, clients[id]->LEVEL + 1);
		++clients[id]->LEVEL;
		SendChatPacket(id, id, system_message);
		switch (clients[id]->CharcterType) {
		case PlayerType::SWORDMAN:
			clients[id]->HP = clients[id]->MAX_HP += 3;
			clients[id]->status.MP = clients[id]->status.MAX_MP += 1;
			clients[id]->status.SP = clients[id]->status.MAX_SP += 2;
			clients[id]->status.MAX_EXP *= 2;
			clients[id]->status.ATK += 3;
			clients[id]->status.DEF += 2;
			clients[id]->status.MAG += 1;
			clients[id]->status.MR += 2;
			clients[id]->status.Ability_Point += 3;
			clients[id]->status.Skill_Point += 1;
			break;
		case PlayerType::WITCH:
			clients[id]->HP = clients[id]->MAX_HP += 1;
			clients[id]->status.MP = clients[id]->status.MAX_MP += 3;
			clients[id]->status.SP = clients[id]->status.MAX_SP += 2;
			clients[id]->status.MAX_EXP *= 2;
			clients[id]->status.ATK += 1;
			clients[id]->status.DEF += 1;
			clients[id]->status.MAG += 3;
			clients[id]->status.MR += 3;
			clients[id]->status.Ability_Point += 3;
			clients[id]->status.Skill_Point += 1;
			break;
		case PlayerType::ARCHER:
			clients[id]->HP = clients[id]->MAX_HP += 2;
			clients[id]->status.MP = clients[id]->status.MAX_MP += 2;
			clients[id]->status.SP = clients[id]->status.MAX_SP += 2;
			clients[id]->status.MAX_EXP *= 2;
			clients[id]->status.ATK += 3;
			clients[id]->status.DEF += 1;
			clients[id]->status.MAG += 2;
			clients[id]->status.MR += 2;
			clients[id]->status.Ability_Point += 3;
			clients[id]->status.Skill_Point += 1;
			break;
		default:
			clients[id]->HP = clients[id]->MAX_HP += 2;
			clients[id]->status.MP = clients[id]->status.MAX_MP += 2;
			clients[id]->status.SP = clients[id]->status.MAX_SP += 2;
			clients[id]->status.MAX_EXP *= 2;
			clients[id]->status.ATK += 2;
			clients[id]->status.DEF += 2;
			clients[id]->status.MAG += 2;
			clients[id]->status.MR += 2;
			clients[id]->status.Ability_Point += 3;
			clients[id]->status.Skill_Point += 1;
			break;
		}	
	}
	SendStatChangePacket(id);
	SendSetDBData(id);
}

bool ServerManager::Encounter(int npc_id, int client_id, int range)
{
	/*
	if (npc_id < NPC_ID_START || npc_id >= NUM_NPC + NPC_ID_START)
		return false;
	if (client_id >= MAX_USER)
		return false;
	NPC* npc = reinterpret_cast<NPC*> (objects[npc_id]);*/
	if (range < abs(objects[npc_id]->x - objects[client_id]->x))
		return false;
	if (range < abs(objects[npc_id]->y - objects[client_id]->y))
		return false;
	return true;
}




// LUA API


inline int API_Send_Message(lua_State * L)
{
	int to = (int)lua_tonumber(L, -3);
	int from = (int)lua_tonumber(L, -2);
	char *message = (char *)lua_tostring(L, -1);
	wchar_t wmess[MAX_STR_LEN];
	size_t wlen;

	mbstowcs_s(&wlen, wmess, strlen(message) + 1, message, _TRUNCATE);

	lua_pop(L, 4);
	extern std::unique_ptr<ServerManager> server;
	server->SendChatPacket(to, from, wmess);
	return 0;
}

inline int API_get_x(lua_State * L)
{
	int id = (int)lua_tonumber(L, -1);
	extern std::unique_ptr<ServerManager> server;
	int x = server->GetObjectByID(id)->x;
	lua_pop(L, 2);
	lua_pushnumber(L, x);
	return 1;
}

inline int API_get_y(lua_State * L)
{
	int id = (int)lua_tonumber(L, -1);
	extern std::unique_ptr<ServerManager> server;
	int y = server->GetObjectByID(id)->y;
	lua_pop(L, 2);
	lua_pushnumber(L, y);
	return 1;
}

inline int API_AddFleeEvent(lua_State * L)
{
	lua_getglobal(L, "myid");	// rows 전역 변수 값을 꺼내겠다. 상태 기계에서 rows라는 변수 값을 꺼내서 맨 위에 올려놓는다. (push)
	int player = (int)lua_tonumber(L, -2);
	int id = (int)lua_tonumber(L, -1);
	lua_pop(L, 3);

	lua_getglobal(L, "distance");
	int distacne = (int)lua_tonumber(L, -1);
	lua_pop(L, 1);
	// 이동한 거리가 0이므로 현재 최초 이동 방향을 초기화
	if (distacne == 0) {
		std::random_device rd;
		std::default_random_engine dre(rd());
		std::uniform_int_distribution<> randomDir(0, 3); // 0 : UP, 1 : DOWN, 2 : LEFT, 3 : RIGHT
		int dir = randomDir(dre);
		lua_getglobal(L, "set_direction");
		lua_pushnumber(L, dir);
		int err = lua_pcall(L, 1, 0, 0);
		if (err) error_display(L);
	}
	
	std::chrono::duration<double> currentTime = std::chrono::high_resolution_clock::now().time_since_epoch();
	static double prevTime = 0.0;
	
	TimerEvent ev(id, TimerEvent::Command::Flee, currentTime.count() + 1.0);
	ev.targetID = player;
	extern std::unique_ptr<ServerManager> server;

	if (abs(prevTime - ev.startTime) > 1.0)
		server->AddTimerEvent(ev);
	else {
		ev.startTime = prevTime + 1.0;
		server->AddTimerEvent(ev);
		// puts("교정!!!");
	}
	prevTime = currentTime.count();
	
	return 1;
}

inline int API_SetPosition(lua_State * L)
{
	lua_getglobal(L, "myid");
	int x = (int)lua_tonumber(L, -3);
	int y = (int)lua_tonumber(L, -2);
	int id = (int)lua_tonumber(L, -1);

	lua_pop(L, 4);
	extern std::unique_ptr<ServerManager> server;
	server->CoordMove(id, x, y);

	return 0;
}