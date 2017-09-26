#include "stdafx.h"
#include "cDInputMouse.h"


cDInputMouse::cDInputMouse()
{
}


cDInputMouse::~cDInputMouse()
{
}

HRESULT cDInputMouse::CreateDevice(HWND hWnd)
{
	HRESULT hr;

	m_hWnd = hWnd;
	if (FAILED(hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&m_pDI, NULL)))
		return hr;

	if (FAILED(hr = m_pDI->CreateDevice(GUID_SysMouse, &m_pMouse, NULL)))
		return hr;

	if (FAILED(hr = m_pMouse->SetDataFormat(&c_dfDIMouse2)))
		return hr;

	hr = m_pMouse->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

	if (FAILED(hr))
		return hr;

	m_pMouse->Acquire();

	return S_OK;
}

HRESULT cDInputMouse::ReadData()
{
	HRESULT hr;

	if (NULL == m_pMouse)
		return S_OK;

	ZeroMemory(&m_MouseState2, sizeof(m_MouseState2));
	hr = m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE2), &m_MouseState2);

	if (FAILED(hr))
	{
		hr = m_pMouse->Acquire();
		while (hr == DIERR_INPUTLOST)
			hr = m_pMouse->Acquire();

		return S_OK;
	}

	m_ptMouse.x = m_MouseState2.lX;
	m_ptMouse.y = m_MouseState2.lY;

	return S_OK;
}

VOID cDInputMouse::FreeDirectInput()
{
	if (m_pMouse)
		m_pMouse->Unacquire();

	SAFE_RELEASE(m_pMouse);
	SAFE_RELEASE(m_pDI);
}

void cDInputMouse::OnActivate(WPARAM wParam)
{
	if (WA_INACTIVE != wParam && m_pMouse)
	{
		m_pMouse->Acquire();
	}
}

BOOL cDInputMouse::PushLeft()
{
	return (m_MouseState2.rgbButtons[0] & 0x80) ? TRUE : FALSE;
}

BOOL cDInputMouse::PushRight()
{
	return (m_MouseState2.rgbButtons[1] & 0x80) ? TRUE : FALSE;
}

BOOL cDInputMouse::PushMiddle()
{
	return (m_MouseState2.rgbButtons[2] & 0x80) ? TRUE : FALSE;
}
