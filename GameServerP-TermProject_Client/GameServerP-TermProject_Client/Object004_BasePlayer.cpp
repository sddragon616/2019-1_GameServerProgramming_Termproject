#include "stdafx.h"
#include "Object004_BasePlayer.h"
#include "ResourceManager.h"


BasePlayer::BasePlayer(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int hp, const CImage & Image) : AttackObject(x, y, w, h, hp, Image)
{
}

BasePlayer::BasePlayer(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int hp, unsigned int max_hp, const CImage & Image) : AttackObject(x, y, w, h, hp, max_hp, Image)
{
}

BasePlayer::BasePlayer(uVec2i pos, uVec2i size, unsigned int hp, const CImage & Image) : AttackObject(pos, size, hp, Image)
{
}

BasePlayer::BasePlayer(uVec2i pos, uVec2i size, unsigned int hp, unsigned int max_hp, const CImage & Image) : AttackObject(pos, size, hp, max_hp, Image)
{
}

BasePlayer::~BasePlayer()
{
}

void BasePlayer::Update(float fTimeElapsed)
{
	if (m_State != AnimationState::MeleeAttack) {
		AttackFrame = 0;
		attackAnimation_runtime = 0.0f;
	}
	switch (m_State) {
	case AnimationState::Idle:
		if (GetDirectionBit()) m_State = AnimationState::Walk;
		break;
	case AnimationState::Walk:
	{
		double distance = m_dMoveSpeed * fTimeElapsed;
		if (distance < 1.0) distance = 1.0;	// 최소보정, 1 미만의 값인 경우 픽셀 좌표가 바뀌지 않기 때문

		// 좌우 동시입력 처리
		if ((GetDirectionBit() & KEYDIR_LEFT) && (GetDirectionBit() & KEYDIR_RIGHT)) {
			unsigned char dir = GetDirectionBit();
			dir -= KEYDIR_LEFT | KEYDIR_RIGHT;
			SetDirectionBit(dir);
		}
		// 상하 동시입력 처리
		if ((GetDirectionBit() & KEYDIR_UP) && (GetDirectionBit() & KEYDIR_DOWN)) {
			unsigned char dir = GetDirectionBit();
			dir -= KEYDIR_UP | KEYDIR_DOWN;
			SetDirectionBit(dir);
		}

		if (GetDirectionBit() & KEYDIR_UP) {
			direction = 8;
		//	SetPosY(GetPosition().y + static_cast<unsigned int>(distance));
		}

		if (GetDirectionBit() & KEYDIR_DOWN) {
			direction = 2;
		//	SetPosY(GetPosition().y - static_cast<unsigned int>(distance));
		}

		if (GetDirectionBit() & KEYDIR_LEFT) {
			direction = 4;
		//	SetPosX(GetPosition().x - static_cast<unsigned int>(distance));
		}

		if (GetDirectionBit() & KEYDIR_RIGHT) {
			direction = 6;
		//	SetPosX(GetPosition().x + static_cast<unsigned int>(distance));
		}

		if ((GetDirectionBit() & KEYDIR_UP) && (GetDirectionBit() & KEYDIR_LEFT)) direction = 7;
		if ((GetDirectionBit() & KEYDIR_UP) && (GetDirectionBit() & KEYDIR_RIGHT)) direction = 9;
		if ((GetDirectionBit() & KEYDIR_DOWN) && (GetDirectionBit() & KEYDIR_LEFT)) direction = 1;
		if ((GetDirectionBit() & KEYDIR_DOWN) && (GetDirectionBit() & KEYDIR_RIGHT)) direction = 3;

		walkAnimation_runtime += (m_fWalkMotionSpeed * fTimeElapsed);
		WalkMotion = static_cast<unsigned char>(walkAnimation_runtime) % 4;
		WalkFrame = (WalkMotion < 3) ? WalkMotion : 1;

		if (!GetDirectionBit()) {
			m_State = AnimationState::Idle;
		}
	}
		break;
	case AnimationState::MeleeAttack:
		attackAnimation_runtime += (m_fAttackMotionSpeed * fTimeElapsed);
		AttackFrame = static_cast<unsigned char>(attackAnimation_runtime) % 3;
		if (AttackFrame > 1) {
			m_State = AnimationState::Idle;
		}			
		break;
	default:
		break;
	}
}

void BasePlayer::Render(HDC hdc)
{
	if (GetResource()) {
		switch (m_State) {
		case AnimationState::Idle:
			GetResource()->PlayerWalkingDraw(hdc, objectType, GetPosition().x - GetWindowLT().w, GetPosition().y - GetWindowLT().h, direction, 1);
			break;
		case AnimationState::Walk:
			GetResource()->PlayerWalkingDraw(hdc, objectType, GetPosition().x - GetWindowLT().w, GetPosition().y - GetWindowLT().h, direction, WalkFrame);
			break;
		case AnimationState::MeleeAttack:
			GetResource()->PlayerAttackDraw(hdc, objectType, GetPosition().x - GetWindowLT().w, GetPosition().y - GetWindowLT().h, direction, AttackFrame);
			break;
		}
	}
}

//bool PlayerObject::RectAttackCollide(CBaseObject * Target)
//{
//	CBaseObject * meleeAttackHitBox = new CBaseObject();
//	switch (dirrection) {
//	case 8:
//		meleeAttackHitBox->SetPosition(GetPosition().x, GetPosition().y - m_nMeleeAttackRange);
//		break;
//	case 2:
//		meleeAttackHitBox->SetPosition(GetPosition().x, GetPosition().y + m_nMeleeAttackRange);
//		break;
//	case 4:
//		meleeAttackHitBox->SetPosition(GetPosition().x - m_nMeleeAttackRange, GetPosition().y);
//		break;
//	case 6:
//		meleeAttackHitBox->SetPosition(GetPosition().x + m_nMeleeAttackRange, GetPosition().y);
//		break;
//	case 7:
//		meleeAttackHitBox->SetPosition(GetPosition().x - (sqrt(0.5) * m_nMeleeAttackRange), GetPosition().y - (sqrt(0.5) * m_nMeleeAttackRange));
//		break;
//	case 9:
//		meleeAttackHitBox->SetPosition(GetPosition().x + (sqrt(0.5) * m_nMeleeAttackRange), GetPosition().y - (sqrt(0.5) * m_nMeleeAttackRange));
//		break;
//	case 1:
//		meleeAttackHitBox->SetPosition(GetPosition().x - (sqrt(0.5) * m_nMeleeAttackRange), GetPosition().y + (sqrt(0.5) * m_nMeleeAttackRange));
//		break;
//	case 3:
//		meleeAttackHitBox->SetPosition(GetPosition().x + (sqrt(0.5) * m_nMeleeAttackRange), GetPosition().y + (sqrt(0.5) * m_nMeleeAttackRange));
//		break;
//	default:
//		meleeAttackHitBox->SetPosition(GetPosition().x, GetPosition().y);
//		break;
//	}
//	meleeAttackHitBox->SetSize(m_v2dMeleeAttachReach);
//	if (Target)
//		return Target->RectCollide(meleeAttackHitBox->GetPosition(), meleeAttackHitBox->GetSize());
//	// return Target->RectCollide(meleeAttackHitBox);
//	else
//		return false;
//}
