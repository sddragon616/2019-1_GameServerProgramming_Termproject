#include "stdafx.h"
#include "Object000_BaseObject.h"


BaseObject::~BaseObject()
{
}



void BaseObject::DrawHitBoxTriggerSet()
{
	if (DrawBoxTrigger)
		DrawBoxTrigger = false;
	else
		DrawBoxTrigger = true;
}

void BaseObject::DrawHitBoxTriggerSet(bool Trigger)
{
	DrawBoxTrigger = Trigger;
}

void BaseObject::DrawHitBox(HDC hdc)
{
	if (DrawBoxTrigger) {
		HPEN hPen, oldPen;
		hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
		oldPen = (HPEN)SelectObject(hdc, hPen);

		// 윗변
		MoveToEx(hdc, m_v2dPosition.x /*- (m_v2dSize.width / 2)*/ - window_left, CLIENT_HEIGHT - (m_v2dPosition.y + (m_v2dSize.height)) + window_top, NULL);
		LineTo(hdc, m_v2dPosition.x +(m_v2dSize.width ) - window_left, CLIENT_HEIGHT - (m_v2dPosition.y + (m_v2dSize.height)) + window_top);

		// 밑변
		MoveToEx(hdc, m_v2dPosition.x /*- (m_v2dSize.width / 2) */- window_left, CLIENT_HEIGHT - (m_v2dPosition.y) + window_top, NULL);
		LineTo(hdc, m_v2dPosition.x + (m_v2dSize.width ) - window_left, CLIENT_HEIGHT - (m_v2dPosition.y) + window_top);

		// 좌변
		MoveToEx(hdc, m_v2dPosition.x/* - (m_v2dSize.width / 2)*/ - window_left, CLIENT_HEIGHT - (m_v2dPosition.y + (m_v2dSize.height)) + window_top, NULL);
		LineTo(hdc, m_v2dPosition.x/* - (m_v2dSize.width / 2)*/ - window_left, CLIENT_HEIGHT - (m_v2dPosition.y) + window_top);

		// 우변
		MoveToEx(hdc, m_v2dPosition.x + (m_v2dSize.width  ) - window_left, CLIENT_HEIGHT - (m_v2dPosition.y + (m_v2dSize.height)) + window_top, NULL);
		LineTo(hdc, m_v2dPosition.x + (m_v2dSize.width ) - window_left, CLIENT_HEIGHT - (m_v2dPosition.y) + window_top);

		SelectObject(hdc, oldPen);
		DeleteObject(hPen);
	}
}

void BaseObject::DrawCircleRangeTriggerSet()
{
	if (DrawCircleTrigger)
		DrawCircleTrigger = false;
	else
		DrawCircleTrigger = true;
}

void BaseObject::DrawCircleRangeTriggerSet(bool Trigger)
{
	DrawCircleTrigger = Trigger;
}

void BaseObject::DrawCircleRange(HDC hdc, short range)
{
	if (DrawCircleTrigger) {
		HPEN hPen, oldPen;
		hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
		oldPen = (HPEN)SelectObject(hdc, hPen);

		const unsigned short Angles = 32;	// 원을 몇각형으로 그릴 것인가.
		double ratio = (2 * M_PI) / static_cast<double>(Angles);
		
		for (double angle = 0.0; angle < 2 * M_PI; angle += ratio) {
			int x = static_cast<int>((static_cast<double>(range) * cos(angle)) - (static_cast<double>(range) * sin(angle))) + m_v2dPosition.x - window_left;
			int y = static_cast<int>((static_cast<double>(range) * sin(angle)) + (static_cast<double>(range) * cos(angle))) + CLIENT_HEIGHT - m_v2dPosition.y + window_top;
			MoveToEx(hdc, x, y, NULL);
			double nextTheta = angle + ratio;
			int next_x = static_cast<int>((static_cast<double>(range) * cos(nextTheta)) - (static_cast<double>(range) * sin(nextTheta))) + m_v2dPosition.x - window_left;
			int next_y = static_cast<int>((static_cast<double>(range) * sin(nextTheta)) + (static_cast<double>(range) * cos(nextTheta))) + CLIENT_HEIGHT - m_v2dPosition.y + window_top;
			LineTo(hdc, next_x, next_y);
		}

		SelectObject(hdc, oldPen);
		DeleteObject(hPen);
	}
}

bool BaseObject::ProcessInput(unsigned char * keybuffer, float fTimeElapsed)
{
	return false;
}

void BaseObject::Update(float fTimeElapsed)
{
}

void BaseObject::Render(HDC hdc)
{
	if (ObjectImage)
		ObjectImage.Draw(hdc, m_v2dPosition.x - window_left, m_v2dPosition.y + window_top, m_v2dSize.width, m_v2dSize.height);
}

