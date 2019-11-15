#include "stdafx.h"
#include "Object002_MovingObject.h"

MovingObject::MovingObject(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int hp, const CImage & Image) : BaseLifeObject(x, y, w, h, hp, Image)
{
}

MovingObject::MovingObject(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int hp, unsigned int max_hp, const CImage & Image) : BaseLifeObject(x, y, w, h, hp, max_hp, Image)
{
}

MovingObject::MovingObject(uVec2i pos, uVec2i size, unsigned int hp, const CImage & Image) : BaseLifeObject(pos, size, hp, Image)
{
}

MovingObject::MovingObject(uVec2i pos, uVec2i size, unsigned int hp, unsigned int max_hp, const CImage & Image) : BaseLifeObject(pos, size, hp, max_hp, Image)
{
}

MovingObject::~MovingObject()
{
}
