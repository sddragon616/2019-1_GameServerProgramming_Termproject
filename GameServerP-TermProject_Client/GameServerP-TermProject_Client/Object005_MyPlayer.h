#pragma once
#include "Object004_BasePlayer.h"
class MyPlayer : public BasePlayer
{
private:
	unsigned int MP = 5, MAX_MP = 10;
	unsigned int SP = 5, MAX_SP = 10;
	unsigned int EXP = 0, MAX_EXP = 100;

	unsigned int Level = 1;
	unsigned int ATK = 1, DEF = 0, MAG = 0, MR = 0;
	unsigned int AP = 0, Skill_Point = 0;

	wchar_t USER_ID[10] = L"";
protected:
	unsigned char dirrection = 0;		// 방향이 어딜 보고 있는지 명시적으로 저장 후 사용

	double GetMPratio() const
	{
		if (MAX_MP)
			return (static_cast<double>(MP) / static_cast<double>(MAX_MP));
		return 0.0;
	}
	double GetSPratio() const
	{
		if (MAX_SP)
			return (static_cast<double>(SP) / static_cast<double>(MAX_SP));
		return 0.0;
	}
	double GetEXPratio() const
	{
		if (MAX_EXP)
			return (static_cast<double>(EXP) / static_cast<double>(MAX_EXP));
		return 0.0;
	}

public:
	MyPlayer() {}
	MyPlayer(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int hp, unsigned int mp, unsigned int stamina, unsigned int exp, unsigned int max_exp, const CImage& Image);
	MyPlayer(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int hp, unsigned int max_hp, unsigned int mp, unsigned int max_mp, unsigned int stamina, unsigned int max_stamina, unsigned int exp, unsigned int max_exp, const CImage& Image);
	MyPlayer(uVec2i pos, uVec2i size, unsigned int hp, unsigned int mp, unsigned int stamina, unsigned int exp, unsigned int max_exp, const CImage& Image);
	MyPlayer(uVec2i pos, uVec2i size, unsigned int hp, unsigned int max_hp, unsigned int mp, unsigned int max_mp, unsigned int stamina, unsigned int max_stamina, unsigned int exp, unsigned int max_exp, const CImage& Image);
	virtual ~MyPlayer();

	void SetAllStat();

	// MP값 제어
	void SetMP(unsigned int mp) { MP = mp; }
	void SetMaxMP(unsigned int mp)
	{
		if (MP > mp)
			MP = mp;
		MAX_MP = mp;
	}
	const unsigned int GetMP() const { return MP; }
	const unsigned int GetMaxMP() const { return MAX_MP; }

	// 스태미나 제어
	void SetStamina(unsigned int sp) { SP = sp; }
	void SetMaxStamina(unsigned int sp)
	{
		if (SP > sp)
			SP = sp;
		MAX_SP = sp;
	}
	const unsigned int GetStamina() const { return SP; }
	const unsigned int GetMaxStamina() const { return MAX_SP; }

	// 경험치값 제어
	void SetEXP(unsigned int exp) { EXP = exp; }
	void SetMaxEXP(unsigned int exp)
	{
		if (EXP > exp)
			EXP = exp;
		MAX_EXP = exp;
	}
	const unsigned int GetEXP() const { return EXP; }
	const unsigned int GetMaxEXP() const { return MAX_EXP; }


	void SetLevel(unsigned int lv) { Level = lv; }
	const unsigned int GetLevel() const { return Level; }

	void SetATK(unsigned int atk) { ATK = atk; }
	void SetDEF(unsigned int def) { DEF = def; }
	void SetMAG(unsigned int mag) { MAG = mag; }
	void SetMR(unsigned int mr) { MR = mr; }
	const unsigned int GetATK() const { return ATK; }
	const unsigned int GetDEF() const { return DEF; }
	const unsigned int GetMAG() const { return MAG; }
	const unsigned int GetMR() const { return MR; }

	void SetAP(unsigned int ap) { AP = ap; }
	const unsigned int GetAP() const { return AP; }
	void SetSkillPoint(unsigned int sp) { Skill_Point = sp; }
	const unsigned int GetSkillPoint() const { return Skill_Point; }

	virtual bool ProcessInput(unsigned char * pKeysBuffer, float fTimeElapsed) override;
	virtual void Render(HDC hdc) override;


	void SetUserID(wchar_t * user_id) { if (user_id) wcscpy_s(USER_ID, user_id); }

	wchar_t* GetUserID() { return USER_ID; }
};

