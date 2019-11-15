#include "stdafx.h"
#include "DataBaseManager_2013180006JaeHwanKim.h"


void DataBaseManager::HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode)
{
	SQLSMALLINT iRec = 0;
	SQLINTEGER  iError;
	WCHAR       wszMessage[1000];
	WCHAR       wszState[SQL_SQLSTATE_SIZE + 1];
	if (RetCode == SQL_INVALID_HANDLE) {
		fwprintf(stderr, L"Invalid handle!\n");
		return;
	}
	while (SQLGetDiagRec(hType, hHandle, ++iRec, wszState, &iError, wszMessage,
		(SQLSMALLINT)(sizeof(wszMessage) / sizeof(WCHAR)), (SQLSMALLINT *)NULL) == SQL_SUCCESS)
		// Hide data truncated.. 
		if (wcsncmp(wszState, L"01004", 5))
			fwprintf(stderr, L"[%5.5s] %s (%d)\n", wszState, wszMessage, iError);
}

DataBaseManager::DataBaseManager()
{
}


DataBaseManager::~DataBaseManager()
{
	OnDestroy();
}

bool DataBaseManager::OnCreate()
{
	if (!NetworkInitialize()) {
		delete this;
		return false;
	}
	SQLRETURN retcode;

	setlocale(LC_ALL, "korean");

	// Allocate environment handle  
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

	// Allocate Environment Success
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		// Set the ODBC version environment attribute  
		retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);

		// Set the ODBC version environment attribute  Success
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			// Allocate connection handle  
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

			// Allocate connection Success
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				// Set login timeout to 5 seconds
				SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);

				// Connect to data source  
				retcode = SQLConnect(hdbc, (SQLWCHAR*)L"2019_1_GSTP_2013180006JaeHwanKim", SQL_NTS, (SQLWCHAR*)NULL/*ID*/, 0/*패스워드*/, NULL, 0);

				// connect success
				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
					// Allocate statement handle  
					retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

					// DB 접속 성공
					printf("DB Access OK!\n");
					return true;
				}
				else
					SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
			}
			else {
				SQLFreeHandle(SQL_HANDLE_ENV, henv);
				printf("DB Initialize Failed!\n");
				return false;
			}
		}
		else
			SQLFreeHandle(SQL_HANDLE_ENV, henv);
	}
	printf("DB Initialize Failed!\n");
	delete this;
	return false;
}

bool DataBaseManager::OnDestroy()
{
	isDestroyed = true;
	SQLCancel(hstmt);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	SQLDisconnect(hdbc);
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
	SQLFreeHandle(SQL_HANDLE_ENV, henv);
	printf("DB Destroyed\n");
	return true;
}

void DataBaseManager::FrameAdvance()
{
}

bool DataBaseManager::NetworkInitialize()
{
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		::puts("Error - Can't load 'winsock.dll' file");
		assert(!"Error : \"winsock.dll\" 파일을 열 수 없습니다!");
		return false;
		// ::PostQuitMessage(0);
	}

	// socket()
	// client_sock = socket(AF_INET, SOCK_STREAM, 0);
	mySocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);//WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (mySocket == INVALID_SOCKET) {
		puts("Error - Invalid socket");
		assert(!"Error : 잘못된 소켓을 생성하였습니다!");
		return false;
		// ::PostQuitMessage(0);
	}

	printf("Client Socket : %d\n", static_cast<int>(mySocket));

	send_wsabuf.buf = send_buffer;
	send_wsabuf.len = BUF_SIZE;
	recv_wsabuf.buf = recv_buffer;
	recv_wsabuf.len = BUF_SIZE;

	return ServerConnect();
}

