#pragma once
#include <XInput.h> // XInput API
#pragma comment(lib, "XInput.lib")

#define MAX_CONTROLLERS 4  // XInput handles up to 4 controllers 
#define INPUT_DEADZONE  ( 0.24f * FLOAT(0x7FFF) )  // Default to 24% of the +/- 32767 range.   This is a reasonable default value but can be altered if needed.

struct CONTROLER_STATE
{
	XINPUT_STATE state;
	bool bConnected;
};

class cDInputJoystick
{
private:

public:
	cDInputJoystick();
	~cDInputJoystick();

	CONTROLER_STATE m_Controllers[MAX_CONTROLLERS];
	WCHAR			m_szMessage[4][1024] = { 0 };
	HWND			m_hWnd;
	bool			m_bDeadZoneOn = true;

	HRESULT UpdateControllerState();
	void WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	bool GetButtonPressed(int a_iButton);
};

/*
JOY_BUTTON1    = 0x1,
JOY_BUTTON2    = 0x2,
JOY_BUTTON3    = 0x4,
JOY_BUTTON4    = 0x8,
*/

/*
//stdafx.h
#include "cDInputjoystick.h"
extern cDInputJoystick* g_DI_Joystick;

//Dx3D.cpp
cDInputJoystick* g_DI_Joystick;

g_DI_Joystick = new cDInputJoystick;

g_DI_Joystick->UpdateControllerState();

if (g_DI_Joystick)
g_DI_Joystick->WndProc(hWnd, message, wParam, lParam);
*/