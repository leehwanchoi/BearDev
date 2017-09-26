#include "stdafx.h"
#include "cRay.h"


cRay::cRay()
	: m_vOrg(0, 0, 0)
	, m_vDir(0, 0, 1)
{
	m_eRaySpace = E_NONE;
}


cRay::~cRay()
{
}

cRay cRay::RayAtViewSpace(int x, int y)
{
	D3DVIEWPORT9 vp;
	g_pD3DDevice->GetViewport(&vp);

	D3DXMATRIXA16 matProj;
	g_pD3DDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	cRay ray;
	ray.m_vDir.x = ((x * 2.0f) / vp.Width - 1.0f) / matProj._11;
	ray.m_vDir.y = ((-y * 2.0f) / vp.Height + 1.0f) / matProj._22;
	ray.m_vDir.z = 1.0f;

	ray.m_eRaySpace = E_VIEW;

	return ray;
}

cRay cRay::RayAtWorldSpace(int x, int y)
{
	cRay ray = cRay::RayAtViewSpace(x, y);

	D3DXMATRIXA16 matView, matInvView;
	g_pD3DDevice->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matInvView, NULL, &matView);

	D3DXVec3TransformCoord(&ray.m_vOrg, &ray.m_vOrg, &matInvView);
	D3DXVec3TransformNormal(&ray.m_vDir, &ray.m_vDir, &matInvView);
	D3DXVec3Normalize(&ray.m_vDir, &ray.m_vDir);

	ray.m_eRaySpace = E_WORLD;

	return ray;
}

bool cRay::IsPicked(ST_SPHERE* pSphere)
{
	assert(m_eRaySpace == E_WORLD && "레이가 월드 공간으로 변환이 되지 않았음.");
	
	D3DXVECTOR3 vLocalOrg = m_vOrg - pSphere->vCenter;

	// Q 반직선의 출발점, V 반직선의 방향
	float qv = D3DXVec3Dot(&vLocalOrg, &m_vDir);
	float vv = D3DXVec3Dot(&m_vDir, &m_vDir);
	float qq = D3DXVec3Dot(&vLocalOrg, &vLocalOrg);
	float rr = pSphere->fRadius * pSphere->fRadius;

	// 구와 반직선의 연립방정식으 판별식 (QV)(QV)-(VV)(QQ - rr) >= 0
	return qv * qv - vv * (qq - rr) >= 0;
}
