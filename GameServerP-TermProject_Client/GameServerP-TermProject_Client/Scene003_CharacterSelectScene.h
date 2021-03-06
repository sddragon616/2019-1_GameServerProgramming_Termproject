#pragma once
#include "Scene000_BaseScene.h"

class CharacterSelectScene : public BaseScene
{
public:
	CharacterSelectScene(SceneTag tag, Framework* pFramework) : BaseScene(tag, pFramework) {}
	virtual ~CharacterSelectScene();

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
};

