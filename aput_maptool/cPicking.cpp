#include "stdafx.h"
#include "cPicking.h"


cPicking::cPicking()
	: m_pSphere(NULL)
	, m_pBigSphere(NULL)
	, m_vPickedPosition(0, 0, 0)
{
}


cPicking::~cPicking()
{
	SAFE_RELEASE(m_pSphere);
	SAFE_RELEASE(m_pBigSphere);
}

void cPicking::Setup()
{
	D3DXCreateSphere(g_pD3DDevice, 1.0f, 10, 10, &m_pSphere, NULL);

	for (int i = 0; i < 50; ++i)
	{
		ST_SPHERE stSphere;
		stSphere.fRadius = 1.0f;
		stSphere.isPicked = false;
		stSphere.vCenter = D3DXVECTOR3((rand() % 501) / 10.0f - 25,
			(rand() % 501) / 10.0f - 25, (rand() % 501) / 10.0f - 25);
		m_vecSphere.push_back(stSphere);
	}

	D3DXCreateSphere(g_pD3DDevice, 2.0f, 10, 10, &m_pBigSphere, NULL);

	D3DCOLOR c = D3DCOLOR_XRGB(125, 0, 125);
	m_vecPCVertex.push_back(ST_PC_VERTEX(D3DXVECTOR3(-15, 0,-15), c));
	m_vecPCVertex.push_back(ST_PC_VERTEX(D3DXVECTOR3(-15, 0, 15), c));
	m_vecPCVertex.push_back(ST_PC_VERTEX(D3DXVECTOR3( 15, 0, 15), c));
	m_vecPCVertex.push_back(ST_PC_VERTEX(D3DXVECTOR3(-15, 0,-15), c));
	m_vecPCVertex.push_back(ST_PC_VERTEX(D3DXVECTOR3( 15, 0, 15), c));
	m_vecPCVertex.push_back(ST_PC_VERTEX(D3DXVECTOR3( 15, 0,-15), c));
}

void cPicking::Render()
{
	D3DXMATRIXA16 matWorld;

	g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	g_pD3DDevice->SetTexture(0, NULL);

	for each (auto p in m_vecSphere)
	{
		D3DXMatrixTranslation(&matWorld, p.vCenter.x, p.vCenter.y, p.vCenter.z);
		g_pD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);

		if (p.isPicked)
			g_pD3DDevice->SetMaterial(&RED_MTRL);
		else
			g_pD3DDevice->SetMaterial(&WHITE_MTRL);

		m_pSphere->DrawSubset(0);
	}

	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, false);
	D3DXMatrixIdentity(&matWorld);
	g_pD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);
	g_pD3DDevice->SetFVF(ST_PC_VERTEX::FVF);
	g_pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, &m_vecPCVertex[0],
		sizeof(ST_PC_VERTEX));
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, true);

	D3DXMatrixTranslation(&matWorld, m_vPickedPosition.x,
		m_vPickedPosition.y, m_vPickedPosition.z);
	g_pD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);
	g_pD3DDevice->SetMaterial(&BLUE_MTRL);
	m_pSphere->DrawSubset(0);
}

void cPicking::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_MBUTTONDOWN:
		{
			cRay ray = cRay::RayAtWorldSpace(LOWORD(lParam), HIWORD(lParam));
			for (int i = 0; i < m_vecSphere.size(); ++i)
			{
				m_vecSphere[i].isPicked = ray.IsPicked(&m_vecSphere[i]);
			}
		}
		break;
		case WM_LBUTTONDOWN:
		{
			cRay ray = cRay::RayAtWorldSpace(LOWORD(lParam), HIWORD(lParam));
			for (int i = 0; i < m_vecIndex.size(); i += 3)
			{
				float f;
				if (D3DXIntersectTri(&m_vecVertex[m_vecIndex[i + 0]],
					&m_vecVertex[m_vecIndex[i + 1]], &m_vecVertex[m_vecIndex[i + 2]],
					&ray.m_vOrg, &ray.m_vDir, NULL, NULL, &f))
				{
					m_vPickedPosition = ray.m_vOrg + ray.m_vDir * f;
				}
			}
		}
		break;
	}
}