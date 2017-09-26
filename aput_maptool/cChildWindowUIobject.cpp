#include "stdafx.h"
#include "cChildWindowUIobject.h"


cChildWindowUIobject::cChildWindowUIobject()
	: m_vLocalPos(0, 0, 0)
	, m_pParent(NULL)
	, m_stSize(0, 0)
	, m_isDebugRender(false)
	, m_nTag(0)
{
	D3DXMatrixIdentity(&m_matWorld);
}


cChildWindowUIobject::~cChildWindowUIobject()
{
}

void cChildWindowUIobject::Update()
{
	UpdateWorldTM();
	UpdateChildren();
}

void cChildWindowUIobject::UpdateWorldTM()
{
	m_matWorld._41 = m_vLocalPos.x;
	m_matWorld._42 = m_vLocalPos.y;
	m_matWorld._43 = m_vLocalPos.z;

	if (m_pParent)
	{
		m_matWorld._41 += m_pParent->m_matWorld._41;
		m_matWorld._42 += m_pParent->m_matWorld._42;
		m_matWorld._43 += m_pParent->m_matWorld._43;
	}
}

void cChildWindowUIobject::UpdateChildren()
{
	for each(auto p in m_vecChild)
		p->Update();
}

void cChildWindowUIobject::Render(LPD3DXSPRITE pSprite)
{
	if (m_isDebugRender)
	{
		float l = m_matWorld._41;
		float t = m_matWorld._42;
		float r = m_matWorld._41 + m_stSize.fWidth;
		float b = m_matWorld._42 + m_stSize.fHeight;

		D3DCOLOR c = D3DCOLOR_XRGB(255, 0, 0);
		vector<ST_RHWC_VERTEX> vecVertex;
		vecVertex.reserve(8);

		vecVertex.push_back(ST_RHWC_VERTEX(D3DXVECTOR4(l, t, 0, 1), c));
		vecVertex.push_back(ST_RHWC_VERTEX(D3DXVECTOR4(r, t, 0, 1), c));

		vecVertex.push_back(ST_RHWC_VERTEX(D3DXVECTOR4(r, t, 0, 1), c));
		vecVertex.push_back(ST_RHWC_VERTEX(D3DXVECTOR4(r, b, 0, 1), c));

		vecVertex.push_back(ST_RHWC_VERTEX(D3DXVECTOR4(r, b, 0, 1), c));
		vecVertex.push_back(ST_RHWC_VERTEX(D3DXVECTOR4(l, b, 0, 1), c));

		vecVertex.push_back(ST_RHWC_VERTEX(D3DXVECTOR4(l, b, 0, 1), c));
		vecVertex.push_back(ST_RHWC_VERTEX(D3DXVECTOR4(l, t, 0, 1), c));

		g_pD3DDevice2->SetFVF(ST_RHWC_VERTEX::FVF);
		g_pD3DDevice2->DrawPrimitiveUP(D3DPT_LINELIST, 4, &vecVertex[0], sizeof(ST_RHWC_VERTEX));
	}

	for each (auto p in m_vecChild)
		p->Render(pSprite);
}

void cChildWindowUIobject::AddChild(cChildWindowUIobject* pChild)
{
	pChild->m_pParent = this;
	m_vecChild.push_back(pChild);
}

cChildWindowUIobject* cChildWindowUIobject::GetChildByTag(int tag)
{
	if (m_nTag == tag)
		return this;

	for each (auto p in m_vecChild)
	{
		cChildWindowUIobject* pChild = p->GetChildByTag(tag);
		if (pChild)
			return pChild;
	}

	return NULL;
}