bool DataBaseManager::ServerConnect()
{
	printf("윈도우 실행 전에 접속할 Server IP(IPv4)를 입력하세요.\nDB 서버에서 게임 서버가 실행된다면 LocalHost IP인 127.0.0.1을 입력하시면 됩니다.\nServer IP(IPv4) >> ");
	// strcpy(server_ip, "127.0.0.1");
	scanf_s("%s", server_ip, unsigned int(sizeof(server_ip)));

	// 서버정보 객체설정	
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	// serveraddr.sin_addr.s_addr = inet_addr(server_ip);
	serveraddr.sin_addr.S_un.S_addr = inet_addr(server_ip);
	serveraddr.sin_port = htons(SERVERPORT);

	// connect()
	// if (connect(client_sock, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == SOCKET_ERROR) {
	if (WSAConnect(mySocket, (struct sockaddr *)&serveraddr, sizeof(serveraddr), NULL, NULL, NULL, NULL) == SOCKET_ERROR) {
		printf("%s IP Error - Fail to connect\n", server_ip);
		closesocket(mySocket);
		WSACleanup();
		assert(!"입력된 IP에서 클라이언트에 맞는 서버가 실행되어 있지 않습니다.");
		return false;
	}
	else {
		// WSAAsyncSelect(mySocket, m_hWnd, WM_SOCKET, FD_CLOSE | FD_READ); // m_hWnd 초기화도 안하고, 커넥트도 안하고, 이걸 부르고 있었으니 당연히 안되지...
		BasePacket* packet = reinterpret_cast<DBUserKeyPacket *>(send_buffer);
		packet->size = sizeof(BasePacket);
		packet->type = DS_Type::DBConnect;
		SendPacket(reinterpret_cast<char *>(packet));

		puts("Server Connected");
		return true;
	}
}

