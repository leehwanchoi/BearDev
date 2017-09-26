#include "stdafx.h"
#include "cInterpolation.h"


cInterpolation::cInterpolation()
	: m_vFrom(0, 0, 0)
	, m_vMid(0, 0, 0)
	, m_vTo(0, 0, 0)
	, m_fActionTime(2)
	, m_fPassedTime(0)
	, m_nNextIndex(0)
{
	D3DXMatrixIdentity(&m_matWorld);
}


cInterpolation::~cInterpolation()
{
}

void cInterpolation::Setup()
{
	ST_PC_VERTEX stVertex;
	stVertex.c = D3DCOLOR_XRGB(255, 0, 0);

	m_vecTarget.push_back(D3DXVECTOR3( 10.0f, 0.0f, 0.0f));
	m_vecTarget.push_back(D3DXVECTOR3(  5.0f, 0.0f, sqrtf(75.0f)));
	m_vecTarget.push_back(D3DXVECTOR3( -5.0f, 0.0f, sqrtf(75.0f)));
	m_vecTarget.push_back(D3DXVECTOR3(-10.0f, 0.0f, 0.0f));
	m_vecTarget.push_back(D3DXVECTOR3(-5.0f, 0.0f,-sqrtf(75.0f)));
	m_vecTarget.push_back(D3DXVECTOR3( 5.0f, 0.0f,-sqrtf(75.0f)));

	for (int i = 0; i < 6; ++i)
	{
		stVertex.p = m_vecTarget[i];
		m_vecPCVertex.push_back(stVertex);
		stVertex.p = m_vecTarget[(i + 1) % 6];
		m_vecPCVertex.push_back(stVertex);
	}

	m_vTo = m_vecTarget[m_nNextIndex];
}

void cInterpolation::Update()
{
	static bool bMode = true;

	if (GetAsyncKeyState(VK_F1) & 0x0001)
		bMode = !bMode;

	if (bMode)
		LinearInterpolation();
	else
		BezierInterpolation();
}

void cInterpolation::Render()
{
	D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity(&matWorld);

	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, false);
	g_pD3DDevice->SetTexture(0, NULL);
	g_pD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);
	g_pD3DDevice->SetFVF(ST_PC_VERTEX::FVF);
	g_pD3DDevice->DrawPrimitiveUP(D3DPT_LINELIST, m_vecPCVertex.size() / 2,
		&m_vecPCVertex[0], sizeof(ST_PC_VERTEX));
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, true);
}

void cInterpolation::LinearInterpolation()
{
	D3DXMATRIXA16 matR, matT;
	D3DXVECTOR3 vPos, vDir;

	vDir = m_vTo - m_vFrom;
	m_fPassedTime += g_pTimeManager->getElapsedTime();
	float t = m_fPassedTime / m_fActionTime;

	// t 값 만큼 이동 할 위치를 계산하고 트랜슬레이션 매트릭스 생성
	vPos = m_vFrom * (1.0f - t) + m_vTo * t;
	D3DXMatrixTranslation(&matT, vPos.x, vPos.y, vPos.z);

	// 목적지를 바라보는 방향으로 회전 매트릭스를 생성
	D3DXVec3Normalize(&vDir, &vDir);
	// 뷰행렬 만드는 원리를 사용하여 만들어진 행렬의 역행렬을 통해서
	// 캐릭터를 회전 시킬 행렬을 생성한다.
	// 뷰행렬은 현재 가진 디렉션 방향으로 부터 z축 양수 방향으로의 회전을 하도록하고
	// 위치를 원점으로 이동 시키는 행렬이다.
	// 1. Eye의 위치를 0, 0, 0 으로 잡아서 회전량만 가지는 행렬을 생성하도록 한다.
	// 2. 그렇게 만든 행렬의 역행렬을 만들어서 z축 양수 방향으로 부터 dir 방향으로
	// 바라보도록 회전 시키는 행렬을 만들어 낸다.
	D3DXMatrixLookAtLH(&matR, &D3DXVECTOR3(0, 0, 0), &vDir, &D3DXVECTOR3(0, 1, 0));
	// 역행렬(회전 값만 가진 행렬의 역행렬은 간단하게 행렬의 전치로 구할 수 있다.)
	D3DXMatrixTranspose(&matR, &matR);

	m_matWorld = matR * matT;

	if (m_fPassedTime >= m_fActionTime)
	{
		m_fPassedTime = 0.0f;
		m_vFrom = vPos;
		m_nNextIndex = (m_nNextIndex + 1) % m_vecTarget.size();
		m_vTo = m_vecTarget[m_nNextIndex];
	}
}

void cInterpolation::BezierInterpolation()
{
	D3DXMATRIXA16 matR, matT;
	D3DXVECTOR3 vPos, vLerp1, vLerp2, vDir;

	m_fPassedTime += g_pTimeManager->getElapsedTime();
	float t = m_fPassedTime / m_fActionTime;

	D3DXVec3Lerp(&vLerp1, &m_vFrom, &m_vMid, t);
	D3DXVec3Lerp(&vLerp2, &m_vMid, &m_vTo, t);

	vDir = vLerp2 - vLerp1;
	D3DXVec3Lerp(&vPos, &vLerp1, &vLerp2, t);
	D3DXMatrixTranslation(&matT, vPos.x, vPos.y, vPos.z);

	// 목적지를 바라보는 방향으로 회전 매트릭스를 생성
	D3DXVec3Normalize(&vDir, &vDir);
	D3DXMatrixLookAtLH(&matR, &D3DXVECTOR3(0, 0, 0), &vDir, &D3DXVECTOR3(0, 1, 0));
	D3DXMatrixTranspose(&matR, &matR);

	m_matWorld = matR * matT;

	if (m_fPassedTime >= m_fActionTime)
	{
		m_fPassedTime = 0.0f;
		m_vFrom = vPos;
		m_nNextIndex = (m_nNextIndex + 1) % m_vecTarget.size();
		m_vMid = m_vecTarget[m_nNextIndex];
		m_nNextIndex = (m_nNextIndex + 1) % m_vecTarget.size();
		m_vTo = m_vecTarget[m_nNextIndex];
	}
}