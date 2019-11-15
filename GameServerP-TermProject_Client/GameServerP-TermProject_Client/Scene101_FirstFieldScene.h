#pragma once
#include "Scene000_BaseScene.h"

class MyPlayer;
class BasePlayer;
class AttackObject;


class FirstFieldScene :
	public BaseScene
{
private:
	MyPlayer *m_pPlayer = nullptr;
	BasePlayer *m_ppOtherPlayers[MAX_USER] = { nullptr, };
	AttackObject *m_ppMonsterObject[NUM_NPC + NUM_SMART_NPC] = { nullptr, };

	wchar_t	chatting[10][MAX_STR_LEN] = { L"" };

	float send_time = 0.0f;

public:
	FirstFieldScene(SceneTag tag, Framework* pFramework) : BaseScene(tag, pFramework) { BuildObjects(); }
	virtual ~FirstFieldScene();

	// 씬 입장 시 호출
	virtual void BuildObjects() override;

	// 플레이어로부터 입력 처리
	virtual bool ProcessInput(UCHAR *keybuffer, float fTimeElapsed = 0.0f) override;
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;

	// 이 씬에서의 서버와 패킷 처리
	virtual void ProcessPacket(char *ptr) override;

	// 기본 게임 로직
	virtual void Update(float fTimeElapsed) override;
	virtual void Render(HDC hdc) override;

	void SendMoveDirection();

	MyPlayer* GetPlayer() const {return m_pPlayer; }
};

