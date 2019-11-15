#pragma once
#include "Object003_AttackObject.h"

class BasePlayer : public AttackObject
{
private:
	AnimationState m_State = AnimationState::Idle;

public:
	BasePlayer() {}
	BasePlayer(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int hp, const CImage& Image);
	BasePlayer(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int hp, unsigned int max_hp, const CImage& Image);
	BasePlayer(uVec2i pos, uVec2i size, unsigned int hp, const CImage& Image);
	BasePlayer(uVec2i pos, uVec2i size, unsigned int hp, unsigned int max_hp, const CImage& Image);
	virtual ~BasePlayer();
	

	void SetState(AnimationState state) { m_State = state; }
	AnimationState GetState() const { return m_State; }

	virtual void Update(float fTimeElapsed) override;
	virtual void Render(HDC hdc) override;

	struct RenderingPlayerPriority {
		bool operator()(BasePlayer a, BasePlayer b) {
			return a.GetPosition().y > b.GetPosition().y;
		}
	};
};