// Dx3D.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "Dx3D.h"
#include "cMainGame.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// ======================= API -> Dx 변경을 위한 추가부분
cMainGame*	g_pMainGame;
HWND		g_hWnd;
cDInputKeyboard* g_DI_Keyboard;
cDInputMouse*	 g_DI_Mouse;
cDInputJoystick* g_DI_Joystick;
// =======================

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void setWindowSize(int x, int y, int width, int height);

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
    LoadStringW(hInstance, IDC_DX3D, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 응용 프로그램 초기화를 수행합니다.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

	// ======================= API -> Dx 변경을 위한 추가부분
	g_pMainGame = new cMainGame;
	g_pMainGame->init();

	g_DI_Keyboard = new cDInputKeyboard;
	g_DI_Keyboard->CreateDevice(g_hWnd);

	g_DI_Mouse = new cDInputMouse;
	g_DI_Mouse->CreateDevice(g_hWnd);

	g_DI_Joystick = new cDInputJoystick;
	// =======================

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DX3D));

    MSG msg;

	// ======================= API -> Dx 변경을 위해
	// 기본 루프 방식을 변경 GetMessage -> PeekMessage
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				break;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			/////////추가/////////
			//이제 프로시져 메시지는 이곳에서 바로보냄
			g_pMainGame->WndProc(g_hWnd, msg.message, msg.wParam, msg.lParam);
			g_DI_Keyboard->OnActivate(msg.wParam);
			g_DI_Mouse->OnActivate(msg.wParam);
			g_DI_Joystick->WndProc(g_hWnd, msg.message, msg.wParam, msg.lParam);
			//////////////////////

			g_pTimeManager->update(120.f);
			g_pMainGame->update();
			g_pMainGame->render();
			g_pDeltaTimeManager->Update();	//추가

			g_DI_Joystick->UpdateControllerState();
			g_DI_Keyboard->ReadData();
			g_DI_Mouse->ReadData();
		}
	}
	// =======================

	// ======================= API -> Dx 변경을 위한 추가부분
	g_DI_Keyboard->FreeDirectInput();
	g_DI_Mouse->FreeDirectInput();

	g_pMainGame->Release();
	delete g_pMainGame;
	// =======================

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
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DX3D));
    wcex.hCursor        = LoadCursor(nullptr, IDC_HAND);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_DX3D);
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
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(
	   szWindowClass,    //윈도우 클래스의 이름
	   L"Ap ut",    //윈도우 타이틀바 이름
	   WS_MINIMIZE | WS_EX_TOPMOST | WS_POPUP,   //윈도우 스타일		   //WS_EX_TOPMOST | WS_POPUP	WINSTYLE
	   WINSTARTX,  //윈도우 화면좌표 x
	   WINSTARTY,  //윈도우 화면좌표 y
	   WINSIZEX,   //윈도우 화면좌표 width
	   WINSIZEY,   //윈도우 화면좌표 height
	   NULL,       //부모 윈도우
	   (HMENU)NULL,//메뉴핸들
	   hInstance,  //인스턴스 지정
	   NULL);      //윈도우 및 자식 윈도우를 생성하면
				   //지정해주되 그렇지 않으면 NULL

   SetMenu(hWnd, NULL);	//추가
   setWindowSize(WINSTARTX, WINSTARTY, WINSIZEX, WINSIZEY);

   if (!hWnd)
   {
      return FALSE;
   }

   // ======================= API -> Dx 변경을 위한 추가부분
   g_hWnd = hWnd;
   // =======================

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적:  주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//이제 이곳에서 사용 안함. (WinMain에서 바로 보냄)
	/*if (g_pMainGame)
	g_pMainGame->WndProc(hWnd, message, wParam, lParam);

	if (g_DI_Keyboard)
	g_DI_Keyboard->OnActivate(wParam);

	if (g_DI_Mouse)
	g_DI_Mouse->OnActivate(wParam);

	if (g_DI_Joystick)
	g_DI_Joystick->WndProc(hWnd, message, wParam, lParam);*/

    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다.
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
				//exit(1);	//추가
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다.
            EndPaint(hWnd, &ps);
        }
        break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			//exit(1);	//추가
			//PostQuitMessage(0);
			break;
		}
		break;
    case WM_DESTROY:
		//exit(1);	//추가
		PostQuitMessage(0);
        break;
	//커서 없애기
	//case WM_SETCURSOR:
	//	SetCursor(NULL);
	//	break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void setWindowSize(int x, int y, int width, int height)
{
	RECT rc;

	rc.left = 0;
	rc.top = 0;
	rc.right = width;
	rc.bottom = height;

	AdjustWindowRect(&rc, WINSTYLE, false);

	SetWindowPos(g_hWnd, NULL, x, y, (rc.right - rc.left),
		(rc.bottom - rc.top), SWP_NOZORDER | SWP_NOMOVE);
}
