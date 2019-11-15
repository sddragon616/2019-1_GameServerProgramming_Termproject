#include "stdafx.h"
#include "Scene002_TitleScene.h"
#include "Scene101_FirstFieldScene.h"
#include "Object005_MyPlayer.h"
#include "Framework.h"
#include "ResourceManager.h"
//#include "Object000_BaseObject.h"

TitleScene::~TitleScene()
{
}

void TitleScene::BuildObjects()
{
}

bool TitleScene::ProcessInput(UCHAR * keybuffer, float fTimeElapsed)
{
	if (keybuffer)
		if (keybuffer[VK_SPACE] & 0xF0) {
			if(m_pFramework) m_pFramework->ServerConnect();
			SendLoginPacket();
		}

	return false;
}

bool TitleScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool TitleScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

void TitleScene::ProcessPacket(char * ptr)
{
	switch (ptr[1])
	{
	case SC_LOGIN_OK:
	{
		sc_packet_login_ok *packet = reinterpret_cast<sc_packet_login_ok *>(ptr);
		m_pFramework->SetMyID(packet->id);
	
		consolePrint("User [%d] 필드 입장\n", packet->id);
		if (m_pFramework) {
			m_pFramework->ChangeScene(SceneTag::Field);
			FirstFieldScene* Field = reinterpret_cast<FirstFieldScene*>(m_pFramework->GetNowScene());
			Field->GetPlayer()->SetObjectType(packet->playerType);
			Field->GetPlayer()->SetLevel(packet->LEVEL);
			Field->GetPlayer()->SetHP(packet->HP);
			Field->GetPlayer()->SetMaxHP(packet->MAX_HP);
			Field->GetPlayer()->SetMP(packet->MP);
			Field->GetPlayer()->SetMaxMP(packet->MAX_MP);
			Field->GetPlayer()->SetStamina(packet->SP);
			Field->GetPlayer()->SetMaxStamina(packet->MAX_SP);
			Field->GetPlayer()->SetEXP(packet->EXP);
			Field->GetPlayer()->SetMaxEXP(packet->MAX_EXP);
			Field->GetPlayer()->SetATK(packet->ATK);
			Field->GetPlayer()->SetDEF(packet->DEF);
			Field->GetPlayer()->SetMAG(packet->MAG);
			Field->GetPlayer()->SetMR(packet->MR);
			Field->GetPlayer()->SetAP(packet->Ability_Point);
			Field->GetPlayer()->SetSkillPoint(packet->Skill_Point);
		}
		break;
	}
	case SC_LOGIN_FAIL:
	{
		consolePrint("LOGIN FAILED!\n");
		break;
	}
	}
}

void TitleScene::Update(float fTimeElapsed)
{
}

void TitleScene::Render(HDC hdc)
{
	if (m_pFramework)
		if (m_pFramework->GetResource()) {
			m_pFramework->GetResource()->GetTitleImage().Draw(hdc, 0, 0, CLIENT_WIDTH, CLIENT_HEIGHT);
			m_pFramework->GetResource()->GetTitleTextImage().Draw(hdc, CLIENT_WIDTH / 8, 10, 3 * CLIENT_WIDTH / 4, CLIENT_HEIGHT / 2);
		}
}

void TitleScene::SendLoginPacket()
{
	cs_packet_login *myPacket = reinterpret_cast<cs_packet_login *>(m_pFramework->GetSendBuf());
	myPacket->size = sizeof(cs_packet_login);
	myPacket->type = CS_LOGIN;

	consolePrint("10자 이내의 ID를 입력해 주십시오. >> ");
	wchar_t id[10] = L"";
	wscanf_s(L"%s", id, _countof(id));
	FirstFieldScene* Field = reinterpret_cast<FirstFieldScene*>(m_pFramework->GetSceneArray(BaseScene::SceneTag::Field));
	Field->GetPlayer()->SetUserID(id);
	wcsncpy_s(myPacket->player_id, id, _countof(myPacket->player_id));
	if (m_pFramework)
		m_pFramework->SendPacket(reinterpret_cast<char *>(myPacket));
}
