#include "stdafx.h"
#include "Object005_MyPlayer.h"
#include "ResourceManager.h"

MyPlayer::MyPlayer(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int hp, unsigned int mp, unsigned int stamina, unsigned int exp, unsigned int max_exp, const CImage & Image) : BasePlayer(x, y, w, h, hp, Image)
{
	MAX_MP = MP = hp;
	MAX_SP = SP = stamina;
	MAX_EXP = max_exp;
	EXP = (EXP > MAX_EXP) ? MAX_EXP : EXP;
}

MyPlayer::MyPlayer(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int hp, unsigned int max_hp, unsigned int mp, unsigned int max_mp, unsigned int stamina, unsigned int max_stamina, unsigned int exp, unsigned int max_exp, const CImage & Image) : BasePlayer(x, y, w, h, hp, max_hp, Image)
{
	MP = (mp > max_mp) ? max_mp : mp;
	MAX_MP = max_mp;
	SP = (stamina > max_stamina) ? max_stamina : stamina;
	MAX_SP = max_stamina;
	MAX_EXP = max_exp;
	EXP = (EXP > MAX_EXP) ? MAX_EXP : EXP;
}

MyPlayer::MyPlayer(uVec2i pos, uVec2i size, unsigned int hp, unsigned int mp, unsigned int stamina, unsigned int exp, unsigned int max_exp, const CImage & Image) : BasePlayer(pos, size, hp, Image)
{
	MAX_MP = MP = hp;
	MAX_SP = SP = stamina;
	MAX_EXP = max_exp;
	EXP = (EXP > MAX_EXP) ? MAX_EXP : EXP;
}

MyPlayer::MyPlayer(uVec2i pos, uVec2i size, unsigned int hp, unsigned int max_hp, unsigned int mp, unsigned int max_mp, unsigned int stamina, unsigned int max_stamina, unsigned int exp, unsigned int max_exp, const CImage & Image) : BasePlayer(pos, size, hp, max_hp, Image)
{
	MP = (mp > max_mp) ? max_mp : mp;
	MAX_MP = max_mp;
	SP = (stamina > max_stamina) ? max_stamina : stamina;
	MAX_SP = max_stamina;
	MAX_EXP = max_exp;
	EXP = (EXP > MAX_EXP) ? MAX_EXP : EXP;
}

MyPlayer::~MyPlayer()
{
}

void MyPlayer::SetAllStat()
{
}

bool MyPlayer::ProcessInput(unsigned char * pKeysBuffer, float fTimeElapsed)
{
	unsigned char directionbit = 0;
	if (pKeysBuffer[VK_UP] & 0xF0)
		directionbit |= KEYDIR_UP;
	if (pKeysBuffer[VK_DOWN] & 0xF0) 
		directionbit |= KEYDIR_DOWN;
	if (pKeysBuffer[VK_LEFT] & 0xF0) 
		directionbit |= KEYDIR_LEFT;
	if (pKeysBuffer[VK_RIGHT] & 0xF0) 
		directionbit |= KEYDIR_RIGHT;
	SetDirectionBit(directionbit);
	return false;
}

void MyPlayer::Render(HDC hdc)
{
	BasePlayer::Render(hdc);

	if (GetResource()) {
		wchar_t value[25];
		unsigned short Gauge_width = CLIENT_WIDTH / 5;
		unsigned short Gauge_height = CLIENT_HEIGHT / 25;

		wsprintf(value, TEXT("Level : %d"), Level);
		DrawFont(hdc, value, 50, 10, 10, 25, RGB(192, 168, 95));
		DrawFont(hdc, USER_ID, 200, 10, 10, 25, RGB(192, 168, 95));

		DrawFont(hdc, L"HP", 5, 35, 10, 25, RGB(255, 0, 0));
		GetResource()->GetGaugeBarImage().Draw(hdc, 50, 35, Gauge_width, Gauge_height, 9, 0, 1, 8);	// MAX HP
		if (static_cast<unsigned int>(GetHPratio() * static_cast<double>(Gauge_width)) > 0)
			GetResource()->GetGaugeBarImage().Draw(hdc, 50, 35, static_cast<unsigned int>(GetHPratio() * static_cast<double>(Gauge_width)), Gauge_height, 0, 0, 9, 8);	// HP
		
		wsprintf(value, TEXT("%d / %d"), GetHP(), GetMaxHP());
		DrawFont(hdc, value, 55 + Gauge_width, 35, 10, 25, RGB(255, 0, 0));

		DrawFont(hdc, L"MP", 5, 36 + Gauge_height, 10, 25, RGB(48, 52, 109));
		GetResource()->GetGaugeBarImage().Draw(hdc, 50, 36 + Gauge_height, Gauge_width, Gauge_height, 9, 8, 1, 8);	// MAX MP
		if (static_cast<unsigned int>(GetMPratio() * static_cast<double>(Gauge_width)) > 0)
			GetResource()->GetGaugeBarImage().Draw(hdc, 50, 36 + Gauge_height, static_cast<unsigned int>(GetMPratio() * static_cast<double>(Gauge_width)), Gauge_height, 0, 8, 9, 8);	// MP

		wsprintf(value, TEXT("%d / %d"), MP, MAX_MP);
		DrawFont(hdc, value, 55 + Gauge_width, 36 + Gauge_height, 10, 25, RGB(48, 52, 109));

		DrawFont(hdc, L"SP", 5, 37 + 2 * Gauge_height, 10, 25, RGB(255, 127, 39));
		GetResource()->GetGaugeBarImage().Draw(hdc, 50, 37 + 2 * Gauge_height, Gauge_width, Gauge_height, 9, 16, 1, 8);	// MAX SP
		if (static_cast<unsigned int>(GetSPratio() * static_cast<double>(Gauge_width)) > 0)
			GetResource()->GetGaugeBarImage().Draw(hdc, 50, 37 + 2 * Gauge_height, static_cast<unsigned int>(GetSPratio() * static_cast<double>(Gauge_width)), Gauge_height, 0, 16, 9, 8);	// SP
		
		wsprintf(value, TEXT("%d / %d"), SP, MAX_SP);
		DrawFont(hdc, value, 55 + Gauge_width, 37 + 2 * Gauge_height, 10, 25, RGB(255, 127, 39));

		DrawFont(hdc, L"EXP", 5, 38 + 3 * Gauge_height, 10, 25, RGB(0, 255, 0));
		GetResource()->GetGaugeBarImage().Draw(hdc, 50, 38 + 3 * Gauge_height, Gauge_width, Gauge_height, 9, 24, 1, 8);	// MAX EXP
		if (static_cast<unsigned int>(GetEXPratio() * static_cast<double>(Gauge_width)) > 0) // assert
			GetResource()->GetGaugeBarImage().Draw(hdc, 50, 38 + 3 * Gauge_height, static_cast<unsigned int>(GetEXPratio() * static_cast<double>(Gauge_width)), Gauge_height, 0, 24, 9, 8);	// EXP
		
		wsprintf(value, TEXT("%d / %d"), EXP, MAX_EXP);
		DrawFont(hdc, value, 55 + Gauge_width, 38 + 3 * Gauge_height, 10, 25, RGB(0, 255, 0));
	}
}
