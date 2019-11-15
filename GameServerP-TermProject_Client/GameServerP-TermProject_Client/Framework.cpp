#include "stdafx.h"
// 
// Ÿ��Ʋ �� ���� include�� ����
#include "Framework.h"
#include "Scene001_LogoScene.h"
#include "Scene002_TitleScene.h"
#include "Scene003_CharacterSelectScene.h"
#include "Scene101_FirstFieldScene.h"

template<typename T>
T GetUserDataPtr(HWND hWnd)
{
	return reinterpret_cast<T>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
}
void SetUserDataPtr(HWND hWnd, LPVOID ptr)
{
	LONG_PTR result = :: SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(ptr));
}

Framework::Framework()
{
}

Framework::~Framework()
{
	OnDestroy();
}

bool Framework::OnCreate(HINSTANCE hInstance, HWND hWnd, const RECT & rc)
{
	// srand((unsigned int)time(NULL));
	m_hInstance = hInstance;
	m_hWnd = hWnd;
	m_rcClient = rc;
	// Ŭ���̾�Ʈ ��ǥ �ʱ�ȭ
	m_rcClient.right -= m_rcClient.left;
	m_rcClient.bottom -= m_rcClient.top;
	m_rcClient.left = 0;
	m_rcClient.top = 0;

	// ��Ʈ��ũ �ʱ�ȭ, �ݵ�� �ڵ��� �޾ƿ��� ���� AsyncSelect �ؾ���.
	NetworkInitialize();

	// ���� ����
	CreatebackBuffer();

	// Ŭ������ ������ ���ν��� ����
	::SetUserDataPtr(m_hWnd, this);

	// ...

	// ĸ�� ����
	lstrcpy(m_CaptionTitle, TITLESTRING);
#if defined(SHOW_CAPTIONFPS)
	lstrcat(m_CaptionTitle, TEXT("("));
#endif
	m_TitleLength = lstrlen(m_CaptionTitle);
	SetWindowText(m_hWnd, m_CaptionTitle);

	// Ÿ�̸� �ʱ�ȭ
	m_current_time = std::chrono::system_clock::now();
	m_fps = 0;
	
	// �� ����
	BuildScene();
	// ������ ���� �����ΰ�?
	ChangeScene(BaseScene::SceneTag::Logo);
	// m_pCurrScene->SetPlayersInfo();


	return (m_hWnd != NULL);
}

void Framework::CreatebackBuffer()
{
	if (m_hDC) {
		::SelectObject(m_hDC, NULL);
		::DeleteDC(m_hDC);
	}
	if (m_hBitmapBackBuffer) ::DeleteObject(m_hBitmapBackBuffer);

	HDC hdc = ::GetDC(m_hWnd);
	m_hDC = ::CreateCompatibleDC(hdc);	// ȣȯ�� �����ִ� �Լ�
	// ������ ��ũ ���ο� ����۸� �����. hdc�� ȣȯ�� ��Ʈ���� ����Ѵ�.
	m_hBitmapBackBuffer = ::CreateCompatibleBitmap(hdc, m_rcClient.right, m_rcClient.bottom);
	::SelectObject(m_hDC, m_hBitmapBackBuffer);

	SetBKColor(RGB(255, 255, 255));

	ClearBackgroundColor();

	::ReleaseDC(m_hWnd, hdc);
}

void Framework::BuildScene()
{
	// arrScene[SceneTag::Title] = new TitleScene();	// �̷� ������� ���� ������.
	arrScene[BaseScene::SceneTag::Logo] = new LogoScene(BaseScene::SceneTag::Logo, this);
	arrScene[BaseScene::SceneTag::Title] = new TitleScene(BaseScene::SceneTag::Title, this);
	arrScene[BaseScene::SceneTag::CharacterSelect] = new CharacterSelectScene(BaseScene::SceneTag::CharacterSelect, this);
	arrScene[BaseScene::SceneTag::Field] = new FirstFieldScene(BaseScene::SceneTag::Field, this);
}

