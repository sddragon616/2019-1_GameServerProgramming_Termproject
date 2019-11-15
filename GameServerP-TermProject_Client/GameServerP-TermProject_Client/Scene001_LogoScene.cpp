#include "stdafx.h"
#include "Scene001_LogoScene.h"
#include "Framework.h"
#include "ResourceManager.h"

//
//bool isVisible(BaseObject* a, BaseObject* b)
//{
//	if (VIEW_RANGE < abs(a->GetPosition().x - b->GetPosition().x))
//		return false;
//	if (VIEW_RANGE < abs(a->GetPosition().y - b->GetPosition().y))
//		return false;
//	return true;
//}

LogoScene::~LogoScene()
{
}

void LogoScene::BuildObjects()
{
}

void LogoScene::Render(HDC hdc)
{
	if (m_pFramework)
		if (m_pFramework->GetResource()) 
			m_pFramework->GetResource()->GetLogoImage().Draw(hdc, 0, 0, CLIENT_WIDTH, CLIENT_HEIGHT);
}

void LogoScene::Update(float fTimeElapsed)
{
	m_CurrentTime += fTimeElapsed;

	if (m_CurrentTime > 1.0f) {
		m_pFramework->ChangeScene(BaseScene::SceneTag::Title, true);
		m_CurrentTime = 0.0f;
	}
}

bool LogoScene::ProcessInput(UCHAR * keybuffer, float fTimeElapsed)
{
	return false;
}

bool LogoScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		// 키보드를 누른 시점의 최초 한번만 실행.
		if ((lParam & 0x40000000) != 0x40000000) break;
		break;
	case WM_KEYUP:	
		break;
	}
	return false;
}

bool LogoScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

void LogoScene::ProcessPacket(char *ptr)
{
	// 이 시점에서는 아직 네트워크 접속을 하지 않았다.

}

//void CMainScene::SendMoveDirection()
//{
	//if (m_pPlayer && m_pPlayer->GetDirectionBit()) {
	//	CSPacket_Move *myMovePacket = reinterpret_cast<CSPacket_Move *>(m_pFramework->GetSendBuf());
	//	myMovePacket->size = sizeof(CSPacket_Move);
	//	// 클라이언트가 어느 방향으로 갈 지 키입력 정보를 저장한 비트를 서버로 보내기
	//	myMovePacket->dir = m_pPlayer->GetDirectionBit();
	//	myMovePacket->type = CS_Type::Move;
	//	m_pFramework->SendPacket(reinterpret_cast<char *>(myMovePacket));
	//}
//}
