#pragma once
#include "Object001_BaseLifeObject.h"

class MovingObject : public BaseLifeObject
{
protected:
	double m_dMoveSpeed = 250.0;		// 이동속도
	float m_fWalkMotionSpeed = 5.0f;	// 이동 애니메이션 속도
	unsigned char WalkFrame = 0;		// 걷는 중 어느 이미지의 프레임을 그릴 지 사용 0, 1, 2, 3...
	unsigned char WalkMotion = 1;		// 걷는 이미지 프레임을 결정. 3이면 1로... 0, 1, 2, 1, 0, 1, 2, 1...	
	float walkAnimation_runtime = 0.0f;	// 걷는 애니메이션 속도를 제어하기 위한 변수

public:
	MovingObject() {}
	MovingObject(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int hp, const CImage& Image);
	MovingObject(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int hp, unsigned int max_hp, const CImage& Image);
	MovingObject(uVec2i pos, uVec2i size, unsigned int hp, const CImage& Image);
	MovingObject(uVec2i pos, uVec2i size, unsigned int hp, unsigned int max_hp, const CImage& Image);
	virtual ~MovingObject();

	// 이동속도
	void SetMovingSpeed(double speed) { m_dMoveSpeed = speed; }
	double GetMovingSpeed() const { return m_dMoveSpeed; }
};
