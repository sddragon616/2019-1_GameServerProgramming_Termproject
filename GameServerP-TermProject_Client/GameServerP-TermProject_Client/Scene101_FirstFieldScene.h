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

	// �� ���� �� ȣ��
	virtual void BuildObjects() override;

	// �÷��̾�κ��� �Է� ó��
	virtual bool ProcessInput(UCHAR *keybuffer, float fTimeElapsed = 0.0f) override;
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;

	// �� �������� ������ ��Ŷ ó��
	virtual void ProcessPacket(char *ptr) override;

	// �⺻ ���� ����
	virtual void Update(float fTimeElapsed) override;
	virtual void Render(HDC hdc) override;

	void SendMoveDirection();

	MyPlayer* GetPlayer() const {return m_pPlayer; }
};

