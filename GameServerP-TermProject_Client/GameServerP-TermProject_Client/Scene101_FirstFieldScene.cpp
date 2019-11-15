#include "stdafx.h"
#include "Scene101_FirstFieldScene.h"
#include "Framework.h"
#include "ResourceManager.h"
#include "Object005_MyPlayer.h"

FirstFieldScene::~FirstFieldScene()
{
}

void FirstFieldScene::BuildObjects()
{
	m_pPlayer = new MyPlayer(static_cast<unsigned int>(500), static_cast<unsigned int>(310), static_cast<unsigned int>(32), static_cast<unsigned int>(64), static_cast<unsigned int>(100), static_cast<unsigned int>(30), static_cast<unsigned int>(50), static_cast<unsigned int>(1), static_cast<unsigned int>(100), m_pFramework->GetResource()->GetSwordManImage());
	m_pPlayer->SetObjectType(0);
	if (m_pFramework)
		if (m_pFramework->GetResource()) {
			m_pPlayer->SetResource(m_pFramework->GetResource());
			m_pPlayer->SetBackgroundSize(m_pPlayer->GetResource()->GetMapImage().GetWidth(), m_pPlayer->GetResource()->GetMapImage().GetHeight());
		}
	m_pPlayer->CenterPlayerScrolling();
}

bool FirstFieldScene::ProcessInput(UCHAR * keybuffer, float fTimeElapsed)
{
	send_time += fTimeElapsed;
	if (m_pPlayer) {
		m_pPlayer->ProcessInput(keybuffer, fTimeElapsed);
		if (m_pPlayer->GetDirectionBit() && m_pPlayer->GetState() != MeleeAttack) {
			if (send_time > 0.1f) {
				send_time = 0.0f;
				SendMoveDirection();
			}
		}
		if (!m_pPlayer->GetDirectionBit()) {
			return true;
		}
	}
	return false;
}

bool FirstFieldScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		// 키보드를 누르고 있을 경우 최초 한번만 실행.
		if ((lParam & 0x40000000) != 0x40000000) {
			switch (wParam) {
			case VK_RETURN: {
				printf("Chatting >> "); 
				cs_packet_chat packet;
				wscanf_s(L"%s", packet.message, MAX_STR_LEN);
				packet.size = sizeof(packet);
				packet.type = CS_CHAT;
				m_pFramework->SendPacket(reinterpret_cast<char *>(&packet));
			}
				break;
			case 'o': case 'O':
#ifdef _DEBUG
				m_pPlayer->DrawHitBoxTriggerSet();
				m_pPlayer->DrawCircleRangeTriggerSet();
				if (m_ppOtherPlayers) {
					for (unsigned int i = 0; i < MAX_USER; ++i) {
						if (m_ppOtherPlayers[i]) {						
							m_ppOtherPlayers[i]->DrawHitBoxTriggerSet(m_pPlayer->isHitBoxDraw());
							m_ppOtherPlayers[i]->DrawCircleRangeTriggerSet(m_pPlayer->isCircleRangeDraw());
						}
					}
				}
				if (m_ppMonsterObject) {
					for (unsigned int i = 0; i < NUM_NPC + NUM_SMART_NPC; ++i) {
						if (m_ppMonsterObject[i]) {
							m_ppMonsterObject[i]->DrawHitBoxTriggerSet(m_pPlayer->isHitBoxDraw());
							m_ppMonsterObject[i]->DrawCircleRangeTriggerSet(m_pPlayer->isCircleRangeDraw());
						}
					}
				}
#endif
				break;
			case 'z': case 'Z': 
				if (m_pPlayer->GetState() != AnimationState::MeleeAttack) {
					m_pPlayer->SetState(AnimationState::MeleeAttack);
					if (m_pPlayer && m_pPlayer->GetDirection()) {
						cs_packet_attack *myAttackPacket = reinterpret_cast<cs_packet_attack *>(m_pFramework->GetSendBuf());
						myAttackPacket->size = sizeof(cs_packet_move);
						myAttackPacket->type = CS_ATTACK;
						switch (m_pPlayer->GetDirection()) {
						case 2: case 5:
							myAttackPacket->direction = DIR_DOWN;
							break;
						case 4:
							myAttackPacket->direction = DIR_LEFT;
							break;
						case 6:
							myAttackPacket->direction = DIR_RIGHT;
							break;
						case 8:
							myAttackPacket->direction = DIR_UP;
							break;
						case 7:
							myAttackPacket->direction = DIR_LEFT_UP;
							break;
						case 9:
							myAttackPacket->direction = DIR_RIGHT_UP;
							break;
						case 1:
							myAttackPacket->direction = DIR_LEFT_DOWN;
							break;
						case 3:
							myAttackPacket->direction = DIR_RIGHT_DOWN;
							break;
						default:
							consolePrint("Unknown Direction\n");
							return false;
						}
						m_pFramework->SendPacket(reinterpret_cast<char *>(myAttackPacket));
					}
				}
				break;
			}

		}
		break;
	case WM_KEYUP:
		break;
	}
	return false;
}