void DataBaseManager::ReadPacket(SOCKET sock)
{
	DWORD iobyte, ioflag = 0;

	if (WSARecv(sock, &recv_wsabuf, 1, &iobyte, &ioflag, NULL, NULL))
		printf("Recv Error [%d]\n", WSAGetLastError());

	BYTE *ptr = reinterpret_cast<BYTE *>(recv_buffer);

	while (0 != iobyte) {
		if (0 == in_packet_size) in_packet_size = ptr[0];
		if (iobyte + saved_packet_size >= in_packet_size) {
			memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
			ProcessPacket(packet_buffer);
			ptr += in_packet_size - saved_packet_size;
			iobyte -= in_packet_size - saved_packet_size;
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		else {
			memcpy(packet_buffer + saved_packet_size, ptr, iobyte);
			saved_packet_size += iobyte;
			iobyte = 0;
		}
	}
}

void DataBaseManager::SendPacket(char * clientToServerPacket)
{
	send_wsabuf.buf = send_buffer;
	send_wsabuf.len = clientToServerPacket[0];	// size
	DWORD ioByte = 0;

	if (WSASend(mySocket, &send_wsabuf, 1, &ioByte, 0, NULL, NULL)) 
		printf("Error while sending packet [%d]\n", WSAGetLastError());
}

void DataBaseManager::ProcessPacket(char * ptr)
{
	switch (ptr[1]) 
	{
	case SD_Type::Login:
	{
		DBUserKeyPacket *myPacket = reinterpret_cast<DBUserKeyPacket *>(ptr);
		if (!SearchAllData(myPacket->user_id, myPacket->id)) {
			DBUserKeyPacket* loginFailPacket = reinterpret_cast<DBUserKeyPacket *>(send_buffer);
			wcsncpy_s(loginFailPacket->user_id, myPacket->user_id, NCHAR_LENGTH);		
			loginFailPacket->id = myPacket->id;
			loginFailPacket->size = sizeof(DBUserKeyPacket);
			loginFailPacket->type = DS_Type::LoginFail;
			SendPacket(reinterpret_cast<char *>(loginFailPacket));
		}
	}
		break;
	case SD_Type::NewPlayer: 
	{
		SD_Packet_NewPlayerData *myPacket = reinterpret_cast<SD_Packet_NewPlayerData *>(ptr);

		if (NewPlayerData(myPacket->user_id, myPacket->playerType, myPacket->hp, myPacket->mp, myPacket->stamina, myPacket->max_exp, myPacket->atk, myPacket->def, myPacket->mag, myPacket->mr))
			SearchAllData(myPacket->user_id, myPacket->id);
		else {
			DBUserKeyPacket* loginFailPacket = reinterpret_cast<DBUserKeyPacket *>(send_buffer);
			wcsncpy_s(loginFailPacket->user_id, myPacket->user_id, NCHAR_LENGTH);
			loginFailPacket->id = myPacket->id;
			loginFailPacket->size = sizeof(DBUserKeyPacket);
			loginFailPacket->type = DS_Type::NewPlayerFail;
			SendPacket(reinterpret_cast<char *>(loginFailPacket));
		}		
	}
		break;
	case SD_Type::SearchUser:
		break;
	case SD_Type::SetDBData:
	{
		SD_SetDBDataPacket * packet = reinterpret_cast<SD_SetDBDataPacket *>(ptr);

		SetDBData(packet->user_id, packet->PlayerType, packet->LEVEL,
			packet->HP, packet->MAX_HP, packet->MP, packet->MAX_MP, packet->SP, packet->MAX_SP, packet->EXP, packet->MAX_EXP, packet->ATK, packet->DEF, packet->MAG, packet->MR,
			packet->Ability_Point, packet->Skill_Point, packet->Position_x, packet->Position_y);
	}
		break;
	}

}

bool DataBaseManager::SearchAllData(wchar_t * user_id, int id)
{
	SQLRETURN retcode;
	SQLWCHAR sUserID[NCHAR_LENGTH], sType[NCHAR_LENGTH];
	SQLINTEGER sLV, sHP, sMAX_HP, sMP, sMAX_MP, sStamina, sMAX_Stamina, sEXP, sMAX_EXP, sAtk, sDef, sMag, sMR, sAP, sSkillPoint, sPosX, sPosY;
	SQLLEN cbUserID = 0, cbType = 0, cbLV = 0, cbHP = 0, cbMAX_HP = 0, cbMP = 0, cbMAX_MP = 0, cbStamina = 0, cbMAX_Stamina = 0, cbEXP = 0, cbMAX_EXP = 0, 
		cbAtk = 0, cbDef = 0, cbMag = 0, cbMR = 0, cbAP = 0, cbSkillPoint = 0, cbPosX = 0, cbPosY = 0;	// 콜백 길이

	std::wstring exec(L"EXEC Search_All_Data ");
	exec.append(user_id);

	retcode = SQLExecDirect(hstmt, (SQLWCHAR *)exec.data(), SQL_NTS);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLBindCol(hstmt, 1, SQL_C_CHAR, sUserID, NCHAR_LENGTH, &cbUserID);
		retcode = SQLBindCol(hstmt, 2, SQL_C_CHAR, sType, NCHAR_LENGTH, &cbType);
		retcode = SQLBindCol(hstmt, 3, SQL_C_LONG, &sLV, 100, &cbLV);
		retcode = SQLBindCol(hstmt, 4, SQL_C_LONG, &sHP, 100, &cbHP);
		retcode = SQLBindCol(hstmt, 5, SQL_C_LONG, &sMAX_HP, 100, &cbMAX_HP);
		retcode = SQLBindCol(hstmt, 6, SQL_C_LONG, &sMP, 100, &cbMP);
		retcode = SQLBindCol(hstmt, 7, SQL_C_LONG, &sMAX_MP, 100, &cbMAX_MP);
		retcode = SQLBindCol(hstmt, 8, SQL_C_LONG, &sStamina, 100, &cbStamina);
		retcode = SQLBindCol(hstmt, 9, SQL_C_LONG, &sMAX_Stamina, 100, &cbPosY);
		retcode = SQLBindCol(hstmt, 10, SQL_C_LONG, &sEXP, 100, &cbEXP);
		retcode = SQLBindCol(hstmt, 11, SQL_C_LONG, &sMAX_EXP, 100, &cbMAX_EXP);
		retcode = SQLBindCol(hstmt, 12, SQL_C_LONG, &sAtk, 100, &cbAtk);
		retcode = SQLBindCol(hstmt, 13, SQL_C_LONG, &sDef, 100, &cbDef);
		retcode = SQLBindCol(hstmt, 14, SQL_C_LONG, &sMag, 100, &cbMag);
		retcode = SQLBindCol(hstmt, 15, SQL_C_LONG, &sMR, 100, &cbMR);
		retcode = SQLBindCol(hstmt, 16, SQL_C_LONG, &sAP, 100, &cbAP);
		retcode = SQLBindCol(hstmt, 17, SQL_C_LONG, &sSkillPoint, 100, &cbSkillPoint);
		retcode = SQLBindCol(hstmt, 18, SQL_C_LONG, &sPosX, 100, &cbPosX);
		retcode = SQLBindCol(hstmt, 19, SQL_C_LONG, &sPosY, 100, &cbPosY);

		retcode = SQLFetch(hstmt);
		if (retcode == SQL_ERROR/*|| retcode == SQL_SUCCESS_WITH_INFO*/) {
			printf("Search All Data : Error\n");
			HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, retcode);
		}
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			puts("\n=========================");
			wprintf(L"NAME : %LS\nLv : %d\nCLASS : %LS\nHP : %d / %d\nMP : %d / %d\nStamina : %d / %d\nExperience : %d / %d\nATK : %d\nDEF : %d\nMAG : %d\nMR : %d\nAbility Point : %d\nSkill Point : %d\nposition (%d, %d)\n", sUserID, 
				sLV, sType, sHP, sMAX_HP, sMP, sMAX_MP, sStamina, sMAX_Stamina, sEXP, sMAX_EXP, sAtk, sDef, sMag, sMR, sAP, sSkillPoint, sPosX, sPosY);
			puts("=========================");

			DS_GetUserDataPacket* packet = reinterpret_cast<DS_GetUserDataPacket *>(send_buffer);;
			packet->size = sizeof(DS_GetUserDataPacket);
			packet->type = DS_Type::LoginSuccess;
			packet->id = id;

			if (!wcsncmp(sType, L"SwordMan", NCHAR_LENGTH))
				packet->PlayerType = PlayerType::SWORDMAN;
			else if (!wcsncmp(sType, L"Witch", NCHAR_LENGTH))
				packet->PlayerType = PlayerType::WITCH;
			else if (!wcsncmp(sType, L"Archer", NCHAR_LENGTH))
				packet->PlayerType = PlayerType::ARCHER;
			else 
				packet->PlayerType = 0;
			
			packet->LEVEL = static_cast<unsigned short>(sLV);
			packet->HP = sHP;
			packet->MAX_HP = sMAX_HP;
			packet->MP = sMP;
			packet->MAX_MP = sMAX_MP;
			packet->SP = sStamina;
			packet->MAX_SP = sMAX_Stamina;
			packet->EXP = sEXP;
			packet->MAX_EXP = sMAX_EXP;
			packet->ATK = sAtk;
			packet->DEF = sDef;
			packet->MAG = sMag;
			packet->MR = sMR;
			packet->Ability_Point = static_cast<short>(sAP);
			packet->Skill_Point = static_cast<short>(sSkillPoint);
			packet->Position_x = sPosX;
			packet->Position_y = sPosY;

			SendPacket(reinterpret_cast<char *>(packet));
			return true;
		}
		else
			printf("Search Failed\n");
	}
	else {
		printf("%LS\n", exec.data());
		HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, retcode);	
	}
	SQLCloseCursor(hstmt);
	return false;
}

