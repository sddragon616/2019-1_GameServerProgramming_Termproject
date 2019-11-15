// GameServerProgramming_TermProject_Server_2013180006JaeHwan-Kim.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//
#include "stdafx.h"
#include "ServerManager_2013180006JaeHwanKim.h"
std::unique_ptr<ServerManager> server;
int main()
{
	server = std::make_unique<ServerManager>();
	server->Run();
	return 0;
}