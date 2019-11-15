#include "stdafx.h"
#include "Scene003_CharacterSelectScene.h"
#include "Framework.h"
#include "ResourceManager.h"


CharacterSelectScene::~CharacterSelectScene()
{
}

void CharacterSelectScene::BuildObjects()
{
}

bool CharacterSelectScene::ProcessInput(UCHAR * keybuffer, float fTimeElapsed)
{
	return false;
}

bool CharacterSelectScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool CharacterSelectScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

void CharacterSelectScene::ProcessPacket(char * ptr)
{
}

void CharacterSelectScene::Update(float fTimeElapsed)
{
}

void CharacterSelectScene::Render(HDC hdc)
{
}