bool DataBaseManager::SetDBPosition(wchar_t * id, int x, int y)
{
	SQLRETURN retcode;
	SQLLEN cbUserID = 0, cbx = 0, cby = 0;	// 콜백 길이

	std::wstring exec(L"EXEC Set_DB_Position ");
	exec.append(id);
	exec.append(L", ");
	wchar_t convert_x[4], convert_y[4];
	_itow_s(x, convert_x, 10);
	exec.append(convert_x);
	exec.append(L", ");
	_itow_s(y, convert_y, 10);
	exec.append(convert_y);

	retcode = SQLExecDirect(hstmt, (SQLWCHAR *)exec.data(), SQL_NTS);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		SQLCloseCursor(hstmt);
		return true;
	}
	else {
		printf("%LS\n", exec.data());
		HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, retcode);
	}
	SQLCloseCursor(hstmt);
	printf("Search Failed");
	return false;
}

bool DataBaseManager::NewPlayerData(wchar_t * id, int type, int hp, int mp, int stamina, int max_exp, int atk, int def, int mag, int mr)
{
	SQLRETURN retcode;

	std::wstring exec(L"EXEC NewPlayer ");
	exec.append(id);
	exec.append(L", ");
	wchar_t convert_hp[4], convert_mp[4], convert_stamina[4], convert_maxExp[4], convert_atk[4], convert_def[4], convert_mag[4], convert_mr[4];
	
	switch (type) {
	case PlayerType::SWORDMAN:
		exec.append(L"SwordMan");
		break;
	case PlayerType::WITCH:
		exec.append(L"Witch");
		break;
	case PlayerType::ARCHER:
		exec.append(L"Archer");
		break;
	}
	
	exec.append(L", ");
	_itow_s(hp, convert_hp, 10);
	exec.append(convert_hp);
	exec.append(L", ");
	_itow_s(mp, convert_mp, 10);
	exec.append(convert_mp);
	exec.append(L", ");
	_itow_s(stamina, convert_stamina, 10);
	exec.append(convert_stamina);
	exec.append(L", ");
	_itow_s(max_exp, convert_maxExp, 10);
	exec.append(convert_maxExp);
	exec.append(L", ");
	_itow_s(atk, convert_atk, 10);
	exec.append(convert_atk);
	exec.append(L", ");
	_itow_s(def, convert_def, 10);
	exec.append(convert_def);
	exec.append(L", ");
	_itow_s(mag, convert_mag, 10);
	exec.append(convert_mag);
	exec.append(L", ");
	_itow_s(mr, convert_mr, 10);
	exec.append(convert_mr);

	retcode = SQLExecDirect(hstmt, (SQLWCHAR *)exec.data(), SQL_NTS);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		SQLCloseCursor(hstmt);
		return true;		
	}
	printf("Insert Failed\n");
	HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, retcode);
	SQLCloseCursor(hstmt);
	return false;
}

