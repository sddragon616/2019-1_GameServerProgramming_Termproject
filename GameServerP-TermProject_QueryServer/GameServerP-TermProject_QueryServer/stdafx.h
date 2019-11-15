// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 특정 포함 파일이 들어 있는
// 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// 여기서 프로그램에 필요한 추가 헤더를 참조합니다.
// Debugging
#include <assert.h>
#include <stdio.h>
#include <iostream> // wstring

// Define
#pragma warning(disable:4996) // SDL 체크 무시
#ifdef UNICODE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console") 
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console") 
#endif
#include <locale.h>

// Network
#define _WINSOCK_DEPRECATED_NO_WARNINGS // inet_addt 함수와 WSAAsyncSelect 함수의 SDL 검사
#define WM_SOCKET	(WM_USER + 1)
#include <winsock2.h>
#pragma comment(lib, "ws2_32")

// DB
#include "../../GameServerP-TermProject_Server/GameServerP-TermProject_Server/DB_Protocol.h"
#pragma comment(lib, "odbc32.lib")
#include <sqlext.h>  
