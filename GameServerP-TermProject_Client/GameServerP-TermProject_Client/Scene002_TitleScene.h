#pragma once
#include "Scene000_BaseScene.h"
class TitleScene : public BaseScene
{
public:
	TitleScene(SceneTag tag, Framework* pFramework) : BaseScene(tag, pFramework) {}
	virtual ~TitleScene();

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

	void SendLoginPacket();
};

