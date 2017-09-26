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
	다이렉트X 에서는 월드, 뷰, 프로젝션, 뷰포트 매트릭스를 프로그래머가 직접 연산처리 하지 않고
	내부적으로 처리가 되기 때문에 생성 후에 장치에 셋팅만 해주면 된다.
	뷰포트 매트릭스는 셋팅을 따로 하지 않으면 기본적으로 화면 전체를 사용하도록 셋팅이 되어 있다.
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

	// 프로젝션 매트릭스 생성 및 셋팅
	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, rc.right / (float)rc.bottom, 1, 1000);
	g_pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);

	// 카메라의 회전과 이동도 RT 순서로 해야되므로
	// 원점을 기준으로 카메라 위치를 잡고 회전, 이동을 적용 시킨다.
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

void cCamera::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
		break;
	case WM_MOUSEWHEEL:
		m_fDistance -= GET_WHEEL_DELTA_WPARAM(wParam) / 100.0f;
		break;
	}
}

