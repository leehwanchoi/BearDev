#include "stdafx.h"
#include "cCamera.h"


cCamera::cCamera()
	: m_fDistance(3.5)
	, m_vEye(0, 0, -m_fDistance)
	, m_vLookAt(0, 0, 0)
	, m_vUp(0, 1, 0)
	, m_fRotY(0)
	, m_fRotX(0)
	, m_isAutoRotY(false)
	, m_isTalk(false)
	, m_isShowMenu(false)
	, m_fMouseSensitivity(200.0f)
{
	m_ptPrevMouse.x = WINSIZEX / 2;
	m_ptPrevMouse.y = WINSIZEY / 2;
}


cCamera::~cCamera()
{
}

void cCamera::Setup()
{
	float l = ROTY_MIN_RANGEX;
	float t = 0;
	float r = ROTY_MAX_RANGEX;
	float b = WINSIZEY;

	D3DCOLOR c = D3DCOLOR_XRGB(255, 255, 255);
	m_vecVertex.reserve(8);

	m_vecVertex.push_back(ST_RHWC_VERTEX(D3DXVECTOR4(l, t, 0, 1), c));
	m_vecVertex.push_back(ST_RHWC_VERTEX(D3DXVECTOR4(r, t, 0, 1), c));

	m_vecVertex.push_back(ST_RHWC_VERTEX(D3DXVECTOR4(r, t, 0, 1), c));
	m_vecVertex.push_back(ST_RHWC_VERTEX(D3DXVECTOR4(r, b, 0, 1), c));

	m_vecVertex.push_back(ST_RHWC_VERTEX(D3DXVECTOR4(r, b, 0, 1), c));
	m_vecVertex.push_back(ST_RHWC_VERTEX(D3DXVECTOR4(l, b, 0, 1), c));

	m_vecVertex.push_back(ST_RHWC_VERTEX(D3DXVECTOR4(l, b, 0, 1), c));
	m_vecVertex.push_back(ST_RHWC_VERTEX(D3DXVECTOR4(l, t, 0, 1), c));
}

void cCamera::Update(D3DXVECTOR3* pTarget /*= NULL*/)
{
	RECT rc;
	GetClientRect(g_hWnd, &rc);

	//D3DVIEWPORT9 viewport;
	//g_pD3DDevice->GetViewport(&viewport);
	//viewport.Width = rc.right - rc.left;
	//viewport.Height = rc.bottom - rc.top;
	//g_pD3DDevice->SetViewport(&viewport);

	// �������� ��Ʈ���� ���� �� ����
	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, rc.right / (float)rc.bottom, 1, 1000);
	g_pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);

	// ī�޶��� ȸ���� �̵��� RT ������ �ؾߵǹǷ�
	// ������ �������� ī�޶� ��ġ�� ��� ȸ��, �̵��� ���� ��Ų��.
	m_vEye = D3DXVECTOR3(0, 0, -m_fDistance);

	/*
	//////////// ���̽�ƽ ���� ////////////////
	*/
	//UpdateControllByJoystick();
	/*
	///////////////////////////////////////////
	*/

	/* Y�� ȸ�� */
	if (m_ptPrevMouse.x <= ROTY_MIN_RANGEX || m_ptPrevMouse.x >= 65531)
	{
		m_fRotY -= 0.01;
		m_isAutoRotY = true;
		m_ptPrevMouse.x = ROTY_MIN_RANGEX;
	}
	else if (m_ptPrevMouse.x >= ROTY_MAX_RANGEX)
	{
		m_fRotY += 0.01;
		m_isAutoRotY = true;
		m_ptPrevMouse.x = ROTY_MAX_RANGEX;
	}
	else
		m_isAutoRotY = false;

	D3DXMATRIXA16 matRotX, matRotY;
	D3DXMatrixRotationX(&matRotX, m_fRotX);
	D3DXMatrixRotationY(&matRotY, m_fRotY);

	// ��Ʈ������ ����ؼ� �� ������ ��ġ ���� ����
	D3DXVec3TransformCoord(&m_vEye, &m_vEye, &(matRotX * matRotY));
	// ��Ʈ������ ����ؼ� ���� ������ ��ġ ���� ����: D3DXVec3TransformNormal()

	if (pTarget)
	{
		D3DXVECTOR3 targetPos = *pTarget;
		targetPos.y += 1.0f;

		m_vLookAt = targetPos;		// ī�޶�� �׻� Ÿ���� �ٶ󺻴�. (3��Ī ����)
		m_vEye = targetPos + m_vEye; // ������ �������� ȸ���� �������� Ÿ���� ������
	}

	// �� ��Ʈ���� ���� �� ����
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &m_vEye, &m_vLookAt, &m_vUp);
	g_pD3DDevice->SetTransform(D3DTS_VIEW, &matView);
}

