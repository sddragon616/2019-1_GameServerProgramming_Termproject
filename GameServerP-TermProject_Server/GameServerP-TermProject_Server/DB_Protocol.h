#pragma once

constexpr unsigned short SERVERPORT = 3500;
constexpr unsigned short BUF_SIZE = 1024;
constexpr unsigned char NCHAR_LENGTH = 10;

namespace DB_Protocol {
	enum PlayerType {
		SWORDMAN = 0,
		WITCH = 1,
		ARCHER = 2,
		PLAYER_TYPE_COUNT
	};
}

using PlayerType = DB_Protocol::PlayerType;

namespace DS
{
	enum DBToServerSocketType {
		DBConnect = 100,
		LoginFail,
		LoginSuccess,
		NewPlayerFail
	};
}
using DS_Type = DS::DBToServerSocketType;

namespace SD
{
	enum ServerToDataBaseSocketType {
		Login = 100,
		NewPlayer,
		SearchUser,
		SetDBPosition,
		SetDBData
	};
}
using SD_Type = SD::ServerToDataBaseSocketType;




#pragma pack(push ,1)


struct BasePacket {
	char size;
	char type;
};

struct DBUserKeyPacket : public BasePacket
{
	int id;
	wchar_t user_id[NCHAR_LENGTH];
};


struct SD_Packet_NewPlayerData : public DBUserKeyPacket {
	char playerType;
	int hp;
	int mp;
	int stamina;
	int max_exp;
	int atk;
	int def;
	int mag;
	int mr;
};

struct DS_GetUserDataPacket : public DBUserKeyPacket {
	char PlayerType;
	unsigned short LEVEL;
	int	HP, MAX_HP, MP, MAX_MP, SP, MAX_SP;
	int EXP, MAX_EXP;
	int ATK, DEF, MAG, MR;
	short Ability_Point;
	short Skill_Point;
	int Position_x;
	int Position_y;
};

using SD_SetDBDataPacket = DS_GetUserDataPacket;

#pragma pack (pop)
