#pragma once

constexpr int MAX_USER = 10000;
constexpr int MAX_STR_LEN = 50;

// constexpr int NPC_ID_START = 1000; //
constexpr int NPC_ID_START = MAX_USER + 1;

constexpr int NUM_NPC = 5000; // 200000;
constexpr int SMART_NPC_START = NUM_NPC + 1;
constexpr int NUM_SMART_NPC = 100;

#define WORLD_WIDTH		300
#define WORLD_HEIGHT	300

#define VIEW_RADIUS		10

#define SERVER_PORT		3500

#define CS_LOGIN		1
#define CS_MOVE			2
#define CS_ATTACK		3
#define CS_CHAT			4
#define CS_LOGOUT		5

#define SC_LOGIN_OK			1
#define SC_LOGIN_FAIL		2
#define SC_POSITION			3
#define SC_CHAT				4
#define SC_STAT_CHANGE		5
#define SC_REMOVE_OBJECT	6
#define SC_ADD_OBJECT		7



// Extend Protocol

enum AnimationState {
	Idle = 0,
	Walk = 1,
	MeleeAttack = 2
};

constexpr unsigned short BUFSIZE = 1024;
constexpr unsigned char tile_unit = 32;


// value를 minimum, maximum 사이로 보정하는 함수
template <typename var>
var clamp(var minimum, var value, var maximum) { return  (minimum > ((value < maximum) ? value : maximum)) ? minimum : ((value < maximum) ? value : maximum); }

#define SC_PUT_PLAYER		SC_ADD_OBJECT
#define SC_REMOVE_PLAYER	SC_REMOVE_OBJECT
#define SC_POS				SC_POSITION

#define CS_TELEPORT 6

enum MOVING_DIRECTION {
	DIR_UP = 0,
	DIR_DOWN = 1,
	DIR_LEFT = 2,
	DIR_RIGHT = 3,
	DIR_LEFT_UP	= 4,
	DIR_RIGHT_UP = 5,
	DIR_LEFT_DOWN = 6,
	DIR_RIGHT_DOWN = 7,
	TELEPORT = 8,
};

enum OBJECT_TYPE {
	// PLAYER
	SWORDMAN = 0,
	WITCH = 1,
	ARCHER = 2,
	PLAYER_TYPE_COUNT,

	// MONSTER
	MONSTER_START = 100,
	FLYBUG = 101,
	CRAB = 102,
	SKULLGOLEM = 103,
	SPEAR = 104,
	SLASHER = 105,
	MONSTER_TYPE_COUNT
};

constexpr int NUM_MONSTER_TYPE = MONSTER_TYPE_COUNT - MONSTER_START;


// Packet Type
#pragma pack(push ,1)

struct sc_packet_login_ok {
	char size;
	char type;
	int id;
	short	x, y;
	int	HP, LEVEL, EXP;
	// extends
	char playerType;
	int MAX_HP, MP, MAX_MP, SP, MAX_SP;
	int ATK, DEF, MAG, MR;
	int MAX_EXP;
	short Ability_Point;
	short Skill_Point;
};

struct sc_packet_login_fail {
	char size;
	char type;
};

struct sc_packet_position {
	char size;
	char type;
	int id;
	short x, y;
};

struct sc_packet_chat {
	char size;
	char type;
	int	id;
	wchar_t	message[MAX_STR_LEN];
};

struct sc_packet_stat_change {
	char size;
	char type;
	int	id;
	int	HP, LEVEL, EXP;
	// extends
	int MAX_HP, MP, MAX_MP, SP, MAX_SP;
	int ATK, DEF, MAG, MR;
	int MAX_EXP;
	short Ability_Point;
	short Skill_Point;
};


struct sc_packet_remove_object {
	char size;
	char type;
	int id;
};

struct sc_packet_add_object {
	char size;
	char type;
	int id;
	int obj_class;		// 1: PLAYER,    2:ORC,  3:Dragon, …..
	short x, y;
	int	HP, LEVEL, EXP;

	// extends
	int MAX_HP;
};


struct cs_packet_login {
	char	size;
	char	type;
	wchar_t player_id[10];
};

struct cs_packet_move {
	char	size;
	char	type;
	char	direction;		// 0:Up, 1:Down, 2:Left, 3:Right
};

struct cs_packet_attack {
	char	size;
	char	type;
	char	direction;	// Attack Dir
};

struct cs_packet_chat {
	char	size;
	char	type;
	wchar_t	message[MAX_STR_LEN];
};

struct cs_packet_logout {
	char	size;
	char	type;
};



// Extecnd Packet Type
using sc_packet_pos = sc_packet_position;

#pragma pack (pop)


