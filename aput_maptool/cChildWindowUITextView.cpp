#include "stdafx.h"
#include "cChildWindowUITextView.h"


cChildWindowUITextView::cChildWindowUITextView()
	: m_pFont(NULL)
	, m_dwDrawTextFormat(DT_LEFT | DT_TOP | DT_NOCLIP)
	, m_dwColor(D3DCOLOR_XRGB(255, 255, 255))
{
}


cChildWindowUITextView::~cChildWindowUITextView()
{
	SAFE_RELEASE(m_pFont);
}

void cChildWindowUITextView::Render(LPD3DXSPRITE pSprite)
{
	RECT rc;
	SetRect(&rc, m_matWorld._41, m_matWorld._42,
		m_matWorld._41 + m_stSize.fWidth, m_matWorld._42 + m_stSize.fHeight);

	m_pFont->DrawTextA(NULL, m_sText.c_str(), m_sText.length(), &rc,
		m_dwDrawTextFormat, m_dwColor);

	cChildWindowUIobject::Render(pSprite);
}
