#pragma once
#include"cGameNode.h"

#include "iMap.h"

#define NUM_SKINNEDMESH 20

class cCamera;
class cCharacterController;
class cAseCharacter;
class cHeightMap;
class cAStar;
class cUIDraw;
class cSkinnedMesh;
class cFrustum;
class cAlphaBlendTest;

class cMainScene : public cGameNode
{
private:
	cCamera*				m_pCamera;
	cCharacterController*	m_pController;

	iMap*					m_pMap;
	cHeightMap*				m_pHeight;

	cAseCharacter*			m_pAseCharacter;

	cAStar*					m_pAStar;

	cUIDraw*				m_pUIDraw;

	cFrustum*				m_pFrustum;

	cAlphaBlendTest*		m_pAlphaTest;

	cSkinnedMesh*			m_pSkinnedMesh;
	vector<cSkinnedMesh*>	m_vecSkinnedMesh;

	LPD3DXFONT				m_pFont;
	LPRECT					m_pRect;

public:
	cMainScene();
	virtual ~cMainScene();

	virtual HRESULT Init() override;
	virtual void Release() override;
	virtual void Update() override;
	virtual void Render() override;

	virtual void WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
};

