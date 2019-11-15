#pragma once


class ServerManager
{
public:
	ServerManager();
	~ServerManager();

	// 복사 생성자는 삭제되어야 한다.
	ServerManager(const ServerManager&) = delete;
	ServerManager& operator=(const ServerManager&) = delete;

	void Run(); // Main 함수에서는 이거 하나만 실행하자. 다른 public 함수는 필요 없다.

	Object* GetObjectByID(unsigned int id) const { return objects[id]; }

private:
	HANDLE						hIOCP;

	SOCKADDR_IN					serverAddr;
	SOCKET						listenSocket;

	std::vector<std::thread>	workerThreads;
	std::priority_queue<TimerEvent, std::vector<TimerEvent>, TimerEvent::Priority>	timerQueue;
	std::mutex					timerQueue_Lock;

	Object*						objects[NPC_ID_START + NUM_NPC + NUM_SMART_NPC] = { nullptr };	// client, 투사체, 큐브 장애물 등에 다 ID를 등록 후 클라이언트에 뿌려줘야 하므로 Objects라고 명명
	Player*						clients[MAX_USER] = { nullptr };
	int QueryServer = -1;

	bool NetworkInitialize();

	static DWORD WINAPI RegisterAcceptThread(LPVOID self);
	void AcceptThread();
	static DWORD WINAPI RegisterWorkerThread(LPVOID self);
	void WorkerThread();
	static DWORD WINAPI RegisterTimerThread(LPVOID self);
	void TimerThread();

	void ObjectInitialize();
	void SendPacket(unsigned int id, char *packet);	// packet을 id 클라이언트에 보낸다
	void RecvPacket(unsigned int id);	// 해당 id의 클라이언트의 패킷을 받아온다
	void SendLoginOKPacket(unsigned int to);
	void SendAddObjectPacket(unsigned int to, unsigned int obj);
	void SendPositionPacket(unsigned int to, unsigned int obj);
	void SendRemoveObjectPacket(unsigned int to, unsigned int id);
	void SendStatChangePacket(unsigned int to);

	void SendSetDBData(unsigned int id);

public:
	void SendChatPacket(unsigned int to, unsigned int from, wchar_t *message);

private:
	void ProcessPacket(unsigned int id, char *buf);

	void DisConnect(unsigned int id);


	const unsigned int GetNewID();
	bool isPlayer(unsigned int id);
	bool isNearObject(unsigned int a, unsigned int b);
	bool isSleepingNPC(unsigned int npc_id);
	void WakeUpNPC(unsigned int id);

	void RandomMove(unsigned int id);
public:
	void CoordMove(unsigned int id, int x, int y);
private:
	void ChaseMove(unsigned int id, unsigned int target);

	void AddTimerEvent(unsigned int id, TimerEvent::Command cmd = TimerEvent::Command::Random_Move, double seconds = 1.0);
public:
	void AddTimerEvent(TimerEvent& timerEvent);
private:
	void ProcessEvent(TimerEvent& timerEvent);

	bool isObstacle(int x, int y);

	void LevelUp(int id);

	bool Encounter(int npc_id, int client_id, int range = 5);
};


