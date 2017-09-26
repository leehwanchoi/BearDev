#include "stdafx.h"
#include "cDInputJoystick.h"


cDInputJoystick::cDInputJoystick()
{
}

cDInputJoystick::~cDInputJoystick()
{
}

HRESULT cDInputJoystick::UpdateControllerState()
{
	DWORD dwResult;
	for (DWORD i = 0; i < MAX_CONTROLLERS; i++)
	{
		// Simply get the state of the controller from XInput.
		dwResult = XInputGetState(i, &m_Controllers[i].state);

		if (dwResult == ERROR_SUCCESS)
			m_Controllers[i].bConnected = true;
		else
			m_Controllers[i].bConnected = false;
	}

	WCHAR sz[4][1024];
	for (DWORD i = 0; i < MAX_CONTROLLERS; i++)
	{
		if (m_Controllers[i].bConnected)
		{
			WORD wButtons = m_Controllers[i].state.Gamepad.wButtons;

			if (m_bDeadZoneOn)
			{
				// Zero value if thumbsticks are within the dead zone 
				if ((m_Controllers[i].state.Gamepad.sThumbLX < INPUT_DEADZONE &&
					m_Controllers[i].state.Gamepad.sThumbLX > -INPUT_DEADZONE) &&
					(m_Controllers[i].state.Gamepad.sThumbLY < INPUT_DEADZONE &&
					m_Controllers[i].state.Gamepad.sThumbLY > -INPUT_DEADZONE))
				{
					m_Controllers[i].state.Gamepad.sThumbLX = 0;
					m_Controllers[i].state.Gamepad.sThumbLY = 0;
				}

				if ((m_Controllers[i].state.Gamepad.sThumbRX < INPUT_DEADZONE &&
					m_Controllers[i].state.Gamepad.sThumbRX > -INPUT_DEADZONE) &&
					(m_Controllers[i].state.Gamepad.sThumbRY < INPUT_DEADZONE &&
					m_Controllers[i].state.Gamepad.sThumbRY > -INPUT_DEADZONE))
				{
					m_Controllers[i].state.Gamepad.sThumbRX = 0;
					m_Controllers[i].state.Gamepad.sThumbRY = 0;
				}
			}
		}
	}

	return S_OK;
}

void cDInputJoystick::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_ACTIVATEAPP:
	{
						   if (wParam == TRUE)
						   {
							   // App is now active, so re-enable XInput
							   XInputEnable(true);
						   }
						   else
						   {
							   // App is now inactive, so disable XInput to prevent
							   // user input from effecting application and to 
							   // disable rumble. 
							   XInputEnable(false);
						   }
						   break;
	}

	case WM_KEYDOWN:
	{
					   if (wParam == 'D') m_bDeadZoneOn = !m_bDeadZoneOn;
					   break;
	}
	}
}

bool cDInputJoystick::GetButtonPressed(int a_iButton)
{
	if (g_DI_Joystick->m_Controllers->state.Gamepad.wButtons & a_iButton)
		return true;

	return false;
}

bool cDInputJoystick::GetLXLEFT()
{
	if (g_DI_Joystick->m_Controllers->state.Gamepad.sThumbLX < 0)
		return true;

	return false;
}

bool cDInputJoystick::GetLXRIGHT()
{
	if (g_DI_Joystick->m_Controllers->state.Gamepad.sThumbLX > 0)
		return true;

	return false;
}

bool cDInputJoystick::GetLYTOP()
{
	if (g_DI_Joystick->m_Controllers->state.Gamepad.sThumbLY > 0)
		return true;

	return false;
}

bool cDInputJoystick::GetLYBOTTOM()
{
	if (g_DI_Joystick->m_Controllers->state.Gamepad.sThumbLY < 0)
		return true;

	return false;
}

bool cDInputJoystick::GetLSHOULDER()
{
	if (g_DI_Joystick->m_Controllers->state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)
		return true;

	return false;
}

bool cDInputJoystick::GetLTRIGGER()
{
	if (g_DI_Joystick->m_Controllers->state.Gamepad.bLeftTrigger > 0)
		return true;

	return false;
}

bool cDInputJoystick::GetRSHOULDER()
{
	if (g_DI_Joystick->m_Controllers->state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
		return true;

	return false;
}

bool cDInputJoystick::GetRTRIGGER()
{
	if (g_DI_Joystick->m_Controllers->state.Gamepad.bRightTrigger > 0)
		return true;

	return false;
}
