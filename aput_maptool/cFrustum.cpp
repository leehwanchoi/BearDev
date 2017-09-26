#include "stdafx.h"
#include "cFrustum.h"


cFrustum::cFrustum()
{
}


cFrustum::~cFrustum()
{
}

void cFrustum::Setup()
{
	// 프로젝션 공간에서 평면에 대한 좌표를 설정한다.
	m_vecOriProjVertex.clear();
	m_vecOriProjVertex.push_back(D3DXVECTOR3(-1.5f,-1.5f, 1.0f)); //0
	m_vecOriProjVertex.push_back(D3DXVECTOR3(-1.5f, 1.5f, 1.0f)); //1
	m_vecOriProjVertex.push_back(D3DXVECTOR3( 1.5f, 1.5f, 1.0f)); //2
	m_vecOriProjVertex.push_back(D3DXVECTOR3( 1.5f,-1.5f, 1.0f)); //3
	m_vecOriProjVertex.push_back(D3DXVECTOR3(-1.5f,-1.5f,-1.0f)); //4
	m_vecOriProjVertex.push_back(D3DXVECTOR3(-1.5f, 1.5f,-1.0f)); //5
	m_vecOriProjVertex.push_back(D3DXVECTOR3( 1.5f, 1.5f,-1.0f)); //6
	m_vecOriProjVertex.push_back(D3DXVECTOR3( 1.5f,-1.5f,-1.0f)); //7

	m_vecPlane.resize(6);
}

void cFrustum::Update()
{
	vector<D3DXVECTOR3> vecWorldVertex(8);

	D3DXMATRIXA16 matProj, matView;
	g_pD3DDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	g_pD3DDevice->GetTransform(D3DTS_VIEW, &matView);

	// 정점 8개의 정보를 프로젝션 공간에서 월드 공간의 정점 정보로 변환 한다.
	for (int i = 0; i < vecWorldVertex.size(); ++i)
	{
		D3DXVec3Unproject(&vecWorldVertex[i], &m_vecOriProjVertex[i],
			NULL,
			&matProj,
			&matView,
			NULL);
	}

	// 6개의 평면 생성
	D3DXPlaneFromPoints(&m_vecPlane[0],
		&vecWorldVertex[0],
		&vecWorldVertex[1],
		&vecWorldVertex[2]);

	D3DXPlaneFromPoints(&m_vecPlane[1],
		&vecWorldVertex[4],
		&vecWorldVertex[6],
		&vecWorldVertex[5]);

	D3DXPlaneFromPoints(&m_vecPlane[2],
		&vecWorldVertex[0],
		&vecWorldVertex[5],
		&vecWorldVertex[1]);

	D3DXPlaneFromPoints(&m_vecPlane[3],
		&vecWorldVertex[3],
		&vecWorldVertex[2],
		&vecWorldVertex[6]);

	D3DXPlaneFromPoints(&m_vecPlane[4],
		&vecWorldVertex[0],
		&vecWorldVertex[7],
		&vecWorldVertex[4]);

	D3DXPlaneFromPoints(&m_vecPlane[5],
		&vecWorldVertex[1],
		&vecWorldVertex[5],
		&vecWorldVertex[6]);
}

bool cFrustum::IsInFrustum(ST_SPHERE * pSphere)
{
	// 평면 6개에 대해서 모두 검사를 해야 한다.
	for each (D3DXPLANE p in m_vecPlane)
	{
		// < 0 앞면, > 0 후면, = 0 면 위 : 앞면이면 평면 바깥쪽이다.
		if (D3DXPlaneDotCoord(&p, &pSphere->vCenter) < 0)
		{
			// 평면 중 하나라도 앞면이 나온다면 범위를 벗어난 것이다.
			pSphere->isInFrustum = false;
			return false;
		}
	}

	// 6면 모두 후면이 나왔다면 육면체 안에 있는 것이기 때문에 그려야 한다.
	pSphere->isInFrustum = true;
	return true;
}
