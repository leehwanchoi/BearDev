#pragma once
#include "cGameNode.h"
#include "iMap.h"

class cSkinnedMesh;

class cCamera;
class cSkyBox;
class cXobjectLoader;
class cLoadMap;
class cLoadingScene;

class cParticle_Firework;
class cParticle_Explosion;
class cParticle_Snow;
class cPulsatingStar;

class cShowMenu;
class cUIObject;

class cEndingScene : public cGameNode
{
private:
	cSkyBox*						m_pSkyBox;
	cLoadMap*						m_pLoadMap;
	cLoadingScene*					m_pLoadingScene;
	iMap*							m_pMap;

	cCamera*						m_pCamera;

	vector<cParticle_Firework*>		m_vec_Firework;
	vector<cParticle_Explosion*>	m_vec_Explosion;
	cParticle_Snow*					m_pPtc_Snow;
	vector<cPulsatingStar*>			m_vec_PulsatingStar;
	
	cShowMenu*						m_pShowMenu;

	LPD3DXSPRITE					m_pSprite;
	cUIObject*						m_pUIRoot;

public:
	cEndingScene();
	~cEndingScene();

	virtual HRESULT init() override;
	virtual void Release() override;
	virtual void update() override;
	virtual void render() override;

	virtual void WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
};