void Framework::BuildScene(BaseScene::SceneTag tag)
{
	if (!arrScene[tag]) {
		switch (tag) {
		case BaseScene::SceneTag::Logo:
			arrScene[tag] = new LogoScene(tag, this);
			break;
		case BaseScene::SceneTag::Title:
			arrScene[tag] = new TitleScene(tag, this);
			break;
		case BaseScene::SceneTag::CharacterSelect:
			arrScene[tag] = new CharacterSelectScene(tag, this);
			break;
		case BaseScene::SceneTag::Field:
			arrScene[tag] = new FirstFieldScene(tag, this);
			break;
		default: 
			break;
		}
	}
}

void Framework::ReleaseScene()
{
}

bool Framework::OnDestroy()
{
	// closesocket()
	closesocket(mySocket);
	// ���� ����
	WSACleanup();

	// HDC�� ���� ���۰� ������ ����� �Ѵ�.
	if (m_hbrBackground) ::DeleteObject(m_hbrBackground);

	::SelectObject(m_hDC, NULL);
	if (m_hBitmapBackBuffer)  ::DeleteObject(m_hBitmapBackBuffer);
	if (m_hDC) ::DeleteDC(m_hDC);

	return true;
}

bool Framework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		return m_pCurrScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);	// ESC Ű�� ������ �� �����츦 �����Ѵ�.
			break;
		}
		return m_pCurrScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
	}
	return false;
}

bool Framework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (m_pCurrScene) m_pCurrScene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
	switch (nMessageID)
	{
	case WM_LBUTTONUP:
		break;
	case WM_LBUTTONDOWN:
		break;

	case WM_MOUSEMOVE:
		break;

	}
	return false;
}

HRESULT Framework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_SIZE:	// ������ ũ�⸦ �����ϴ� ���� ���� ���´�.
		break;
	default:
		return static_cast<HRESULT>(::DefWindowProc(hWnd, nMessageID, wParam, lParam));
	}
//	return E_NOTIMPL;
	return 0;
}

void Framework::Update(float fTimeElapsed)
{
	static UCHAR pKeysBuffer[256];
	//bool bProcessedByScene = false;
	// �÷��̾ �������� �ʴ� ȭ�鿡���� �ش� ���� ProcessInput �Լ��� ����
	if (GetKeyboardState(pKeysBuffer) && m_pCurrScene) /*bProcessedByScene =*/ m_pCurrScene->ProcessInput(pKeysBuffer, fTimeElapsed);

	m_pCurrScene->Update(fTimeElapsed);
}

void Framework::SetBKColor(COLORREF color)
{
	m_clrBackBuffer = color;
	if (m_hbrBackground) ::DeleteObject(m_hbrBackground);
	m_hbrBackground = ::CreateSolidBrush(m_clrBackBuffer);	// �� ����� �귯���� ������.
}

void Framework::ClearBackgroundColor()
{
	::FillRect(m_hDC, &m_rcClient, m_hbrBackground);	// ���� ������� ��׶��忡 ���� Ŭ���̾�Ʈ��ŭ�� ������ ��� ���� �귯���� ������ ��ĥ�ض�.
}

void Framework::PreprocessingForDraw()
{
	ClearBackgroundColor();	// �׸��� ������ ���۵Ǹ� ������ �׶��带 �����ϰ� �����.
	::SetBkColor(m_hDC, TRANSPARENT);	// ���ĺ��� ��� �����ϰ� ����
	::SetStretchBltMode(m_hDC, COLORONCOLOR);	// ���� ������ �޶� �þ�ų� �پ�� ������ �ִ� ��� �����.

	m_pCurrScene->Render(m_hDC);
}

void Framework::OnDraw(HDC hDC)
{
	// ���ڷ� �޴� hDC�� �ۿ��� ������� hDC�̴�. �� ��Ʈ�ʿ� m_hDC�� ����ۿ��� ������� �׸��� �Ѹ���.
	::BitBlt(hDC, m_rcClient.left, m_rcClient.top, m_rcClient.right, m_rcClient.bottom, m_hDC, 0, 0, SRCCOPY);
}