bool BaseObject::RectCollide(uVec2i position, uVec2i size)
{
	if (m_v2dPosition.x - (m_v2dSize.width / 2) >= position.x + (size.width / 2))
		return false;
	if (m_v2dPosition.x + (m_v2dSize.width / 2) <= position.x - (size.width / 2))
		return false;
	if (m_v2dPosition.y - (m_v2dSize.height / 2) >= position.y + (size.height / 2))
		return false;
	if (m_v2dPosition.y + (m_v2dSize.height / 2) <= position.y - (size.height / 2))
		return false;
	return true;
}

bool BaseObject::RectCollide(BaseObject * other)
{
	if (m_v2dPosition.x - (m_v2dSize.width / 2) >= other->GetPosition().x + (other->GetSize().width / 2))
		return false;
	if (m_v2dPosition.x + (m_v2dSize.width / 2) <= other->GetPosition().x - (other->GetSize().width / 2))
		return false;
	if (m_v2dPosition.y - (m_v2dSize.height / 2) >= other->GetPosition().y + (other->GetSize().height / 2))
		return false;
	if (m_v2dPosition.y + (m_v2dSize.height / 2) <= other->GetPosition().y - (other->GetSize().height / 2))
		return false;
	return true;
}

bool BaseObject::CircleCollide()
{
	return false;
}

bool BaseObject::RangeInRectCollide()
{
	return false;
}

