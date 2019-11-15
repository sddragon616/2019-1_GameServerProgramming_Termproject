#include "stdafx.h"
#include "Object003_AttackObject.h"
#include "ResourceManager.h"

AttackObject::AttackObject(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int hp, const CImage & Image) : MovingObject(x, y, w, h, hp, Image)
{
}

AttackObject::AttackObject(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int hp, unsigned int max_hp, const CImage & Image) : MovingObject(x, y, w, h, hp, max_hp, Image)
{
}

AttackObject::AttackObject(uVec2i pos, uVec2i size, unsigned int hp, const CImage & Image) : MovingObject(pos, size, hp, Image)
{
}

AttackObject::AttackObject(uVec2i pos, uVec2i size, unsigned int hp, unsigned int max_hp, const CImage & Image) : MovingObject(pos, size, hp, max_hp, Image)
{
}

AttackObject::~AttackObject()
{
}

void AttackObject::Render(HDC hdc)
{
	if (GetResource()) {
		switch (objectType) {
		case FLYBUG:
			GetResource()->GetFlyImage().Draw(hdc, GetPosition().x - GetWindowLT().x, CLIENT_HEIGHT - (GetPosition().y - GetWindowLT().y) - 32, 32, 32, 32, 8 * 32, RENDER_UNIT / 2, RENDER_UNIT / 2);
			break;
		case CRAB:
			GetResource()->GetCrabImage().Draw(hdc, GetPosition().x - GetWindowLT().x, CLIENT_HEIGHT - (GetPosition().y - GetWindowLT().y) - 32, 32, 32, 32, 16 * 32, RENDER_UNIT / 2, RENDER_UNIT / 2);
			break;
		case SKULLGOLEM:
			GetResource()->GetSkullGolemImage().Draw(hdc, GetPosition().x - GetWindowLT().x, CLIENT_HEIGHT - (GetPosition().y - GetWindowLT().y) - 32, 32, 32, 32, 14 * 32, RENDER_UNIT / 2, RENDER_UNIT / 2);
			break;
		case SPEAR:
			GetResource()->GetSpearImage().Draw(hdc, GetPosition().x - GetWindowLT().x - 16, CLIENT_HEIGHT - (GetPosition().y - GetWindowLT().y) - 64, 64, 64, 64, 1232 - (5 * 64), RENDER_UNIT, RENDER_UNIT);
			break;
		case SLASHER:
			GetResource()->GetSlasherImage().Draw(hdc, GetPosition().x - GetWindowLT().x, CLIENT_HEIGHT - (GetPosition().y - GetWindowLT().y) - 32, 32, 32, 32, 9 * 32, RENDER_UNIT / 2, RENDER_UNIT / 2);
			break;
		}
	}
}
