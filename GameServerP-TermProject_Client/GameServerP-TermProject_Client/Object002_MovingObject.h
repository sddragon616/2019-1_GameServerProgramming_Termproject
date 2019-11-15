#pragma once
#include "Object001_BaseLifeObject.h"

class MovingObject : public BaseLifeObject
{
protected:
	double m_dMoveSpeed = 250.0;		// �̵��ӵ�
	float m_fWalkMotionSpeed = 5.0f;	// �̵� �ִϸ��̼� �ӵ�
	unsigned char WalkFrame = 0;		// �ȴ� �� ��� �̹����� �������� �׸� �� ��� 0, 1, 2, 3...
	unsigned char WalkMotion = 1;		// �ȴ� �̹��� �������� ����. 3�̸� 1��... 0, 1, 2, 1, 0, 1, 2, 1...	
	float walkAnimation_runtime = 0.0f;	// �ȴ� �ִϸ��̼� �ӵ��� �����ϱ� ���� ����

public:
	MovingObject() {}
	MovingObject(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int hp, const CImage& Image);
	MovingObject(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int hp, unsigned int max_hp, const CImage& Image);
	MovingObject(uVec2i pos, uVec2i size, unsigned int hp, const CImage& Image);
	MovingObject(uVec2i pos, uVec2i size, unsigned int hp, unsigned int max_hp, const CImage& Image);
	virtual ~MovingObject();

	// �̵��ӵ�
	void SetMovingSpeed(double speed) { m_dMoveSpeed = speed; }
	double GetMovingSpeed() const { return m_dMoveSpeed; }
};
