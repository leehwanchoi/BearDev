#include "stdafx.h"
#include "cPickingCamera.h"


cPickingCamera::cPickingCamera()
	: m_fDistance(5.0f)
	, m_vEye(0, m_fDistance, -m_fDistance)
	, m_vLookAt(0, 0, 0)
	, m_vUp(0, 1, 0)
	, m_fRotY(0.0f)
	, m_fRotX(0.0f)
	, m_isRButtonDown(false)
{
}


cPickingCamera::~cPickingCamera()
{
}

void cPickingCamera::Setup()
{
	RECT rc;
	GetClientRect(g_hWnd, &rc);

	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, rc.right / (float)rc.bottom, 1, 1000);
	g_pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);
}

void cPickingCamera::Update(D3DXVECTOR3* pTarget)
{
	m_vEye = D3DXVECTOR3(0, 0, -m_fDistance);

	D3DXMATRIXA16 matRotX, matRotY;
	D3DXMatrixRotationX(&matRotX, m_fRotX);
	D3DXMatrixRotationY(&matRotY, m_fRotY);

	// 매트릭스를 사용해서 점 벡터의 위치 값을 변경
	D3DXVec3TransformCoord(&m_vEye, &m_vEye, &(matRotX * matRotY));
	// 매트릭스를 사용해서 방향 벡터의 위치 값을 변경: D3DXVec3TransformNormal()

	if (pTarget)
	{
		m_vLookAt = *pTarget;		// 카메라는 항상 타겟을 바라본다. (3인칭 시점)
		m_vEye = *pTarget + m_vEye; // 원점을 기준으로 회전이 끝났으면 타겟을 따라가자
	}

	// 뷰 매트릭스 생성 및 셋팅
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &m_vEye, &m_vLookAt, &m_vUp);
	g_pD3DDevice->SetTransform(D3DTS_VIEW, &matView);
}

void cPickingCamera::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_RBUTTONDOWN:	// 우클릭을 하면 현재 마우스 위치를 저장하고
		m_ptPrevMouse.x = LOWORD(lParam);
		m_ptPrevMouse.y = HIWORD(lParam);
		m_isRButtonDown = true;
		break;
	case WM_RBUTTONUP:
		m_isRButtonDown = false;
		break;
	case WM_MOUSEMOVE:
	{
		if (m_isRButtonDown)
		{
			POINT ptCurrMouse;	// 움직일 때 마다 현재 위치를 찾아서
			ptCurrMouse.x = LOWORD(lParam);
			ptCurrMouse.y = HIWORD(lParam);

			// 이전 위치와의 차이 값으로 회전량을 결정한다.
			m_fRotY += (ptCurrMouse.x - m_ptPrevMouse.x) / 100.0f;
			m_fRotX += (ptCurrMouse.y - m_ptPrevMouse.y) / 100.0f;

			// X축 회전에 의한 왜곡 현상을 방지하기 위해서 제한 값을 준다.
			if (m_fRotX <= -D3DX_PI * 0.5f + D3DX_16F_EPSILON)
				m_fRotX = -D3DX_PI * 0.5f + D3DX_16F_EPSILON;
			else if (m_fRotX >= D3DX_PI * 0.5f - D3DX_16F_EPSILON)
				m_fRotX = D3DX_PI * 0.5f - D3DX_16F_EPSILON;

			m_ptPrevMouse = ptCurrMouse;
		}
	}
		break;
	case WM_MOUSEWHEEL:
		//m_fDistance -= GET_WHEEL_DELTA_WPARAM(wParam) / 100.0f;
		//if (m_fDistance < 1)
		//	m_fDistance = 1;
		break;
	}
}