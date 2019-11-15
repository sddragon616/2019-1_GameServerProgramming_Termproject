#pragma once
#include "Object002_MovingObject.h"
class AttackObject : public MovingObject
{
protected:
	float m_fAttackMotionSpeed = 10.0f;		// 공격 애니메이션 속도
	unsigned char AttackFrame = 0;			// 공격 중 어느 이미지의 프레임을 그릴 지 사용
	float attackAnimation_runtime = 0.0f;	// 공격 애니메이션 속도를 제어하기 위한 변수

public:
	AttackObject() {}
	AttackObject(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int hp, const CImage& Image);
	AttackObject(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int hp, unsigned int max_hp, const CImage& Image);
	AttackObject(uVec2i pos, uVec2i size, unsigned int hp, const CImage& Image);
	AttackObject(uVec2i pos, uVec2i size, unsigned int hp, unsigned int max_hp, const CImage& Image);
	virtual ~AttackObject();

	// 공격속도 세팅
	void SetAttackSpeed(float speed) { m_fAttackMotionSpeed = speed; }

	// 현재 공격 애니메이션 프레임이 몇인가를 가져온다.
	const unsigned int GetAttackFrame() const { return AttackFrame; }

	virtual void Render(HDC hdc) override;

	struct RenderingAttackObjectPriority {
		bool operator()(AttackObject a, AttackObject b) {
			return a.GetPosition().y > b.GetPosition().y;
		}
	};
};

