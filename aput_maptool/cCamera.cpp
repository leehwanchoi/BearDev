#include "stdafx.h"
#include "cCamera.h"


cCamera::cCamera()
	: m_fDistance(5)
	, m_vEye(0, 0, -m_fDistance)
	, m_vLookAt(0, 0, 0)
	, m_vUp(0, 1, 0)
	, m_fRotY(0)
	, m_fRotX(0)
	, m_isRButtonDown(false)
{
}


cCamera::~cCamera()
{
}

void cCamera::Setup()
{
	/*
	���̷�ƮX ������ ����, ��, ��������, ����Ʈ ��Ʈ������ ���α׷��Ӱ� ���� ����ó�� ���� �ʰ�
	���������� ó���� �Ǳ� ������ ���� �Ŀ� ��ġ�� ���ø� ���ָ� �ȴ�.
	����Ʈ ��Ʈ������ ������ ���� ���� ������ �⺻������ ȭ�� ��ü�� ����ϵ��� ������ �Ǿ� �ִ�.
	*/
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

	D3DXMATRIXA16 matRotX, matRotY;
	D3DXMatrixRotationX(&matRotX, m_fRotX);
	D3DXMatrixRotationY(&matRotY, m_fRotY);

	// ��Ʈ������ ����ؼ� �� ������ ��ġ ���� ����
	D3DXVec3TransformCoord(&m_vEye, &m_vEye, &(matRotX * matRotY));
	// ��Ʈ������ ����ؼ� ���� ������ ��ġ ���� ����: D3DXVec3TransformNormal()

	if (pTarget)
	{
		m_vLookAt = *pTarget;		// ī�޶�� �׻� Ÿ���� �ٶ󺻴�. (3��Ī ����)
		m_vEye = *pTarget + m_vEye; // ������ �������� ȸ���� �������� Ÿ���� ������
	}

	// �� ��Ʈ���� ���� �� ����
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &m_vEye, &m_vLookAt, &m_vUp);
	g_pD3DDevice->SetTransform(D3DTS_VIEW, &matView);
}

void cCamera::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_RBUTTONDOWN:	// ��Ŭ���� �ϸ� ���� ���콺 ��ġ�� �����ϰ�
		m_ptPrevMouse.x = LOWORD(lParam);
		m_ptPrevMouse.y = HIWORD(lParam);
		m_isRButtonDown = true;
		break;
	case WM_RBUTTONUP:
		m_isRButtonDown = false;
		break;
	case WM_MOUSEMOVE:
		if (m_isRButtonDown)
		{
			POINT ptCurrMouse;	// ������ �� ���� ���� ��ġ�� ã�Ƽ�
			ptCurrMouse.x = LOWORD(lParam);
			ptCurrMouse.y = HIWORD(lParam);

			// ���� ��ġ���� ���� ������ ȸ������ �����Ѵ�.
			m_fRotY += (ptCurrMouse.x - m_ptPrevMouse.x) / 100.0f;
			m_fRotX += (ptCurrMouse.y - m_ptPrevMouse.y) / 100.0f;

			// X�� ȸ���� ���� �ְ� ������ �����ϱ� ���ؼ� ���� ���� �ش�.
			if (m_fRotX <= -D3DX_PI * 0.5f + D3DX_16F_EPSILON)
				m_fRotX = -D3DX_PI * 0.5f + D3DX_16F_EPSILON;
			else if (m_fRotX >= D3DX_PI * 0.5f - D3DX_16F_EPSILON)
				m_fRotX = D3DX_PI * 0.5f - D3DX_16F_EPSILON;

			m_ptPrevMouse = ptCurrMouse;
		}
		break;
	case WM_MOUSEWHEEL:
		m_fDistance -= GET_WHEEL_DELTA_WPARAM(wParam) / 100.0f;
		break;
	}
}

