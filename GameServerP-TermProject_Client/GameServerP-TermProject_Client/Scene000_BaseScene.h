#pragma once
// 모든 씬들의 베이스가 되는 추상 클래스

class Framework;

class BaseScene
{
public: 
	// BaseScene::SceneTag:: 형식으로 호출해서 외울 필요도 define할 필요도 없이 안의 내용물을 사용할 수 있다. 이 헤더파일만 include 하자.
	enum SceneTag {
		Logo,
		Title,
		CharacterSelect,
		Field,
		Count	// Scene의 총 갯수를 정의
	};
protected:
	SceneTag m_Tag;
	Framework* m_pFramework = nullptr;	// 인자로 받아오는게 아니라 동적할당을 해온다.

public:
	BaseScene(SceneTag tag, Framework* pFramework) : m_Tag(tag), m_pFramework(pFramework) { }
	virtual ~BaseScene() { }	
	
	/*	BaseScene을 상속받은 타이틀씬의 소멸자도 ~BaseScene();이 된다. 그럼 타이틀씬이 소멸할 때 이 소멸자도 받아온다.
		이미 소멸자가 있는데 타이틀씬이 상위계층을 신경써서 지울 필요가 없다. 따라서, virtual을 붙여 상속받아 소멸시킨다.
		virtual 타고 상위 클래스의 끝까지 올라간 다음, 하위 소멸자를 전부 호출해서 상속받은 클래스를 한번에 소멸시키는 것이다.*/

	// 람다함수는 스태틱취급이라 쓰레드 인자 안으로 들어갈 수 있다. 람다함수 내부에 가상함수를 넣으면 된다.

	// 씬 입장 시 호출
	virtual void BuildObjects() = 0;

	// 플레이어로부터 입력 처리
	virtual bool ProcessInput(UCHAR *keybuffer, float fTimeElapsed = 0.0f) = 0;
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) = 0;
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) = 0;

	// 이 씬에서의 서버와 패킷 처리
	virtual void ProcessPacket(char *ptr) = 0;

	// 기본 게임 로직
	virtual void Update(float fTimeElapsed) = 0;
	virtual void Render(HDC hdc) = 0;


	const SceneTag GetTag() const { return m_Tag; }
};