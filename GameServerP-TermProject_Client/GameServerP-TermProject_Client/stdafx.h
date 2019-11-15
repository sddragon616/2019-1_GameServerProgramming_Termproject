// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <atlimage.h>
#include <assert.h>
#define _USE_MATH_DEFINES
#include <math.h>

// Socket Programming
#define _WINSOCK_DEPRECATED_NO_WARNINGS // inet_addt 함수와 WSAAsyncSelect 함수의 SDL 검사
#pragma warning(disable:4996) // SDL 체크 무시
#include <winsock2.h>
#pragma comment(lib, "ws2_32")
//#include "../../GameServerP-TermProject_Server/GameServerP-TermProject_Server/temporary_protocol.h"
#include "../../GameServerP-TermProject_Server/GameServerP-TermProject_Server/2019_화목_protocol.h"
#define WM_SOCKET	(WM_USER + 1)

// C++ 런타임 헤더 파일입니다.
#include <chrono>
#include <string>
#include <memory>

constexpr unsigned char TITLE_MX_LENGTH = 128;	// 타이틀 문자열의 길이를 정의
#define TITLESTRING TEXT("GameServerProgramming Termproject - 2013180006 JaeHwan-Kim")	// 타이틀 이름이 들어갈 문자열이므로 내용을 원하는 대로 바꿔주자.

// 캡션 FPS 출력 여부
// 항상 캡션에 FPS를 출력	(0: 비활성 | 1: 활성)
#define USE_CAPTIONFPS_ALWAYS	1

#if USE_CAPTIONFPS_ALWAYS
#define SHOW_CAPTIONFPS
#elif _DEBUG	// debug에서는 항상 실행
#define SHOW_CAPTIONFPS
#endif

#if defined(SHOW_CAPTIONFPS)
#define MAX_UPDATE_FPS 1.0 / 3.0
#endif

// 최대 FPS 제한
constexpr double MAX_FPS = 60.0;
constexpr double MAX_FPS_TIME = 1.0 / MAX_FPS;

// 콘솔 창 띄움 여부, 디버그용이므로 디버그 모드일때만 define한다.
#ifdef _DEBUG
#define USE_CONSOLE_WINDOW

#endif
#define USE_CONSOLE_WINDOW
// 콘솔 창이 띄워져 있을 때만 콘솔 I/O에 필요한 헤더를 include 하도록 함
#ifdef USE_CONSOLE_WINDOW
#include <stdio.h>
#include <iostream>
#ifdef UNICODE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console") 
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console") 
#endif
#endif

#ifdef UNICODE
// 소켓 함수 오류 출력 후 종료
inline void err_quit(LPCWSTR *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, *msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}
inline void DrawFont(HDC hdc, const TCHAR * text, int x, int y, int font_width = 15, int font_height = 15, COLORREF text_color = RGB(255, 255, 255), LPCTSTR font_type = TEXT("궁서체"), int font_weight = 0)
{
	HFONT myFont = CreateFont(font_height, font_width, 0, 0, font_weight, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, font_type);
	HFONT oldFont = (HFONT)SelectObject(hdc, myFont);
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, text_color);
	TextOut(hdc, x, y, text, static_cast<int>(_tcslen(text)));
	SelectObject(hdc, oldFont);
	DeleteObject(myFont);
}
#else
// 소켓 함수 오류 출력 후 종료
inline void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}
inline void DrawFont(HDC hdc, const char * text, int x, int y, int font_width = 25, int font_height = 25, COLORREF text_color = RGB(255, 255, 255), LPCTSTR font_type = TEXT("궁서체"), int font_weight = 0)
{
	HFONT myFont = CreateFont(font_height, font_width, 0, 0, font_weight, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, font_type);
	HFONT oldFont = (HFONT)SelectObject(hdc, myFont);
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, text_color);
	TextOut(hdc, x, y, text, strlen(text));
	SelectObject(hdc, oldFont);
	DeleteObject(myFont);
}
#endif
// 소켓 함수 오류 출력
inline void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

// USE_CONSOLE_WINDOW가 define되지 않았다면 아무 일도 하지 않는 함수. 그 외에는 printf와 동일하다. printf 대신 사용
__inline int consolePrint(_In_z_ _Printf_format_string_ char const* const _Format, ...)
{
#ifdef USE_CONSOLE_WINDOW
	va_list _ArgList;
	__crt_va_start(_ArgList, _Format);
	int _Result = _vfprintf_l(stdout, _Format, NULL, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
#else
	return -1;
#endif
}

// Client Define
constexpr unsigned short CLIENT_WIDTH = 1024;
constexpr unsigned short CLIENT_HEIGHT = 768;
constexpr unsigned char RENDER_UNIT = 64;

template <typename val>
struct Vector2D {
	union {
		val value[2];
		struct { val x, y; };
		struct { val width, height; };
		struct { val w, h; };
	};
#define v value[0]
	//Vector2D& operator= (const POINT p) { x = p.x, y = p.y; return *this; }
	Vector2D& operator= (const Vector2D<val>& p) { x = p.x, y = p.y; return *this; }
	Vector2D& operator= (const val& s) { x = y = s; return *this; }

	Vector2D& operator+ (const Vector2D<val>& other) { return Vector2D<val>{x + other.x, y + other.y}; }
	Vector2D& operator- (const Vector2D<val>& other) { return Vector2D<val>{x - other.x, y - other.y}; }
};

using uVec2c = Vector2D<unsigned char>;
using uVec2s = Vector2D<unsigned short>;
using uVec2i = Vector2D<unsigned int>;


template <typename val>
struct Vector3D {
	union {
		val value[3];
		struct { val x, y, z; };
		struct { val r, g, b; };
		struct { val width, height, depth; };
		struct { val w, h, d; };
	};
#define v value[0]
	Vector3D& operator= (const Vector3D<val>& p) { x = p.x, y = p.y, z = p.z; return *this; }
	Vector3D& operator= (const val& s) { x = y = z = s; return *this; }

	Vector3D& operator+ (const Vector3D<val>& other) { return Vector2D<val>{x + other.x, y + other.y, z + other.z}; }
	Vector3D& operator- (const Vector3D<val>& other) { return Vector2D<val>{x - other.x, y - other.y, z - other.z}; }
};

enum class ObjectState{
	idle = 0,
	walking = 1,
	melee_attack = 2
};


constexpr unsigned char KEYDIR_UP = 0x01;
constexpr unsigned char KEYDIR_DOWN = 0x02;
constexpr unsigned char KEYDIR_LEFT = 0x04;
constexpr unsigned char KEYDIR_RIGHT = 0x08;

