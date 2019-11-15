#pragma once
#include "Object000_BaseObject.h"

class BaseLifeObject : public BaseObject
{
private :
	unsigned int HP = 1;
	unsigned int MAX_HP = 1;

	unsigned char directionBit = 0;		// �ܺο��� �޾ƿͼ� �����ϱ� ���� �����Ʈ ����

protected:
	unsigned char direction = 5;		// ������ ��� ���� �ִ��� ��������� ���� �� ���

	double GetHPratio() const
	{ 
		if (MAX_HP) 
			return (static_cast<double>(HP) / static_cast<double>(MAX_HP)); 
		return 0.0; 
	}

public:
	BaseLifeObject() {}
	BaseLifeObject(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int hp, const CImage& Image);
	BaseLifeObject(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int hp, unsigned int max_hp, const CImage& Image);
	BaseLifeObject(uVec2i pos, uVec2i size, unsigned int hp, const CImage& Image);
	BaseLifeObject(uVec2i pos, uVec2i size, unsigned int hp, unsigned int max_hp, const CImage& Image);
	virtual ~BaseLifeObject();

	// HP, MaxHP ����
	void SetHP(unsigned int hp) { HP = hp; }
	void SetMaxHP(unsigned int hp)
	{ 
		if (HP > hp) 
			HP = hp;
		MAX_HP = hp; 
	}
	const unsigned int GetHP() const { return HP; }
	const unsigned int GetMaxHP() const { return MAX_HP; }

	// �̵� ���� ���� 
	void SetDirectionBit(unsigned char dir) { directionBit = dir; }
	const unsigned char GetDirectionBit() const { return directionBit; }
	const unsigned char GetDirection() const { return direction; }

	virtual void Update(float fTimeElapsed = 0.0f) override;
};

