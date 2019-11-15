#pragma once
// ��� ������ ���̽��� �Ǵ� �߻� Ŭ����

class Framework;

class BaseScene
{
public: 
	// BaseScene::SceneTag:: �������� ȣ���ؼ� �ܿ� �ʿ䵵 define�� �ʿ䵵 ���� ���� ���빰�� ����� �� �ִ�. �� ������ϸ� include ����.
	enum SceneTag {
		Logo,
		Title,
		CharacterSelect,
		Field,
		Count	// Scene�� �� ������ ����
	};
protected:
	SceneTag m_Tag;
	Framework* m_pFramework = nullptr;	// ���ڷ� �޾ƿ��°� �ƴ϶� �����Ҵ��� �ؿ´�.

public:
	BaseScene(SceneTag tag, Framework* pFramework) : m_Tag(tag), m_pFramework(pFramework) { }
	virtual ~BaseScene() { }	
	
	/*	BaseScene�� ��ӹ��� Ÿ��Ʋ���� �Ҹ��ڵ� ~BaseScene();�� �ȴ�. �׷� Ÿ��Ʋ���� �Ҹ��� �� �� �Ҹ��ڵ� �޾ƿ´�.
		�̹� �Ҹ��ڰ� �ִµ� Ÿ��Ʋ���� ���������� �Ű�Ἥ ���� �ʿ䰡 ����. ����, virtual�� �ٿ� ��ӹ޾� �Ҹ��Ų��.
		virtual Ÿ�� ���� Ŭ������ ������ �ö� ����, ���� �Ҹ��ڸ� ���� ȣ���ؼ� ��ӹ��� Ŭ������ �ѹ��� �Ҹ��Ű�� ���̴�.*/

	// �����Լ��� ����ƽ����̶� ������ ���� ������ �� �� �ִ�. �����Լ� ���ο� �����Լ��� ������ �ȴ�.

	// �� ���� �� ȣ��
	virtual void BuildObjects() = 0;

	// �÷��̾�κ��� �Է� ó��
	virtual bool ProcessInput(UCHAR *keybuffer, float fTimeElapsed = 0.0f) = 0;
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) = 0;
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) = 0;

	// �� �������� ������ ��Ŷ ó��
	virtual void ProcessPacket(char *ptr) = 0;

	// �⺻ ���� ����
	virtual void Update(float fTimeElapsed) = 0;
	virtual void Render(HDC hdc) = 0;


	const SceneTag GetTag() const { return m_Tag; }
};