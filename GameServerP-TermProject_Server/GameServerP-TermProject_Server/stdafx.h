// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#include <tchar.h>



// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>

#define _WINSOCK_DEPRECATED_NO_WARNINGS // inet_addt 함수와 WSAAsyncSelect 함수의 SDL 검사
#pragma warning(disable:4996) // SDL 체크 무시
#include <winsock2.h>
#pragma comment(lib, "ws2_32")
//#include "temporary_protocol.h"
#include "2019_화목_protocol.h"
#include "DB_Protocol.h"
// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#include <tchar.h>

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>

extern "C" {
#include "include/lua.h"
#include "include/lauxlib.h"
#include "include/lualib.h"
}

#pragma comment(lib, "lua53.lib")

inline int API_Send_Message(lua_State *L);
inline int API_get_x(lua_State *L);
inline int API_get_y(lua_State *L);
inline int API_SetPosition(lua_State * L);
inline int API_AddFleeEvent(lua_State * L);
inline void error_display(lua_State *L)
{
#ifdef PRINT
	std::cout << lua_tostring(L, -1);
	lua_pop(L, 1);
#endif
}

#include <thread>
#include <random>
#include <vector>
#include <unordered_set>
#include <mutex>
#include <queue> // 우선순위 큐를 위한 include. PPL 우선순위 큐로 교체하면 더 빨라지고 대신 호환성을 잃는다.
//#include <concurrent_priority_queue.h>	// PPL 우선순위 큐는 여기에 있다.
#include <chrono>

#define PRINT	// 화면 입출력은 시스템콜이라 굉장히 비싼 연산이다. 디버깅 작업이 끝나지 않았다면 속도를 위해 주석처리하자.
#ifdef PRINT
#include <stdio.h>
#include <iostream>
#endif

// PRINT가 define되지 않았다면 아무 일도 하지 않는 함수. 그 외에는 printf와 동일하다. printf 대신 사용
__inline int serverPrint(_In_z_ _Printf_format_string_ char const* const _Format, ...)
{
#ifdef PRINT
	va_list _ArgList;
	__crt_va_start(_ArgList, _Format);
	int _Result = _vfprintf_l(stdout, _Format, NULL, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
#else
	return -1;
#endif
}

struct TimerEvent {
public:
	enum class Command : unsigned char {
		None = 0,
		Random_Move,
		Player_Moved,
		Patrol,
		Chase,
		Flee
	};

	unsigned int objectID;
	unsigned int targetID;
	double startTime;
	Command command = Command::None;

	TimerEvent(unsigned int id, Command cmd, double t) : objectID(id), command(cmd), startTime(t) {}

	constexpr bool operator> (const TimerEvent& t) const {
		return (this->startTime > t.startTime);
	}
	constexpr bool operator< (const TimerEvent& t) const {
		return (this->startTime < t.startTime);
	}

	struct Priority {
		bool operator()(TimerEvent t, TimerEvent u) {
			return t.startTime > u.startTime;
		}
	};
};

struct OVER_EX
{
	enum class Type : unsigned char { NONE, RECV, EVENT };
	WSAOVERLAPPED overlapped;
	WSABUF dataBuffer;
	char buffer[BUFSIZE];
	Type type = Type::NONE;

	void ResetOverlapped() { memset(&overlapped, 0, sizeof(overlapped)); }
	void init() {
		dataBuffer.len = BUFSIZE;
		dataBuffer.buf = buffer;
		ResetOverlapped();
	}
};


struct StatusData
{
	int MP, MAX_MP, SP, MAX_SP;
	int ATK, DEF, MAG, MR;
};

struct PlayerStatusData : public StatusData
{
	int MAX_EXP;
	short Ability_Point;
	short Skill_Point;
};

struct Object
{
	int x, y;
	char dir;
	char CharcterType;
	short LEVEL;
	int HP, MAX_HP;
	int EXP;	// 플레이어라면 자신의 소유중 경험치, 아니라면 쓰러질시 플레이어에게 주는 경험치
};

struct NPC : public Object
{
	bool	is_sleeping;
	int start_x, start_y;
	enum class State : unsigned char {
		Peace,
		War,
		Chase
	}; 
	State BeforeType;
	State Type;
	StatusData data;
};

using Monster = NPC;


struct SMART_NPC : public NPC
{
	lua_State *L;
	std::mutex script_lock;
};

struct Player : public Object
{
	bool connected;	 // 이게 false면 서버에서 처리를 하지 않는다. 아무도 보지 않는 영역의 NPC는 false로 처리해서 CPU를 아끼자
	OVER_EX over;
	SOCKET socket;
	char packet_buf[BUFSIZE];	// OVER_EX 내에서 메시지 버퍼에서 재조립이 안 되었을 경우 대비
	int prev_size;
	std::unordered_set <unsigned int> viewlist;
	std::mutex viewlist_lock;

	wchar_t user_id[NCHAR_LENGTH];
	PlayerStatusData status;
};