//bool CBaseObject::ObstacleRectCollide(PlayerObject * player, double distance)
//{
//	/* 이 오브젝트와 충돌한 other 오브젝트를 원래대로 밀어냄 */
//	/*
//	while (RectCollide(player)) {
//		if (m_v2dPosition.x < player->GetPosition().x - (player->GetSize().width / 2)) {
//			player->SetPosition(m_v2dPosition.x + (m_v2dSize.width / 2) + (player->GetSize().width / 2), player->GetPosition().y);
//			continue;
//		}
//		if (m_v2dPosition.x > player->GetPosition().x + (player->GetSize().width / 2)) {
//			player->SetPosition(m_v2dPosition.x - (m_v2dSize.width / 2) - (player->GetSize().width / 2), player->GetPosition().y);
//			continue;
//		}
//		if (m_v2dPosition.y > player->GetPosition().y + (player->GetSize().height / 2)) {
//			player->SetPosition(player->GetPosition().x, m_v2dPosition.y - (m_v2dSize.height / 2) - (player->GetSize().height / 2));
//			continue;
//		}
//		if (m_v2dPosition.y < player->GetPosition().y - (player->GetSize().height / 2)) {
//			player->SetPosition(player->GetPosition().x, m_v2dPosition.y + (m_v2dSize.height / 2) + (player->GetSize().height / 2));
//			continue;
//		}
//	}*/
//	/*
//	if (RectCollide(player)) {
//		switch (player->GetDirection()) {
//		case 8:
//			player->SetPosition(player->GetPosition().x, m_v2dPosition.y + (m_v2dSize.height / 2) + (player->GetSize().height / 2));
//			break;
//		case 2:
//			player->SetPosition(player->GetPosition().x, m_v2dPosition.y - (m_v2dSize.height / 2) - (player->GetSize().height / 2));
//			break;
//		case 4:
//			player->SetPosition(m_v2dPosition.x + (m_v2dSize.width / 2) + (player->GetSize().width / 2), player->GetPosition().y);
//			break;
//		case 6:
//			player->SetPosition(m_v2dPosition.x - (m_v2dSize.width / 2) - (player->GetSize().width / 2), player->GetPosition().y);
//			break;
//		case 7:
//			player->SetPosition(player->GetPosition().x, m_v2dPosition.y + (m_v2dSize.height / 2) + (player->GetSize().height / 2));
//			player->SetPosition(m_v2dPosition.x + (m_v2dSize.width / 2) + (player->GetSize().width / 2), player->GetPosition().y);
//			break;
//		case 9:
//			player->SetPosition(player->GetPosition().x, m_v2dPosition.y + (m_v2dSize.height / 2) + (player->GetSize().height / 2));
//			player->SetPosition(m_v2dPosition.x - (m_v2dSize.width / 2) - (player->GetSize().width / 2), player->GetPosition().y);
//			break;
//		case 1:
//			player->SetPosition(player->GetPosition().x, m_v2dPosition.y - (m_v2dSize.height / 2) - (player->GetSize().height / 2));
//			player->SetPosition(m_v2dPosition.x + (m_v2dSize.width / 2) + (player->GetSize().width / 2), player->GetPosition().y);
//			break;
//		case 3:
//			player->SetPosition(player->GetPosition().x, m_v2dPosition.y - (m_v2dSize.height / 2) - (player->GetSize().height / 2));
//			player->SetPosition(m_v2dPosition.x - (m_v2dSize.width / 2) - (player->GetSize().width / 2), player->GetPosition().y);
//			break;
//		}
//		return true;
//	}
//	*/
//	/*
//	while (RectCollide(player)) {
//		if (player->GetDirectionBit() & DIR_UP) {
//			player->SetPosition(player->GetPosition().x, m_v2dPosition.y + (m_v2dSize.height / 2) + (player->GetSize().height / 2));
//			continue;
//		}
//		if (player->GetDirectionBit() & DIR_DOWN) {
//			player->SetPosition(player->GetPosition().x, m_v2dPosition.y - (m_v2dSize.height / 2) - (player->GetSize().height / 2));
//			continue;
//		}
//		if (player->GetDirectionBit() & DIR_LEFT) {
//			player->SetPosition(m_v2dPosition.x + (m_v2dSize.width / 2) + (player->GetSize().width / 2), player->GetPosition().y);
//			continue;
//		}
//		if (player->GetDirectionBit() & DIR_RIGHT) {
//			player->SetPosition(m_v2dPosition.x - (m_v2dSize.width / 2) - (player->GetSize().width / 2), player->GetPosition().y);
//			continue;
//		}
//		return true;
//	}
//	*/
//	/*
//	if (RectCollide(player)) {
//		if (player->GetDirectionBit() & DIR_UP) {
//			player->SetPosition(player->GetPosition().x, m_v2dPosition.y + (m_v2dSize.height / 2) + (player->GetSize().height / 2));
//			return true;
//		}
//		if (player->GetDirectionBit() & DIR_DOWN) {
//			player->SetPosition(player->GetPosition().x, m_v2dPosition.y - (m_v2dSize.height / 2) - (player->GetSize().height / 2));
//			return true;
//		}
//		if (player->GetDirectionBit() & DIR_LEFT) {
//			player->SetPosition(m_v2dPosition.x + (m_v2dSize.width / 2) + (player->GetSize().width / 2), player->GetPosition().y);
//			return true;
//		}
//		if (player->GetDirectionBit() & DIR_RIGHT) {
//			player->SetPosition(m_v2dPosition.x - (m_v2dSize.width / 2) - (player->GetSize().width / 2), player->GetPosition().y);
//			return true;
//		}
//	}
//	*/
//	if (RectCollide(player)) {
//		switch (player->GetDirection()) {
//		case 8:
//			player->SetPosY(min(static_cast<int>(player->GetPosition().y + distance), player->GetBackgroundSize().height - (player->GetSize().height / 2)));
//			break;
//		case 2:
//			player->SetPosY(max(static_cast<int>(player->GetPosition().y - distance), (player->GetSize().height / 2)));
//			break;
//		case 4:
//			player->SetPosX(min(static_cast<int>(player->GetPosition().x + distance), player->GetBackgroundSize().width - (player->GetSize().width / 2)));
//			break;
//		case 6:
//			player->SetPosX(max(static_cast<int>(player->GetPosition().x - distance), (player->GetSize().width / 2)));
//			break;
//		case 7:
//			if (this->GetPosition().x + this->GetSize().width > player->GetPosition().x - (player->GetSize().width / 2)) {
//				player->SetPosX(min(static_cast<int>(player->GetPosition().x + distance), player->GetBackgroundSize().width - (player->GetSize().width / 2)));
//				//return true;
//			}
//			if (this->GetPosition().y + this->GetSize().height > player->GetPosition().y - (player->GetSize().height / 2)) {
//				player->SetPosY(min(static_cast<int>(player->GetPosition().y + distance), player->GetBackgroundSize().height - (player->GetSize().height / 2)));
//				//return true;
//			}
//			break;
//		case 9:
//			if (this->GetPosition().x - this->GetSize().width < player->GetPosition().x + (player->GetSize().width / 2)) {
//				player->SetPosX(max(static_cast<int>(player->GetPosition().x - distance), (player->GetSize().width / 2)));
//				//return true;
//			}
//			if (this->GetPosition().y + this->GetSize().height > player->GetPosition().y - (player->GetSize().height / 2)) {
//				player->SetPosY(min(static_cast<int>(player->GetPosition().y + distance), player->GetBackgroundSize().height - (player->GetSize().height / 2)));
//				//return true;
//			}
//			break;
//		case 1:
//			if (this->GetPosition().x + this->GetSize().width > player->GetPosition().x - (player->GetSize().width / 2)) {
//				player->SetPosX(min(static_cast<int>(player->GetPosition().x + distance), player->GetBackgroundSize().width - (player->GetSize().width / 2)));
//				//return true;
//			}
//			if (this->GetPosition().y - this->GetSize().height < player->GetPosition().y + (player->GetSize().height / 2)) {
//				player->SetPosY(max(static_cast<int>(player->GetPosition().y - distance), (player->GetSize().height / 2)));
//				//return true;
//			}
//			break;
//		case 3:
//			if (this->GetPosition().x - this->GetSize().width < player->GetPosition().x + (player->GetSize().width / 2)) {
//				player->SetPosX(max(static_cast<int>(player->GetPosition().x - distance), (player->GetSize().width / 2)));
//				//return true;
//			}
//			if (this->GetPosition().y - this->GetSize().height < player->GetPosition().y + (player->GetSize().height / 2)) {
//				player->SetPosY(max(static_cast<int>(player->GetPosition().y - distance), (player->GetSize().height / 2)));
//				//return true;
//			}
//			break;
//		}
//		return true;
//	}
//	return false;
//}