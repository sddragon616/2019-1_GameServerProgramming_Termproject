#pragma once
#include "Object002_MovingObject.h"
class AttackObject : public MovingObject
{
protected:
	float m_fAttackMotionSpeed = 10.0f;		// ���� �ִϸ��̼� �ӵ�
	unsigned char AttackFrame = 0;			// ���� �� ��� �̹����� �������� �׸� �� ���
	float attackAnimation_runtime = 0.0f;	// ���� �ִϸ��̼� �ӵ��� �����ϱ� ���� ����

public:
	AttackObject() {}
	AttackObject(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int hp, const CImage& Image);
	AttackObject(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int hp, unsigned int max_hp, const CImage& Image);
	AttackObject(uVec2i pos, uVec2i size, unsigned int hp, const CImage& Image);
	AttackObject(uVec2i pos, uVec2i size, unsigned int hp, unsigned int max_hp, const CImage& Image);
	virtual ~AttackObject();

	// ���ݼӵ� ����
	void SetAttackSpeed(float speed) { m_fAttackMotionSpeed = speed; }

	// ���� ���� �ִϸ��̼� �������� ���ΰ��� �����´�.
	const unsigned int GetAttackFrame() const { return AttackFrame; }

	virtual void Render(HDC hdc) override;

	struct RenderingAttackObjectPriority {
		bool operator()(AttackObject a, AttackObject b) {
			return a.GetPosition().y > b.GetPosition().y;
		}
	};
};

