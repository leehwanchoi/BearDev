#pragma once
#include "cChildWindowUIobject.h"

class cChildWindowUITextView : public cChildWindowUIobject
{
private:
	SYNTHESIZE(string, m_sText, Text);
	SYNTHESIZE(DWORD, m_dwDrawTextFormat, DrawTextFormat);
	SYNTHESIZE(DWORD, m_dwColor, Color);
	SYNTHESIZE_ADD_REF(LPD3DXFONT, m_pFont, Font);

public:
	cChildWindowUITextView();
	~cChildWindowUITextView();

	virtual void Render(LPD3DXSPRITE pSprite) override;
};

