#pragma once
#include "cGameNode.h"
#include "cUIButton.h"

enum
{
	E_CREDITBUTTON1 = 0,
	E_CREDITTEXTVIEW
};

class cCreditScene : public cGameNode, public iUIButtonDelegate
{
private:
	cUIObject*				m_pUIRoot;
	LPD3DXSPRITE			m_pSprite;

	string					m_sCredit;
	D3DXVECTOR3				m_vCreditPos;

public:
	cCreditScene();
	virtual ~cCreditScene();

	virtual HRESULT init() override;
	virtual void update()override;
	virtual void Release() override;
	virtual void render()  override;

	virtual void OnClick(cUIButton* pSender) override;

	void UpCredit();
};