void Framework::FrameAdvance()
{
	// Get tick
	m_timeElapsed = std::chrono::system_clock::now() - m_current_time;

	if (m_timeElapsed.count() > MAX_FPS_TIME || MAX_FPS_TIME == 0.0 || MAX_FPS == 0.0) {
		Update(static_cast<float>(m_timeElapsed.count()));
		PreprocessingForDraw(); // ����� �����̹Ƿ� OnDraw�� �ƴϴ�. OnDraw ������ ����ۿ� �׷��ִ� ������ �Ѵ�.
		InvalidateRect(m_hWnd, &m_rcClient, FALSE);	// False�� �ʱ�ȭ�� ���� �ʴ´ٴ� ���̴�. ������ ������ �޽����� ȣ���Ѵ�.
		PAINTSTRUCT ps;
		HDC hdc = ::BeginPaint(m_hWnd, &ps);
		OnDraw(hdc);
		::EndPaint(m_hWnd, &ps);
		m_current_time = std::chrono::system_clock::now();
		if (m_timeElapsed.count() > 0.0)
			m_fps = 1.0 / m_timeElapsed.count();
	}
	InvalidateRect(m_hWnd, &m_rcClient, FALSE);	// False�� �ʱ�ȭ�� ���� �ʴ´ٴ� ���̴�. ������ ������ �޽����� ȣ���Ѵ�.

	// ĸ�ǿ� ���ڸ� �� ������ �����ϴ� ĸ�� ��Ʈ�� ����
#ifdef UNICODE
	_itow_s(
		static_cast<int>(m_fps + 0.5f)
		, m_CaptionTitle + m_TitleLength
		, TITLE_MX_LENGTH - m_TitleLength
		, 10);
	wcscat_s(
		m_CaptionTitle + m_TitleLength
		, TITLE_MX_LENGTH - m_TitleLength
		, TEXT("FPS )"));	
#else
	_itoa_s(
		m_fps + 0.1f
		, m_CaptionTitle + m_TitleLength
		, TITLE_MX_LENGTH - m_TitleLength
		, 10);
	strcat_s(
		m_CaptionTitle + m_TitleLength
		, TITLE_MX_LENGTH - m_TitleLength
		, TEXT("FPS )"));
#endif
	SetWindowText(m_hWnd, m_CaptionTitle);
}

LRESULT Framework::WndProc(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	Framework* self = ::GetUserDataPtr<Framework*>(hWnd);	// static �Լ��� �����Ϸ��� ������ ������. Ŭ������ �����Լ��� ����ϱ� ���� �ļ�
	if (!self)
		return ::DefWindowProc(hWnd, nMessageID, wParam, lParam);	// �޽��� ó���� OS���� �ѱ��.
	
	switch (nMessageID)
	{

	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:

	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:

	case WM_MOUSEMOVE:
		self->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;

	case WM_KEYDOWN:
	case WM_KEYUP:
		self->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = ::BeginPaint(hWnd, &ps);
		::EndPaint(hWnd, &ps);
	}
		break;

	case WM_SOCKET:
		if (WSAGETSELECTERROR(lParam)) {
			closesocket((SOCKET)wParam);
			exit(-1);
			break;
		}
		switch (WSAGETSELECTEVENT(lParam)) 
		{
		case FD_READ:
			self->ReadPacket((SOCKET)wParam);
			break;
		case FD_CLOSE:
			closesocket((SOCKET)wParam);
			exit(-1);
			break;
		}
		break;
	case WM_DESTROY:
		::SetUserDataPtr(hWnd, NULL);
		::PostQuitMessage(0);
		break;
	default:
		return self->OnProcessingWindowMessage(hWnd, nMessageID, wParam, lParam);
	}
	return 0;
//	return LRESULT();
}

void Framework::ChangeScene(BaseScene::SceneTag tag, bool bDestroy)
{
	if (bDestroy) {
		delete m_pCurrScene;
		m_pCurrScene = nullptr;
	}
	if (!arrScene[tag])
		BuildScene(tag);
	m_pCurrScene = arrScene[tag];
}