void cCamera::UpdateControllByJoystick()
{
	if (g_DI_Joystick->m_Controllers->bConnected)
	{
		//���̽�ƽ ����
		if (g_DI_Joystick->m_Controllers->state.Gamepad.sThumbRX > 0)
			m_fRotY += 0.01f;
		else if (g_DI_Joystick->m_Controllers->state.Gamepad.sThumbRX < 0)
			m_fRotY -= 0.01f;
		if (g_DI_Joystick->m_Controllers->state.Gamepad.sThumbRY > 0)
			m_fRotX -= 0.01f;
		else if (g_DI_Joystick->m_Controllers->state.Gamepad.sThumbRY < 0)
			m_fRotX += 0.01f;

		if (m_fRotX <= -D3DX_PI / 6 + D3DX_16F_EPSILON)	//6
			m_fRotX = -D3DX_PI / 6 + D3DX_16F_EPSILON;	//6
		else if (m_fRotX >= D3DX_PI / 4 - D3DX_16F_EPSILON)	//4
			m_fRotX = D3DX_PI / 4 - D3DX_16F_EPSILON;		//4

		//����,�ܾƿ�, �Ÿ� ����
		if (g_DI_Joystick->GetButtonPressed(XINPUT_GAMEPAD_DPAD_DOWN))
			m_fDistance += 0.1f;
		else if (g_DI_Joystick->GetButtonPressed(XINPUT_GAMEPAD_DPAD_UP))
			m_fDistance -= 0.1f;

		if (m_fDistance <= 2.5f)
			m_fDistance = 2.5f;
		else if (m_fDistance >= 8.0f)
			m_fDistance = 8.0f;
	}
}

void cCamera::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (m_isTalk || m_isShowMenu)
	{
		m_ptPrevMouse.x = LOWORD(lParam);
		m_ptPrevMouse.y = HIWORD(lParam);
	}

	switch (message)
	{
	case WM_MOUSEMOVE:
		POINT ptCurrMouse;	// ������ �� ���� ���� ��ġ�� ã�Ƽ�
		ptCurrMouse.x = LOWORD(lParam);
		ptCurrMouse.y = HIWORD(lParam);

		/* Y�� ȸ�� */
		if (!m_isAutoRotY)
		{
			m_fRotY += (ptCurrMouse.x - m_ptPrevMouse.x) / m_fMouseSensitivity;

			// Y�� ȸ���� ���� �ְ� ������ �����ϱ� ���ؼ� ���� ���� �ش�.
			if (m_fRotY <= 0)
				m_fRotY = D3DX_PI * 2;
			else if (m_fRotY >= D3DX_PI * 2)
				m_fRotY = 0;
		}

		/* X�� ȸ�� */
		m_fRotX += (ptCurrMouse.y - m_ptPrevMouse.y) / m_fMouseSensitivity;

		// X�� ȸ���� ���� �ְ� ������ �����ϱ� ���ؼ� ���� ���� �ش�.
		if (m_fRotX <= -D3DX_PI / 6 + D3DX_16F_EPSILON)	//6
			m_fRotX = -D3DX_PI / 6 + D3DX_16F_EPSILON;	//6
		else if (m_fRotX >= D3DX_PI / 4 - D3DX_16F_EPSILON)	//4
			m_fRotX = D3DX_PI / 4 - D3DX_16F_EPSILON;		//4

		m_ptPrevMouse = ptCurrMouse;
		break;

	case WM_MOUSEWHEEL:
		m_fDistance -= GET_WHEEL_DELTA_WPARAM(wParam) / m_fMouseSensitivity;
		
		//����,�ܾƿ� �Ÿ� ����
		if (m_fDistance <= 2.5f)
			m_fDistance = 2.5f;
		else if (m_fDistance >= 8.0f)
			m_fDistance = 8.0f;
		break;
	}
}

//������Լ�
void cCamera::DebugRender()
{
	g_pD3DDevice->SetFVF(ST_RHWC_VERTEX::FVF);
	g_pD3DDevice->DrawPrimitiveUP(D3DPT_LINELIST, 4, &m_vecVertex[0], sizeof(ST_RHWC_VERTEX));
}