bool FirstFieldScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

void FirstFieldScene::ProcessPacket(char * ptr)
{
	static bool first_time = true;
	switch (ptr[1])
	{
	case SC_ADD_OBJECT:
	{
		sc_packet_add_object *my_packet = reinterpret_cast<sc_packet_add_object *>(ptr);
		unsigned int id = my_packet->id;
		if (first_time) {
			first_time = false;
			m_pFramework->SetMyID(id);	
		}
		if (id == m_pFramework->GetMyid()) {
			m_pPlayer->SetPosition(my_packet->x * tile_unit, my_packet->y * tile_unit);
			m_pPlayer->SetObjectType(my_packet->obj_class);
#ifdef USE_CONSOLE_WINDOW
			printf("Your [%d] : (%d, %d)\n", my_packet->id, my_packet->x, my_packet->y);
#endif
		}
		else if (id < MAX_USER) {
			if (!m_ppOtherPlayers[id]) {
				m_ppOtherPlayers[id] = new BasePlayer();
				m_ppOtherPlayers[id]->SetObjectType(my_packet->obj_class);
				if (m_pFramework)
					if (m_pFramework->GetResource()) {					
						m_ppOtherPlayers[id]->SetResource(m_pFramework->GetResource());
						m_ppOtherPlayers[id]->SetBackgroundSize(m_pPlayer->GetResource()->GetMapImage().GetWidth(), m_pPlayer->GetResource()->GetMapImage().GetHeight());
						m_ppOtherPlayers[id]->DrawHitBoxTriggerSet(m_pPlayer->isHitBoxDraw());
						m_ppOtherPlayers[id]->DrawCircleRangeTriggerSet(m_pPlayer->isCircleRangeDraw());
					}
			}
			m_ppOtherPlayers[id]->SetPosition(my_packet->x * tile_unit, my_packet->y * tile_unit);
#ifdef USE_CONSOLE_WINDOW
			printf("Put Player [%d] : (%d, %d)\n", my_packet->id, my_packet->x, my_packet->y);
#endif
		}
		else if (id >= NPC_ID_START){
			if (!m_ppMonsterObject[id - NPC_ID_START]) {
				m_ppMonsterObject[id - NPC_ID_START] = new AttackObject();
				
				m_ppMonsterObject[id - NPC_ID_START]->SetHP(my_packet->HP);
				m_ppMonsterObject[id - NPC_ID_START]->SetMaxHP(my_packet->MAX_HP);
				if (m_pFramework)
					if (m_pFramework->GetResource()) {						
						m_ppMonsterObject[id - NPC_ID_START]->SetResource(m_pFramework->GetResource());
						m_ppMonsterObject[id - NPC_ID_START]->SetBackgroundSize(m_pPlayer->GetResource()->GetMapImage().GetWidth(), m_pPlayer->GetResource()->GetMapImage().GetHeight());
						m_ppMonsterObject[id - NPC_ID_START]->DrawHitBoxTriggerSet(m_pPlayer->isHitBoxDraw());
						m_ppMonsterObject[id - NPC_ID_START]->DrawCircleRangeTriggerSet(m_pPlayer->isCircleRangeDraw());
					}
			}
			m_ppMonsterObject[id - NPC_ID_START]->SetObjectType(my_packet->obj_class);
			m_ppMonsterObject[id - NPC_ID_START]->SetPosition(my_packet->x * tile_unit, my_packet->y * tile_unit);
			//consolePrint("MOVE Object [%d] : (%d, %d)\n", my_packet->id - NPC_ID_START, my_packet->x, my_packet->y);
		}
		break;
	}
	case SC_POSITION:
	{
		sc_packet_position *my_packet = reinterpret_cast<sc_packet_position *>(ptr);
		unsigned int other_id = my_packet->id;
		if (other_id == m_pFramework->GetMyid()) {
			m_pPlayer->SetPosition(my_packet->x * tile_unit, my_packet->y * tile_unit);
			// consolePrint("MOVE Player [%d] : (%d, %d)\n", my_packet->id, my_packet->x, my_packet->y);
		}
		else if (other_id < MAX_USER) {
			if (!m_ppOtherPlayers[other_id]){
				m_ppOtherPlayers[other_id] = new BasePlayer();
				m_ppOtherPlayers[other_id]->SetObjectType(1);
				if (m_pFramework)
					if (m_pFramework->GetResource()) {				
						m_ppOtherPlayers[other_id]->SetResource(m_pFramework->GetResource());
						if (m_pPlayer)
							m_ppOtherPlayers[other_id]->SetBackgroundSize(m_pPlayer->GetResource()->GetMapImage().GetWidth(), m_pPlayer->GetResource()->GetMapImage().GetHeight());
					}
			}
			m_ppOtherPlayers[other_id]->SetPosition(my_packet->x * tile_unit, my_packet->y * tile_unit);
		}
		else {
			if (!m_ppMonsterObject[other_id - NPC_ID_START]) {
				m_ppMonsterObject[other_id - NPC_ID_START] = new AttackObject();
				if (m_pFramework)
					if (m_pFramework->GetResource()) {
						m_ppMonsterObject[other_id - NPC_ID_START]->SetResource(m_pFramework->GetResource());
						if (m_pPlayer)
							m_ppMonsterObject[other_id - NPC_ID_START]->SetBackgroundSize(m_pPlayer->GetResource()->GetMapImage().GetWidth(), m_pPlayer->GetResource()->GetMapImage().GetHeight());
					}
			}
			m_ppMonsterObject[other_id - NPC_ID_START]->SetPosition(my_packet->x * tile_unit, my_packet->y * tile_unit);
			//consolePrint("MOVE Object [%d] : (%d, %d)\n", my_packet->id - NPC_ID_START, my_packet->x, my_packet->y);
		}
		break;
	}
	case SC_REMOVE_OBJECT:
	{
		sc_packet_remove_object *my_packet = reinterpret_cast<sc_packet_remove_object *>(ptr);
		unsigned int other_id = my_packet->id;
		if (other_id < MAX_USER) {
			if (m_ppOtherPlayers[other_id]) {
				delete m_ppOtherPlayers[other_id];
				m_ppOtherPlayers[other_id] = nullptr;
#ifdef USE_CONSOLE_WINDOW
				printf("Player [%d] Remove from Screen\n", my_packet->id);
#endif
			}
		}
		else {
			if (m_ppMonsterObject[other_id - NPC_ID_START]) {
				delete m_ppMonsterObject[other_id - NPC_ID_START];
				m_ppMonsterObject[other_id - NPC_ID_START] = nullptr;
			}
		}
		break;
	}
	case SC_CHAT: 
	{
		sc_packet_chat *my_packet = reinterpret_cast<sc_packet_chat *>(ptr);
		unsigned int other_id = my_packet->id;
		for (char i = 10 - 1; i > 0; --i)
			wcsncpy_s(chatting[i], chatting[i - 1], MAX_STR_LEN);
		wcsncpy_s(chatting[0], my_packet->message, MAX_STR_LEN);

		// consolePrint("MOVE Player [%d] : (%d, %d)\n", my_packet->id, m_pPlayer->GetPosition().x / tile_unit, m_pPlayer->GetPosition().y / tile_unit);
	}
		break;
	default:
	case SC_STAT_CHANGE:
	{
		sc_packet_stat_change* packet = reinterpret_cast<sc_packet_stat_change *>(ptr);
		if (packet->id == m_pFramework->GetMyid()) {
			if (m_pPlayer) {
				m_pPlayer->SetLevel(packet->LEVEL);
				m_pPlayer->SetHP(packet->HP);
				m_pPlayer->SetMaxHP(packet->MAX_HP);
				m_pPlayer->SetMP(packet->MP);
				m_pPlayer->SetMaxMP(packet->MAX_MP);
				m_pPlayer->SetStamina(packet->SP);
				m_pPlayer->SetMaxStamina(packet->MAX_SP);
				m_pPlayer->SetEXP(packet->EXP);
				m_pPlayer->SetMaxEXP(packet->MAX_EXP);
				m_pPlayer->SetATK(packet->ATK);
				m_pPlayer->SetDEF(packet->DEF);
				m_pPlayer->SetMAG(packet->MAG);
				m_pPlayer->SetMR(packet->MR);
				m_pPlayer->SetAP(packet->Ability_Point);
				m_pPlayer->SetSkillPoint(packet->Skill_Point);
			}
		}
	}
		break;
#ifdef USE_CONSOLE_WINDOW
		printf("Unknown PACKET type [%d]\n", ptr[1]);
#endif
	}
}

