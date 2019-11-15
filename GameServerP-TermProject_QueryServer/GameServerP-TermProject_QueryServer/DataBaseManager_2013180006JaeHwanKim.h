#pragma once

class DataBaseManager final
{
private:
	bool isDestroyed = false;

	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0;


	void HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode);

	bool SearchAllData(wchar_t * user_id, int id);
	bool SetDBPosition(wchar_t * id, int x, int y);
	bool NewPlayerData(wchar_t * id, int type, int hp, int mp, int stamina, int max_exp, int atk, int def, int mag, int mr);
	bool SetDBData(wchar_t * id, int type, int lv, int hp, int max_hp, int mp, int max_mp, int stamina, int max_stamina, int exp, int max_exp, int atk, int def, int mag, int mr, int ap, int sp, int x, int y);

public:
	DataBaseManager();
	virtual ~DataBaseManager();

	bool OnCreate();
	bool OnDestroy();
	void FrameAdvance();

	

private:
	// Network;
	char server_ip[17] = "127.0.0.1"; // 서버 IP
	SOCKET mySocket; // 소켓

	// 전역변수 대용으로 사용
	WSABUF  send_wsabuf;
	char	send_buffer[BUF_SIZE];
	WSABUF	recv_wsabuf;
	char	recv_buffer[BUF_SIZE];
	char	packet_buffer[BUF_SIZE];
	DWORD	in_packet_size = 0;
	int		saved_packet_size = 0;

public:
	bool NetworkInitialize();
	bool ServerConnect();
	SOCKET GetMySocket() { return mySocket; }

	void ReadPacket(SOCKET sock);
	void SendPacket(char * clientToServerPacket);
	void ProcessPacket(char * ptr);
};
