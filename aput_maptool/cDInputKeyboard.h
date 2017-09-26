#pragma once
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

class cDInputKeyboard
{
private:
	LPDIRECTINPUT8			m_pDI;
	LPDIRECTINPUTDEVICE8	m_pKeyboard;
	HWND					m_hWnd;
	BYTE					m_Keys[256];

public:
	cDInputKeyboard();
	~cDInputKeyboard();

	HRESULT CreateDevice(HWND hWnd);
	VOID FreeDirectInput();
	void OnActivate(WPARAM wParam);
	HRESULT ReadData();
	BOOL KeyDown(BYTE Key);
};

/*
//stdafx.h에 추가 항목

#include "cDInputKeyboard.h"
extern cDInputKeyboard* g_DI_Keyboard;

//Dx3D.cpp에 추가항목
cDInputKeyboard* g_DI_Keyboard; // 전역변수

g_DI_Keyboard = new cDInputKeyboard; //Init
g_DI_Keyboard->CreateDevice(g_hWnd);

g_DI_Keyboard->ReadData(); //Update

g_DI_Keyboard->FreeDirectInput(); // Release

if (g_DI_Keyboard)
g_DI_Keyboard->OnActivate(wParam); //WndProc에 추가

원하는 곳에
if (g_DI_Keyboard->KeyDown(DIXXXXXXX))로 사용
*/