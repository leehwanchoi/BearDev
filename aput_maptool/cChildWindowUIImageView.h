#pragma once
#include "cChildWindowUIobject.h"

class cChildWindowUIImageView : public cChildWindowUIobject
{
private:
	SYNTHESIZE(D3DCOLOR, m_dwColor, Color);
	SYNTHESIZE_ADD_REF(LPDIRECT3DTEXTURE9, m_pTexture, Texture);

public:
	cChildWindowUIImageView();
	~cChildWindowUIImageView();

	virtual void Render(LPD3DXSPRITE pSprite) override;
};

