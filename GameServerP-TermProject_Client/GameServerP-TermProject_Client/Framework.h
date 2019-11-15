#pragma once
// ������Ͽ��� �� �����ӿ�ũ���� ����ϴ� �������̽��� �� Ŭ������ �θ� �ܿ��� include���� �ʵ��� �Ѵ�.
#include "ResourceManager.h"
#include "Scene000_BaseScene.h"	// �������̽��̹Ƿ� header�� include �Ѵ�.
					

template<typename Enum> // enum class�� �������� �˷��ָ� ���ڿ� �����ϴ� ���� ��ȯ�ϴ� �Լ�
inline constexpr auto GetFnumValueByType(Enum enumerator) noexcept	// enum class E : int {a, b, c}; �� ��,
{																	// auto data = GetEnumValueByType(E::a);
	return static_cast<std::underlying_type_t<enum>>(enummerator);	// data�� ������ int�̰�, ���� 0
}
// enum class�� enum�� class�� ���� ���̴�. �̷����ϸ� enum�� �ڷ����� �Ǿ�����Ƿ� ���� enum class�ۿ� ���ִ´�.

// #define GetSceneEnumInt(Enum) GetEnumValueType(CScene::CurrentScene::Enum)	// ���� CScene�� �������� �ʾ����Ƿ� �ּ�ó��

class Framework final
{
private:
	HINSTANCE	m_hInstance{ NULL };

	HWND		m_hWnd{ NULL };
	RECT		m_rcClient{ 0, 0, 0, 0 };	// Ŭ���̾�Ʈ ũ��

	HBITMAP		m_hBitmapBackBuffer = NULL;	// ��Ʈ�� �ڵ�. ������ API�� �����ϴ� ���۴�. �����쿡 ���� ����.
	HDC			m_hDC = NULL;				// ������ �ڵ�

	COLORREF	m_clrBackBuffer = 0x00000000;	// ����� ����. �������� �ʱ�ȭ
	HBRUSH		m_hbrBackground = NULL;			// �׸��� ����. ��ĥ�� �� ����.

	std::chrono::system_clock::time_point m_current_time;
	std::chrono::duration<double> m_timeElapsed;	// �ð��� �󸶳� �����°�?
	double m_fps;

	TCHAR m_CaptionTitle[TITLE_MX_LENGTH];	// SetWindow �Լ����ٰ� �� Ÿ��Ʋ�� ����ִ´�.
	unsigned char m_TitleLength;

	ResourceManager resourceManager;

	unsigned int myid;
public:
	Framework();
	~Framework();

	bool OnCreate(HINSTANCE hInstance, HWND hWnd, const RECT &rc); // rc�� ������ũ���̰�, m_rcClient�� �����Ѵ�.
	void CreatebackBuffer();	// HBITMAP�� �����.
	void BuildScene();
	void BuildScene(BaseScene::SceneTag tag);
	void ReleaseScene();	

	bool OnDestroy();

	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	HRESULT OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void Update(float fTimeElapsed);
	
	void SetBKColor(COLORREF color);	// COLORREF ��������� �ٲ��ִ� �Լ�
	void ClearBackgroundColor();		// ����� �ʱ�ȭ
	void PreprocessingForDraw();		// ������ ����ۿ� ���������� �׸��°�
	void OnDraw(HDC hDC);				// Paint���� Bitblt �� �ϳ��� �Ѵ�.

	void FrameAdvance();	
	/*������ �޽��� �Ʒ��� �Ҹ��� �Լ��̴�. ��� ������ �� �ȿ��� ó���Ǳ� �����̴�.
	Update�� PreProceessingForDraw�� ó���Ѵ�. InvalidateRect�� �ϰ� Ÿ��Ʋ �ؽ�Ʈ ĸ�ǵ� �Ѵ�.*/

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void ChangeScene(BaseScene::SceneTag tag, bool bDestroy = false);
	// void ChangeScene(CScene::CurrentScene tag, bool bDestroy = false);	// ���������� CScene�� �𸣹Ƿ� �ּ�ó��
	const BaseScene::SceneTag GetNowSceneTag() { return m_pCurrScene->GetTag(); }
	BaseScene * GetSceneArray(unsigned int index) const { return arrScene[index]; }
	BaseScene * GetNowScene() const { return m_pCurrScene; }

	void NetworkInitialize();
	void ServerConnect();
	void ReadPacket(SOCKET sock);
	void SendPacket(char * clientToServerPacket);
	char* GetSendBuf() { return send_buffer; }

	const ResourceManager* GetResource() const { return &resourceManager; }

	void SetMyID(unsigned int id) { myid = id; }
	unsigned int GetMyid() const { return myid; }
	
private:
	BaseScene * arrScene[BaseScene::SceneTag::Count];
	BaseScene * m_pCurrScene;

	char server_ip[17] = "127.0.0.1"; // ���� IP
	SOCKET mySocket; // ����

	// �������� ������� ���
	WSABUF  send_wsabuf;
	char	send_buffer[BUFSIZE];
	WSABUF	recv_wsabuf;
	char	recv_buffer[BUFSIZE];
	char	packet_buffer[BUFSIZE];
	DWORD	in_packet_size = 0;
	int		saved_packet_size = 0;
};