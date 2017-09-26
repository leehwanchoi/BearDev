#include "stdafx.h"
#include "cDInputKeyboard.h"


cDInputKeyboard::cDInputKeyboard()
{
}


cDInputKeyboard::~cDInputKeyboard()
{
}

HRESULT cDInputKeyboard::CreateDevice(HWND hWnd)
{
	HRESULT hr;

	m_hWnd = hWnd;
	if (FAILED(hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&m_pDI, NULL)))
		return hr;

	if (FAILED(hr = m_pDI->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, NULL)))
		return hr;

	if (FAILED(hr = m_pKeyboard->SetDataFormat(&c_dfDIKeyboard)))
		return hr;

	hr = m_pKeyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);

	if (FAILED(hr))
		return hr;

	m_pKeyboard->Acquire();

	return S_OK;
}

VOID cDInputKeyboard::FreeDirectInput()
{
	if (m_pKeyboard)
		m_pKeyboard->Unacquire();

	SAFE_RELEASE(m_pKeyboard);
	SAFE_RELEASE(m_pDI);
}

void cDInputKeyboard::OnActivate(WPARAM wParam)
{
	if (WA_INACTIVE != wParam && m_pKeyboard)
	{
		m_pKeyboard->Acquire();
	}
}

HRESULT cDInputKeyboard::ReadData()
{
	HRESULT hr;

	if (NULL == m_pKeyboard)
		return S_OK;

	ZeroMemory(m_Keys, sizeof(m_Keys));
	hr = m_pKeyboard->GetDeviceState(sizeof(m_Keys), m_Keys);
	if (FAILED(hr))
	{
		hr = m_pKeyboard->Acquire();
		while (hr == DIERR_INPUTLOST)
			hr = m_pKeyboard->Acquire();

		return S_OK;
	}

	return S_OK;
}

BOOL cDInputKeyboard::KeyDown(BYTE Key)
{
	if (m_Keys[Key] & 0x80)
		return TRUE;

	return FALSE;
}