void FirstFieldScene::Update(float fTimeElapsed)
{
	if (m_pPlayer) {
		m_pPlayer->Update(fTimeElapsed);

	}
}

//#include <queue> // 렌더링 순서를 y값이 높은 녀석은 뒤에 서 있도록


void FirstFieldScene::Render(HDC hdc)
{
	if (m_pFramework)
		if (m_pFramework->GetResource()) {
			if (m_pPlayer) {
				m_pFramework->GetResource()->GetMapImage().Draw(hdc, 0, 0, CLIENT_WIDTH, CLIENT_HEIGHT, m_pPlayer->GetWindowLT().w, m_pFramework->GetResource()->GetMapImage().GetHeight() - CLIENT_HEIGHT - m_pPlayer->GetWindowLT().h, CLIENT_WIDTH, CLIENT_HEIGHT);
				m_pPlayer->CenterPlayerScrolling();
			}
			//std::priority_queue<BasePlayer, std::vector<BasePlayer>, BasePlayer::RenderingPlayerPriority> pq;
			if (m_ppOtherPlayers) {
				for (unsigned int i = 0; i < MAX_USER; ++i) {
					
					if (m_ppOtherPlayers[i]) {
						if (m_pPlayer)
							m_ppOtherPlayers[i]->CenterStandardScrolling(m_pPlayer);
						m_ppOtherPlayers[i]->Render(hdc);

						m_ppOtherPlayers[i]->DrawHitBox(hdc);
						m_ppOtherPlayers[i]->DrawCircleRange(hdc, 150);
					}
				}
			}
			//std::priority_queue<AttackObject, std::vector<AttackObject>, BasePlayer::RenderingAttackObjectPriority> aq;
			if (m_ppMonsterObject) {
				for (unsigned int i = NUM_NPC + NUM_SMART_NPC - 1; i > 0 ;--i) {
					if (m_ppMonsterObject[i]) {
						if (m_pPlayer)
							m_ppMonsterObject[i]->CenterStandardScrolling(m_pPlayer);
						// aq.push(*m_ppMonsterObject[i]);
						m_ppMonsterObject[i]->Render(hdc);

						m_ppMonsterObject[i]->DrawHitBox(hdc);
						m_ppMonsterObject[i]->DrawCircleRange(hdc, 150);
					}
				}
			}

			if (m_pPlayer) {		
				m_pPlayer->Render(hdc);

				m_pPlayer->DrawHitBox(hdc);
				m_pPlayer->DrawCircleRange(hdc, 150);
			}
		}
	for (short i = 0; i < 10; ++i)
		DrawFont(hdc, chatting[i], 10, CLIENT_HEIGHT - 50 - (i * 30), 10, 25, RGB(0, 0, 0));
	
}


