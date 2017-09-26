#pragma once
#include "cChildWindowUIobject.h"

class cChildWindowUIButton;

class iChildWindowUIButtonDelegate
{
public:
	virtual void OnClick(cChildWindowUIButton* pSender) PURE;
};

class cChildWindowUIButton : public cChildWindowUIobject
{
private:
	enum eButtonState { E_childNORMAL, E_childMOUSEOVER, E_childSELECTED, E_childCOUNT };

	eButtonState		m_eButtonState;
	LPDIRECT3DTEXTURE9	m_aTexture[E_childCOUNT];

	SYNTHESIZE(iChildWindowUIButtonDelegate*, m_pDelegate, Delegate);

public:
	cChildWindowUIButton();
	~cChildWindowUIButton();

	virtual void SetTexture(string sNormal, string sMouseOver, string sSelected);

	virtual void Update() override;
	virtual void Render(LPD3DXSPRITE pSprite) override;
};

