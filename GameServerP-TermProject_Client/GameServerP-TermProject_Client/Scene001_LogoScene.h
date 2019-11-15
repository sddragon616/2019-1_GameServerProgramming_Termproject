#pragma once
#include "Scene000_BaseScene.h"

class LogoScene : public BaseScene
{
private:	
	float m_CurrentTime = 0.0f;
public:
	LogoScene(SceneTag tag, Framework* pFramework) : BaseScene(tag, pFramework) {}
	virtual ~LogoScene();

	// �� ���� �� ȣ��
	virtual void BuildObjects() ;

	// �÷��̾�κ��� �Է� ó��
	virtual bool ProcessInput(UCHAR *keybuffer, float fTimeElapsed = 0.0f) override;
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;

	// �� �������� ������ ��Ŷ ó��
	virtual void ProcessPacket(char *ptr) override;

	// �⺻ ���� ����
	virtual void Update(float fTimeElapsed) override;
	virtual void Render(HDC hdc) override;
};