bool DataBaseManager::SetDBData(wchar_t * id, int type, int lv, int hp, int max_hp, int mp, int max_mp, int stamina, int max_stamina, int exp, int max_exp, int atk, int def, int mag, int mr, int ap, int sp, int x, int y)
{
	SQLRETURN retcode;

	std::wstring exec(L"EXEC UpdateData ");

	wchar_t convert_lv[4], convert_hp[4], conver_maxhp[4], convert_mp[4], conver_maxmp[4], convert_stamina[4], conver_maxstamina[4], convert_exp[4], convert_maxExp[4], convert_atk[4], convert_def[4], convert_mag[4], convert_mr[4], convert_ap[4], convert_sp[4], convert_x[4], convert_y[4];
	
	exec.append(id);
	exec.append(L", ");

	switch (type) {
	case PlayerType::SWORDMAN:
		exec.append(L"SwordMan");
		break;
	case PlayerType::WITCH:
		exec.append(L"Witch");
		break;
	case PlayerType::ARCHER:
		exec.append(L"Archer");
		break;
	}

	exec.append(L", ");
	_itow_s(lv, convert_lv, 10);
	exec.append(convert_lv);
	exec.append(L", ");
	_itow_s(hp, convert_hp, 10);
	exec.append(convert_hp);
	exec.append(L", ");
	_itow_s(max_hp, conver_maxhp, 10);
	exec.append(conver_maxhp);
	exec.append(L", ");
	_itow_s(mp, convert_mp, 10);
	exec.append(convert_mp);
	exec.append(L", ");
	_itow_s(max_mp, conver_maxmp, 10);
	exec.append(conver_maxmp);
	exec.append(L", ");
	_itow_s(stamina, convert_stamina, 10);
	exec.append(convert_stamina);
	exec.append(L", ");
	_itow_s(max_stamina, conver_maxstamina, 10);
	exec.append(conver_maxstamina);
	exec.append(L", ");
	_itow_s(exp, convert_exp, 10);
	exec.append(convert_exp);
	exec.append(L", ");
	_itow_s(max_exp, convert_maxExp, 10);
	exec.append(convert_maxExp);
	exec.append(L", ");
	_itow_s(atk, convert_atk, 10);
	exec.append(convert_atk);
	exec.append(L", ");
	_itow_s(def, convert_def, 10);
	exec.append(convert_def);
	exec.append(L", ");
	_itow_s(mag, convert_mag, 10);
	exec.append(convert_mag);
	exec.append(L", ");
	_itow_s(mr, convert_mr, 10);
	exec.append(convert_mr);
	exec.append(L", ");
	_itow_s(ap, convert_ap, 10);
	exec.append(convert_ap);
	exec.append(L", ");
	_itow_s(sp, convert_sp, 10);
	exec.append(convert_sp);
	exec.append(L", ");
	_itow_s(x, convert_x, 10);
	exec.append(convert_x);
	exec.append(L", ");
	_itow_s(y, convert_y, 10);
	exec.append(convert_y);

	retcode = SQLExecDirect(hstmt, (SQLWCHAR *)exec.data(), SQL_NTS);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		SQLCloseCursor(hstmt);
		return true;
	}
	printf("%LS - Update Failed\n", exec.data());
	HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, retcode);
	SQLCloseCursor(hstmt);
	return false;
}
