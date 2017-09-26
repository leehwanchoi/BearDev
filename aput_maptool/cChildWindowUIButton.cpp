#include "stdafx.h"
#include "cChildWindowUIButton.h"


cChildWindowUIButton::cChildWindowUIButton()
	: m_eButtonState(E_childNORMAL)
	, m_pDelegate(NULL)
{
}


cChildWindowUIButton::~cChildWindowUIButton()
{
}

void cChildWindowUIButton::SetTexture(string sNormal, string sMouseOver, string sSelected)
{
	D3DXIMAGE_INFO stImageInfo;

	g_pTextureManager2->AddTexture(sNormal, sNormal, &m_aTexture[E_childNORMAL], &stImageInfo);
	g_pTextureManager2->AddTexture(sMouseOver, sMouseOver, &m_aTexture[E_childMOUSEOVER], &stImageInfo);
	g_pTextureManager2->AddTexture(sSelected, sSelected, &m_aTexture[E_childSELECTED], &stImageInfo);

	m_stSize.fWidth = stImageInfo.Width;
	m_stSize.fHeight = stImageInfo.Height;
}

void cChildWindowUIButton::Update()
{
	cChildWindowUIobject::UpdateWorldTM();

	RECT rc;
	SetRect(&rc, m_matWorld._41, m_matWorld._42,
		m_matWorld._41 + m_stSize.fWidth, m_matWorld._42 + m_stSize.fHeight);

	POINT ptCurrMouse;
	GetCursorPos(&ptCurrMouse);
	ScreenToClient(g_childhWnd, &ptCurrMouse);

	if (PtInRect(&rc, ptCurrMouse))
	{
		// Ŭ���� �ϰ� ���� �� (Ű�� ���� ����)
		if (GetKeyState(VK_LBUTTON) & 0x8000)
		{
			// ��ư���� ���콺�� �ִٸ�
			if (m_eButtonState == E_childMOUSEOVER)
			{
				m_eButtonState = E_childSELECTED;
			}
		}
		else // Ŭ���� �ϰ� ���� ���� ����(�Ǵ� �� ����)
		{
			if (m_eButtonState == E_childSELECTED) // �����ٰ� �� ��� (Ŭ��)
			{
				if (m_pDelegate)
					m_pDelegate->OnClick(this);
			}

			m_eButtonState = E_childMOUSEOVER;
		}
	}
	else
	{
		if (GetKeyState(VK_LBUTTON) & 0x8000)
		{

		}
		else
		{
			m_eButtonState = E_childNORMAL;
		}
	}

	cChildWindowUIobject::UpdateChildren();
}

void cChildWindowUIButton::Render(LPD3DXSPRITE pSprite)
{
	RECT rc;
	SetRect(&rc, 0, 0, m_stSize.fWidth, m_stSize.fHeight);
	pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);

	pSprite->SetTransform(&m_matWorld);
	pSprite->Draw(m_aTexture[m_eButtonState], &rc, &D3DXVECTOR3(0, 0, 0),
		&D3DXVECTOR3(0, 0, 0), D3DCOLOR_XRGB(255, 255, 255));

	pSprite->End();

	cChildWindowUIobject::Render(pSprite);
}
