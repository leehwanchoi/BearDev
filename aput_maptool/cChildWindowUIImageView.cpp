#include "stdafx.h"
#include "cChildWindowUIImageView.h"


cChildWindowUIImageView::cChildWindowUIImageView()
	: m_pTexture(NULL)
	, m_dwColor(D3DCOLOR_XRGB(255, 255, 255))
{
}


cChildWindowUIImageView::~cChildWindowUIImageView()
{
	SAFE_RELEASE(m_pTexture);
}

void cChildWindowUIImageView::Render(LPD3DXSPRITE pSprite)
{
	if (m_pTexture)
	{
		RECT rc;
		SetRect(&rc, 0, 0, m_stSize.fWidth, m_stSize.fHeight);

		pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);

		pSprite->SetTransform(&m_matWorld);
		pSprite->Draw(m_pTexture, &rc, &D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(0, 0, 0), m_dwColor);

		pSprite->End();
	}

	cChildWindowUIobject::Render(pSprite);
}
