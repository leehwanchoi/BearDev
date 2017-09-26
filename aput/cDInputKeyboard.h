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
//stdafx.h�� �߰� �׸�

#include "cDInputKeyboard.h"
extern cDInputKeyboard* g_DI_Keyboard;

//Dx3D.cpp�� �߰��׸�
cDInputKeyboard* g_DI_Keyboard; // ��������

g_DI_Keyboard = new cDInputKeyboard; //Init
g_DI_Keyboard->CreateDevice(g_hWnd);

g_DI_Keyboard->ReadData(); //Update

g_DI_Keyboard->FreeDirectInput(); // Release

if (g_DI_Keyboard)
g_DI_Keyboard->OnActivate(wParam); //WndProc�� �߰�

���ϴ� ����
if (g_DI_Keyboard->KeyDown(DIXXXXXXX))�� ���
*/