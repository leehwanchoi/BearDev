#include "stdafx.h"
#include "cEndingScene.h"

#include "cSkinnedMesh.h"
#include "cSkyBox.h"
#include "cXobjectLoader.h"
#include "cLoadMap.h"
#include "cLoadingScene.h"
#include "cCamera.h"
#include "cParticle_Firework.h"
#include "cParticle_Explosion.h"
#include "cParticle_Snow.h"
#include "cPulsatingStar.h"
#include "cShowMenu.h"
#include "cUITextView.h"

cEndingScene::cEndingScene()
	: m_pSkyBox			(NULL)
	, m_pLoadMap		(NULL)
	, m_pMap			(NULL)
	, m_pLoadingScene	(NULL)
	, m_pCamera			(NULL)
	, m_pPtc_Snow		(NULL)
	, m_pShowMenu		(NULL)
	, m_pSprite			(NULL)
	, m_pUIRoot			(NULL)
{
}

cEndingScene::~cEndingScene()
{
	SAFE_RELEASE(m_pSprite);
}

HRESULT cEndingScene::init()
{
	//스테이지 넘버 셋팅
	g_pStageManager->SetStageNum(0);

	//스카이박스 추가 및 초기화
	m_pSkyBox = new cSkyBox;
	m_pSkyBox->init();
	m_pSkyBox->AutoRelease();

	m_pLoadMap = new cLoadMap;
	m_pLoadMap->AutoRelease();
	m_pLoadMap->RealLoad_Village();
	m_pMap = m_pLoadMap;

	m_pLoadingScene = new cLoadingScene;
	m_pLoadingScene->AutoRelease();
	m_pLoadingScene->Setup();

	//카메라
	m_pCamera = new cCamera;
	m_pCamera->AutoRelease();
	m_pCamera->Setup();

	//FX
	cParticle_Firework* Ptc_Firework;
	Ptc_Firework = new cParticle_Firework(D3DXVECTOR3(32,10,40), 100);
	Ptc_Firework->Setup("Textures/flare.bmp");
	m_vec_Firework.push_back(Ptc_Firework);

	Ptc_Firework = new cParticle_Firework(D3DXVECTOR3(25, 15, 35), 100);
	Ptc_Firework->Setup("Textures/flare.bmp");
	m_vec_Firework.push_back(Ptc_Firework);

	Ptc_Firework = new cParticle_Firework(D3DXVECTOR3(25, 15, 55), 100);
	Ptc_Firework->Setup("Textures/flare.bmp");
	m_vec_Firework.push_back(Ptc_Firework);

	Ptc_Firework = new cParticle_Firework(D3DXVECTOR3(45, 15, 55), 100);
	Ptc_Firework->Setup("Textures/flare.bmp");
	m_vec_Firework.push_back(Ptc_Firework);

	cParticle_Explosion* Ptc_Explosion;
	Ptc_Explosion = new cParticle_Explosion(D3DXVECTOR3(30, 15, 20), 30);
	Ptc_Explosion->Setup("Textures/flare.bmp");
	m_vec_Explosion.push_back(Ptc_Explosion);

	Ptc_Explosion = new cParticle_Explosion(D3DXVECTOR3(27, 15, 30), 30);
	Ptc_Explosion->Setup("Textures/flare.bmp");
	m_vec_Explosion.push_back(Ptc_Explosion);

	BoundingBox bBox(D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(128, 30, 128));
	m_pPtc_Snow = new cParticle_Snow(bBox, 250);
	m_pPtc_Snow->Setup("Textures/snowflake.dds");

	cPulsatingStar* PulsatingStar;
	PulsatingStar = new cPulsatingStar();
	PulsatingStar->Setup();
	PulsatingStar->SetPosition(30, 3, 40);
	PulsatingStar->SetScailing(1.0f, 1.0f, 1.0f);
	m_vec_PulsatingStar.push_back(PulsatingStar);

	PulsatingStar = new cPulsatingStar();
	PulsatingStar->Setup();
	PulsatingStar->SetPosition(40, 5, 35);
	PulsatingStar->SetScailing(1.0f, 1.0f, 1.0f);
	m_vec_PulsatingStar.push_back(PulsatingStar);

	PulsatingStar = new cPulsatingStar();
	PulsatingStar->Setup();
	PulsatingStar->SetPosition(35, 6, 35);
	PulsatingStar->SetScailing(1.0f, 1.0f, 1.0f);
	m_vec_PulsatingStar.push_back(PulsatingStar);

	m_pShowMenu = new cShowMenu;
	m_pShowMenu->AutoRelease();
	m_pShowMenu->Setup();

	D3DXCreateSprite(g_pD3DDevice, &m_pSprite);
	cUITextView* pTextView = new cUITextView;
	pTextView->AutoRelease();
	pTextView->SetText("Thanks for Playing !");
	pTextView->SetSize(ST_SIZE(250, 50));
	pTextView->SetLocalPos(D3DXVECTOR3((WINSIZEX / 2) - (pTextView->GetSize().fWidth / 2), (WINSIZEY / 2) - (pTextView->GetSize().fHeight / 2) - 50, 0));
	pTextView->SetFont(g_pFontManager->GetFont(cFontManager::E_QUEST));
	pTextView->SetDebugRender(false);
	pTextView->SetDrawTextFormat(DT_CENTER | DT_VCENTER | DT_WORDBREAK);
	pTextView->SetColor(D3DCOLOR_XRGB(255, 255, 255));
	m_pUIRoot = pTextView;

	//BossMap BGM 정지
	g_pSoundManager->stop("./Sound/BossMap/FranticLevel.wav");

	//Ending BGM 재생
	g_pSoundManager->addSound("./Sound/EndingScene/MainTheme.wav", "./Sound/EndingScene/MainTheme.wav", true, true);
	g_pSoundManager->play("./Sound/EndingScene/MainTheme.wav", 0.8f);

	return S_OK;
}