void Framework::NetworkInitialize()
{
	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
#ifdef USE_CONSOLE_WINDOW
		::puts("Error - Can't load 'winsock.dll' file");
#endif
		assert(!"Error : \"winsock.dll\" ������ �� �� �����ϴ�!");
		// ::PostQuitMessage(0);
	}

	// socket()
	// client_sock = socket(AF_INET, SOCK_STREAM, 0);
	mySocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);//WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (mySocket == INVALID_SOCKET) {
#ifdef USE_CONSOLE_WINDOW
		consolePrint("Error - Invalid socket\n");
#endif
		assert(!"Error : �߸��� ������ �����Ͽ����ϴ�!");
		// ::PostQuitMessage(0);
	}
#ifdef USE_CONSOLE_WINDOW
	consolePrint("Client Socket : %d\n", static_cast<int>(mySocket));
#endif


	send_wsabuf.buf = send_buffer;
	send_wsabuf.len = BUFSIZE;
	recv_wsabuf.buf = recv_buffer;
	recv_wsabuf.len = BUFSIZE;

	// ServerConnect();
}

void Framework::ServerConnect()
{
#ifdef USE_CONSOLE_WINDOW
	consolePrint("������ ���� ���� Server IP(IPv4)�� �Է��ϼ���.\n������ ������� 127.0.0.1�� �Է��Ͻø� �˴ϴ�.\nServer IP(IPv4) >> ");
	scanf_s("%s", server_ip, unsigned int(sizeof(server_ip)));
#else
	strcpy(server_ip, "127.0.0.1");
#endif

	// �������� ��ü����	
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	// serveraddr.sin_addr.s_addr = inet_addr(server_ip);
	serveraddr.sin_addr.S_un.S_addr = inet_addr(server_ip);
	serveraddr.sin_port = htons(SERVER_PORT);

	// connect()
	// if (connect(client_sock, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == SOCKET_ERROR) {
	if (WSAConnect(mySocket, (struct sockaddr *)&serveraddr, sizeof(serveraddr), NULL, NULL, NULL, NULL) == SOCKET_ERROR) {
		consolePrint("%s IP Error - Fail to connect\n", server_ip);
		closesocket(mySocket);
		WSACleanup();
		assert(!"�Էµ� IP���� Ŭ���̾�Ʈ�� �´� ������ ����Ǿ� ���� �ʽ��ϴ�.");
		// ::PostQuitMessage(0);
	}
	else {
		WSAAsyncSelect(mySocket, m_hWnd, WM_SOCKET, FD_CLOSE | FD_READ); // m_hWnd �ʱ�ȭ�� ���ϰ�, Ŀ��Ʈ�� ���ϰ�, �̰� �θ��� �־����� �翬�� �ȵ���...
		consolePrint("Server Connected\n");
	}
}

void Framework::ReadPacket(SOCKET sock)
{
	DWORD iobyte, ioflag = 0;

	int ret = WSARecv(sock, &recv_wsabuf, 1, &iobyte, &ioflag, NULL, NULL);
	if (ret) {
		int err_code = WSAGetLastError();
#ifdef USE_CONSOLE_WINDOW
		consolePrint("Recv Error [%d]\n", err_code);
#endif
	}

	BYTE *ptr = reinterpret_cast<BYTE *>(recv_buffer);

	while (0 != iobyte) {
		if (0 == in_packet_size) in_packet_size = ptr[0];
		if (iobyte + saved_packet_size >= in_packet_size) {
			memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
			m_pCurrScene->ProcessPacket(packet_buffer);
			ptr += in_packet_size - saved_packet_size;
			iobyte -= in_packet_size - saved_packet_size;
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		else {
			memcpy(packet_buffer + saved_packet_size, ptr, iobyte);
			saved_packet_size += iobyte;
			iobyte = 0;
		}
	}
}

void Framework::SendPacket(char * clientToServerPacket)
{
	send_wsabuf.buf = send_buffer;
	send_wsabuf.len = clientToServerPacket[0];	// size
	DWORD ioByte = 0;

	int nByteCheck = WSASend(mySocket, &send_wsabuf, 1, &ioByte, 0, NULL, NULL);
#ifdef USE_CONSOLE_WINDOW
	if (nByteCheck) consolePrint("Error while sending packet [%d]", WSAGetLastError());
#endif
}
