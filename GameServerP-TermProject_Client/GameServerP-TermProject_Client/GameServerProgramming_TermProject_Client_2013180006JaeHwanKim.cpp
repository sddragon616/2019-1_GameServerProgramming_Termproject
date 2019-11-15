// GameServerPTermProjectClient.cpp: 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include "GameServerProgramming_TermProject_Client_2013180006JaeHwanKim.h"
#include "Framework.h"

#define MAX_LOADSTRING 100

Framework myFramework;

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_GAMESERVERPTERMPROJECTCLIENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 응용 프로그램 초기화를 수행합니다.
    if (!InitInstance (hInstance, nCmdShow)) return FALSE;

   // HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GAMESERVERPTERMPROJECTCLIENT));

    MSG msg;

    // 기본 메시지 루프입니다.
	while (true) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			// if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
			if (msg.message == WM_QUIT) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		myFramework.FrameAdvance();
	}

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = Framework::WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GAMESERVERPTERMPROJECTCLIENT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = nullptr;//MAKEINTRESOURCEW(IDC_GAMESERVERPTERMPROJECTCLIENT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   // 윈도우 스타일
   DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_BORDER;

   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   RECT getWinSize;
   GetWindowRect(GetDesktopWindow(), &getWinSize);

   // 클라이언트 사이즈
   RECT rc;
   rc.left = rc.top = 0;
   rc.right = CLIENT_WIDTH;
   rc.bottom = CLIENT_HEIGHT;
   // 윈도우 사이즈에 실제로 추가되는 (캡션, 외곽선 등) 크기를 보정.
   AdjustWindowRect(&rc, dwStyle, FALSE);

   // 클라이언트 절대 좌표(left, top)
   // 데스크톱의 중앙에 클라이언트가 위치하도록 설정
   POINT ptClientWorld;
   ptClientWorld.x = (getWinSize.right - CLIENT_WIDTH) / 2;
   ptClientWorld.y = (getWinSize.bottom - CLIENT_HEIGHT) / 2;
   //HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, dwStyle, ptClientWorld.x, ptClientWorld.y, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr);
   if (!hWnd) return FALSE;
   if (!myFramework.OnCreate(hInstance, hWnd, rc)) return FALSE;
   // 윈도우 표시
   ShowWindow(hWnd, nCmdShow); // 윈도우 창을 보여준다. 이게 없으면 윈도우 창이 안 보이기 떄문에 활성화가 안되므로 아무것도 못한다. 종료 조차도!
							   // 확인 : WnbdProc의 default msg handler가 DefWindowProc 함수를 반환하는가?
   UpdateWindow(hWnd);
   return TRUE;
}