void cEndingScene::Release()
{
	if (m_vec_Firework.size() > 0)
	{
		for (int i = 0; i < m_vec_Firework.size(); i++)
			m_vec_Firework[i]->Destroy();
	}

	if (m_vec_Explosion.size() > 0)
	{
		for (int i = 0; i < m_vec_Explosion.size(); i++)
			m_vec_Explosion[i]->Destroy();
	}

	m_pPtc_Snow->Destroy();
}

void cEndingScene::update()
{
	if (m_pCamera)
	{
		m_pCamera->Update(&D3DXVECTOR3(30.751f, 1.223f, 55.1777f));
		m_pCamera->SetRotY(2.925f);
		m_pCamera->SetRotX(-0.34f);
	}

	if (m_vec_Firework.size() > 0)
	{
		for (int i = 0; i < m_vec_Firework.size(); i++)
			m_vec_Firework[i]->Update();
	}

	if (m_vec_Explosion.size() > 0)
	{
		for (int i = 0; i < m_vec_Explosion.size(); i++)
			m_vec_Explosion[i]->Update();
	}

	if (m_pPtc_Snow)
		m_pPtc_Snow->Update();

	if (m_vec_PulsatingStar.size() > 0)
	{
		for (int i = 0; i < m_vec_PulsatingStar.size(); i++)
			m_vec_PulsatingStar[i]->Update();
	}

	if (m_pShowMenu)
		m_pShowMenu->Update();

	if (m_pUIRoot)
		m_pUIRoot->Update();

	//0번/엔터/스페이스 누르면 씬전환
	if (g_pKeyManager->isOnceKeyDown('0') || 
		g_pKeyManager->isOnceKeyDown(VK_RETURN) ||
		g_pKeyManager->isOnceKeyDown(VK_SPACE))		
		m_pLoadingScene->LoadingScene("TitleScene");	//TitleScene으로 전환

	if (m_pLoadingScene)
		m_pLoadingScene->Update();
}

void cEndingScene::render()
{
	if (m_pLoadMap)
		m_pLoadMap->Render();

	if (m_pLoadingScene)
		m_pLoadingScene->Render();

	if (m_pSkyBox)
		m_pSkyBox->render();

	if (m_vec_Firework.size() > 0)
	{
		for (int i = 0; i < m_vec_Firework.size(); i++)
			m_vec_Firework[i]->Render();
	}

	if (m_vec_Explosion.size() > 0)
	{
		for (int i = 0; i < m_vec_Explosion.size(); i++)
			m_vec_Explosion[i]->Render();
	}

	if (m_pPtc_Snow)
		m_pPtc_Snow->Render();

	if (m_vec_PulsatingStar.size() > 0)
	{
		for (int i = 0; i < m_vec_PulsatingStar.size(); i++)
			m_vec_PulsatingStar[i]->Render();
	}

	if (m_pShowMenu)
		m_pShowMenu->Render();

	if (m_pUIRoot)
		m_pUIRoot->Render(m_pSprite);
}

void cEndingScene::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//if (m_pCamera)
	//	m_pCamera->WndProc(hWnd, message, wParam, lParam);
}