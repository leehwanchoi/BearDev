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
	// �������� �������� ��鿡 ���� ��ǥ�� �����Ѵ�.
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

	// ���� 8���� ������ �������� �������� ���� ������ ���� ������ ��ȯ �Ѵ�.
	for (int i = 0; i < vecWorldVertex.size(); ++i)
	{
		D3DXVec3Unproject(&vecWorldVertex[i], &m_vecOriProjVertex[i],
			NULL,
			&matProj,
			&matView,
			NULL);
	}

	// 6���� ��� ����
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
	// ��� 6���� ���ؼ� ��� �˻縦 �ؾ� �Ѵ�.
	for each (D3DXPLANE p in m_vecPlane)
	{
		// < 0 �ո�, > 0 �ĸ�, = 0 �� �� : �ո��̸� ��� �ٱ����̴�.
		if (D3DXPlaneDotCoord(&p, &pSphere->vCenter) < 0)
		{
			// ��� �� �ϳ��� �ո��� ���´ٸ� ������ ��� ���̴�.
			pSphere->isInFrustum = false;
			return false;
		}
	}

	// 6�� ��� �ĸ��� ���Դٸ� ����ü �ȿ� �ִ� ���̱� ������ �׷��� �Ѵ�.
	pSphere->isInFrustum = true;
	return true;
}
