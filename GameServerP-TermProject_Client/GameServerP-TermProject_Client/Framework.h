#pragma once
// 헤더파일에는 이 프레임워크에서 사용하는 인터페이스나 이 클래스의 부모 외에는 include하지 않도록 한다.
#include "ResourceManager.h"
#include "Scene000_BaseScene.h"	// 인터페이스이므로 header에 include 한다.
					

template<typename Enum> // enum class의 선언형을 알려주며 인자와 대응하는 값을 반환하는 함수
inline constexpr auto GetFnumValueByType(Enum enumerator) noexcept	// enum class E : int {a, b, c}; 일 때,
{																	// auto data = GetEnumValueByType(E::a);
	return static_cast<std::underlying_type_t<enum>>(enummerator);	// data의 형식은 int이고, 값은 0
}
// enum class는 enum을 class로 만든 것이다. 이렇게하면 enum이 자료형이 되어버리므로 같은 enum class밖에 못넣는다.

// #define GetSceneEnumInt(Enum) GetEnumValueType(CScene::CurrentScene::Enum)	// 아직 CScene을 선언하지 않았으므로 주석처리

class Framework final
{
private:
	HINSTANCE	m_hInstance{ NULL };

	HWND		m_hWnd{ NULL };
	RECT		m_rcClient{ 0, 0, 0, 0 };	// 클라이언트 크기

	HBITMAP		m_hBitmapBackBuffer = NULL;	// 비트맵 핸들. 윈도우 API가 제공하는 버퍼다. 윈도우에 띄우는 버퍼.
	HDC			m_hDC = NULL;				// 윈도우 핸들

	COLORREF	m_clrBackBuffer = 0x00000000;	// 백버퍼 색깔. 검정으로 초기화
	HBRUSH		m_hbrBackground = NULL;			// 그리는 도구. 색칠할 떄 쓴다.

	std::chrono::system_clock::time_point m_current_time;
	std::chrono::duration<double> m_timeElapsed;	// 시간이 얼마나 지났는가?
	double m_fps;

	TCHAR m_CaptionTitle[TITLE_MX_LENGTH];	// SetWindow 함수에다가 이 타이틀을 집어넣는다.
	unsigned char m_TitleLength;

	ResourceManager resourceManager;

	unsigned int myid;
public:
	Framework();
	~Framework();

	bool OnCreate(HINSTANCE hInstance, HWND hWnd, const RECT &rc); // rc는 윈도우크기이고, m_rcClient에 저장한다.
	void CreatebackBuffer();	// HBITMAP을 만든다.
	void BuildScene();
	void BuildScene(BaseScene::SceneTag tag);
	void ReleaseScene();	

	bool OnDestroy();

	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	HRESULT OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void Update(float fTimeElapsed);
	
	void SetBKColor(COLORREF color);	// COLORREF 멤버변수를 바꿔주는 함수
	void ClearBackgroundColor();		// 백버퍼 초기화
	void PreprocessingForDraw();		// 설정된 백버퍼에 실질적으로 그리는것
	void OnDraw(HDC hDC);				// Paint에서 Bitblt 단 하나만 한다.

	void FrameAdvance();	
	/*윈도우 메시지 아래에 불리는 함수이다. 모든 로직이 이 안에서 처리되기 떄문이다.
	Update와 PreProceessingForDraw를 처리한다. InvalidateRect도 하고 타이틀 텍스트 캡션도 한다.*/

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void ChangeScene(BaseScene::SceneTag tag, bool bDestroy = false);
	// void ChangeScene(CScene::CurrentScene tag, bool bDestroy = false);	// 마찬가지로 CScene을 모르므로 주석처리
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

	char server_ip[17] = "127.0.0.1"; // 서버 IP
	SOCKET mySocket; // 소켓

	// 전역변수 대용으로 사용
	WSABUF  send_wsabuf;
	char	send_buffer[BUFSIZE];
	WSABUF	recv_wsabuf;
	char	recv_buffer[BUFSIZE];
	char	packet_buffer[BUFSIZE];
	DWORD	in_packet_size = 0;
	int		saved_packet_size = 0;
};