#pragma once
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

class cDInputMouse
{
private:
	LPDIRECTINPUT8			m_pDI;
	LPDIRECTINPUTDEVICE8	m_pMouse;
	DIMOUSESTATE2			m_MouseState2;
	HWND					m_hWnd;

public:
	cDInputMouse();
	~cDInputMouse();

	HRESULT CreateDevice(HWND hWnd);
	HRESULT ReadData();
	VOID FreeDirectInput();
	void OnActivate(WPARAM wParam);
	POINT m_ptMouse;
	BOOL PushLeft();
	BOOL PushRight();
	BOOL PushMiddle();
};

/*
//stdafx.h에 추가 항목

#include "cDInputMouse.h"
extern cDInputMouse*	g_DI_Mouse;

//Dx3D.cpp에 추가항목
cDInputMouse* g_DI_Mouse; // 전역변수

g_DI_Mouse = new cDInputMouse; //Init
g_DI_Mouse->CreateDevice(g_hWnd);

g_DI_Mouse->ReadData(); //Update

g_DI_Mouse->FreeDirectInput(); // Release

if (g_DI_Mouse)
g_DI_Mouse->OnActivate(wParam); //WndProc에 추가

원하는 곳에
if (g_DI_Mouse->PushLeft())로 사용
if (g_DI_Mouse->PushRight())로 사용
if (g_DI_Mouse->PushMiddle())로 사용
*/