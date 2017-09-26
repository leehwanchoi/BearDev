#pragma once

struct childST_SIZE
{
	float fWidth;
	float fHeight;

	childST_SIZE() : fWidth(0.0f), fHeight(0.0f) {}
	childST_SIZE(float w, float h) : fWidth(w), fHeight(h) {}
};

class cChildWindowUIobject : public cObject
{
protected:
	vector<cChildWindowUIobject*>		m_vecChild;
	D3DXMATRIXA16						m_matWorld;

	SYNTHESIZE(D3DXVECTOR3, m_vLocalPos, LocalPos);
	SYNTHESIZE(cChildWindowUIobject*, m_pParent, Parent);
	SYNTHESIZE(childST_SIZE, m_stSize, Size);
	SYNTHESIZE(bool, m_isDebugRender, DebugRender);
	SYNTHESIZE(int, m_nTag, Tag);

public:
	cChildWindowUIobject();
	~cChildWindowUIobject();

	virtual void Update();
	virtual void UpdateWorldTM();
	virtual void UpdateChildren();
	virtual void Render(LPD3DXSPRITE pSprite);
	virtual void AddChild(cChildWindowUIobject* pChild);
	virtual cChildWindowUIobject* GetChildByTag(int tag);
};