void FirstFieldScene::SendMoveDirection()
{
	if (m_pPlayer && m_pPlayer->GetDirectionBit()) {
		cs_packet_move *myMovePacket = reinterpret_cast<cs_packet_move *>(m_pFramework->GetSendBuf());
		myMovePacket->size = sizeof(cs_packet_move);
		myMovePacket->type = CS_MOVE;


		char direction = 0;
		// 좌우 동시입력 처리
		if ((m_pPlayer->GetDirectionBit() & KEYDIR_LEFT) && (m_pPlayer->GetDirectionBit() & KEYDIR_RIGHT)) {
			unsigned char dir = m_pPlayer->GetDirectionBit();
			dir -= KEYDIR_LEFT | KEYDIR_RIGHT;
			m_pPlayer->SetDirectionBit(dir);
		}
		// 상하 동시입력 처리
		if ((m_pPlayer->GetDirectionBit() & KEYDIR_UP) && (m_pPlayer->GetDirectionBit() & KEYDIR_DOWN)) {
			unsigned char dir = m_pPlayer->GetDirectionBit();
			dir -= KEYDIR_UP | KEYDIR_DOWN;
			m_pPlayer->SetDirectionBit(dir);
		}

		if (m_pPlayer->GetDirectionBit() & KEYDIR_UP) {
			direction = 8;
			//	SetPosY(GetPosition().y + static_cast<unsigned int>(distance));
		}

		if (m_pPlayer->GetDirectionBit() & KEYDIR_DOWN) {
			direction = 2;
			//	SetPosY(GetPosition().y - static_cast<unsigned int>(distance));
		}

		if (m_pPlayer->GetDirectionBit() & KEYDIR_LEFT) {
			direction = 4;
			//	SetPosX(GetPosition().x - static_cast<unsigned int>(distance));
		}

		if (m_pPlayer->GetDirectionBit() & KEYDIR_RIGHT) {
			direction = 6;
			//	SetPosX(GetPosition().x + static_cast<unsigned int>(distance));
		}

		if ((m_pPlayer->GetDirectionBit() & KEYDIR_UP) && (m_pPlayer->GetDirectionBit() & KEYDIR_LEFT)) direction = 7;
		if ((m_pPlayer->GetDirectionBit() & KEYDIR_UP) && (m_pPlayer->GetDirectionBit() & KEYDIR_RIGHT)) direction = 9;
		if ((m_pPlayer->GetDirectionBit() & KEYDIR_DOWN) && (m_pPlayer->GetDirectionBit() & KEYDIR_LEFT)) direction = 1;
		if ((m_pPlayer->GetDirectionBit() & KEYDIR_DOWN) && (m_pPlayer->GetDirectionBit() & KEYDIR_RIGHT)) direction = 3;

		switch (direction) {
		case 2: case 5:
			myMovePacket->direction = DIR_DOWN;
			break;
		case 4:
			myMovePacket->direction = DIR_LEFT;
			break;
		case 6:
			myMovePacket->direction = DIR_RIGHT;
			break;
		case 8:
			myMovePacket->direction = DIR_UP;
			break;
		case 7:
			myMovePacket->direction = DIR_LEFT_UP;
			break;
		case 9:
			myMovePacket->direction = DIR_RIGHT_UP;
			break;
		case 1:
			myMovePacket->direction = DIR_LEFT_DOWN;
			break;
		case 3:
			myMovePacket->direction = DIR_RIGHT_DOWN;
			break;
		default:
			consolePrint("Unknown Direction\n");
			return;
		}
		m_pFramework->SendPacket(reinterpret_cast<char *>(myMovePacket));
	}
}
