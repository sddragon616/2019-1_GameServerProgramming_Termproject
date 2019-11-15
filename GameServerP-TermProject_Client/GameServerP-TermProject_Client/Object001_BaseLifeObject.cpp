#include "stdafx.h"
#include "Object001_BaseLifeObject.h"


BaseLifeObject::BaseLifeObject(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int hp, const CImage& Image) : BaseObject(x, y, w, h, Image)
{
	MAX_HP = HP = hp;
}

BaseLifeObject::BaseLifeObject(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int hp, unsigned int max_hp, const CImage& Image) : BaseObject(x, y, w, h, Image)
{
	HP = (hp > max_hp) ? max_hp : hp;
	MAX_HP = max_hp;
}

BaseLifeObject::BaseLifeObject(uVec2i pos, uVec2i size, unsigned int hp, const CImage & Image) : BaseObject(pos, size, Image)
{
	MAX_HP = HP = hp;
}

BaseLifeObject::BaseLifeObject(uVec2i pos, uVec2i size, unsigned int hp, unsigned int max_hp, const CImage & Image) : BaseObject(pos, size, Image)
{
	HP = (hp > max_hp) ? max_hp : hp;
	MAX_HP = max_hp;
}

BaseLifeObject::~BaseLifeObject()
{
}

void BaseLifeObject::Update(float fTimeElapsed)
{
}
