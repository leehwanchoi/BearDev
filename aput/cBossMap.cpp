#include "stdafx.h"
#include "cBossMap.h"
#include "cCamera.h"
#include "cCharacterController.h"
#include "cMonsterController.h"
#include "cSkinnedMesh.h"
#include "cSkinnedCharacter.h"
#include "cSkinnedMonster.h"
#include "cSnowBall.h"
#include "cAStar.h"	
#include "cFrustum.h"
/*===================================================================================*/
/*                                New Header, This Area                              */
/*===================================================================================*/
#include "cSkyBox.h"
#include "cXobjectLoader.h"
#include "cLoadMap.h"
#include "cLoadingScene.h"
#include "cSinWave.h"
#include "cWaveWall.h"
#include "cParticle_Snow.h"
#include "cParticle_Walk.h"
#include "cSemicircleFX.h"
#include "cTeleporter.h"
#include "cTornado.h"
#include "cShowMenu.h"
#include "cShowClear.h"
#include "cShowYouDie.h"
#include "cShowStageNum.h"
#include "cAchievementBox.h"
/*===================================================================================*/

cBossMap::cBossMap()
	: m_pCamera				(NULL)	// ���� ī�޶�
	, m_pController			(NULL)	// ĳ���� ��Ʈ�ѷ�
	, m_pMonsterController	(NULL)  // ���� ��Ʈ�ѷ�
	, m_pSkinnedCharacter	(NULL)	// ĳ���͸޽�
	, m_pSkinnedMonster		(NULL)	// ���͸޽�
	, m_pSkyBox				(NULL)  // ��ī�� �ڽ�
	, m_pLoadMap			(NULL)	// �������� ���̺��� ���� �ε�
	, m_pLoadingScene		(NULL)	// �ε��� + ������
	, m_pSinWave			(NULL)	// �ĵ�
	, m_pWaveWall			(NULL)	// �ĵ���
	, m_FogEffect			(NULL)	// �Ȱ�ȿ��		
	, m_FogTechHandle		(NULL)	// �Ȱ� ��ũ �ڵ�
	, m_pP_Snow				(NULL)
	, m_pP_Walk				(NULL)
	, m_pTeleporter			(NULL)
	, m_pShowMenu			(NULL)	// �޴�
	, m_pShowClear			(NULL)
	, m_pShowYouDie			(NULL)
	, m_pShowStageNum		(NULL)
	, m_pAStar				(NULL)	// A*
	, m_pFrustum			(NULL)	// �������� �ø�
	, m_isOpenPortal		(false)
	, m_nEffectSoundPorta	(0)
	, m_isOnKey1			(false)	// ����� ���� �ʱ�ȭ
{
	//ū������ ���� �ʱ�ȭ
	m_vecSnowBall.clear();

	//������ ���� �ʱ�ȭ
	m_vecSemicircleFX.clear();

	//�޼� UI ���� �ʱ�ȭ
	m_vecAchieveBox.clear();

	//����̵� ����Ʈ ���� �ʱ�ȭ
	m_vecTornadoBarrier.clear();
	m_vecTornado.clear();

	//�浹���� �ٿ���ڽ�/��ü ���� �ʱ�ȭ
	m_vecBoundingBox.clear();			
	m_vecBoundingSphereButton.clear();	

	//����� ���� ���� �ʱ�ȭ
	m_pRect = new RECT;
	m_pRect->left	= WINSIZEX - 300;
	m_pRect->top	= WINSIZEY - 200;
	m_pRect->right	= WINSIZEX;
	m_pRect->bottom = WINSIZEY;

	D3DXCreateFont(g_pD3DDevice, 24, 12, 0, 1, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, false, "Arial", &m_pFont);
}


cBossMap::~cBossMap()
{
	SAFE_DELETE(m_pRect);
	SAFE_RELEASE(m_pFont);
}

HRESULT cBossMap::init()
{
	//�������� �ѹ� ����
	g_pStageManager->SetStageNum(4);

	//ī�޶�
	m_pCamera = new cCamera;
	m_pCamera->AutoRelease();
	m_pCamera->Setup();

	//ĳ���� ��Ʈ�ѷ�
	m_pController = new cCharacterController;
	m_pController->AutoRelease();
	m_pController->Setup(&D3DXVECTOR3(63.5, 0, 4.5));
	//m_pController->Setup(&D3DXVECTOR3(63.5, 0, 100));		//���� �׽�Ʈ��
	m_pController->SetHp(g_pStageManager->GetHp());			//�߰�
	m_pController->SetHpBar(g_pStageManager->GetHpBar());	//�߰�
	m_pController->SetHpBarIsSetHp(true);					//�߰�

	//���� ��Ʈ�ѷ�
	m_pMonsterController = new cMonsterController;
	m_pMonsterController->AutoRelease();

	//��Ų�� �޽�
	m_pSkinnedCharacter = g_pSkinnedMeshManager->GetSkinnedMesh("character", "eskimo_01.X", E_CHARACTER);
	m_pSkinnedMonster = g_pSkinnedMeshManager->GetSkinnedMesh("character", "yeti.X", E_MONSTER);
	/*===================================================================================*/
	/*                                New Source, This Area                              */
	/*===================================================================================*/
	//��ī�̹ڽ� �߰� �� �ʱ�ȭ
	m_pSkyBox = new cSkyBox;
	m_pSkyBox->init();
	m_pSkyBox->AutoRelease();

	m_pLoadMap = new cLoadMap;
	m_pLoadMap->AutoRelease();
	m_pLoadMap->RealLoad_BossMap();
	m_pMap = m_pLoadMap;
	m_pController->SetMap(m_pMap);
	m_pMonsterController->SetMap(m_pMap);
	m_pMap->GetHeight(m_pController->GetFirstPos()->x,
					  m_pController->GetFirstPos()->y,
					  m_pController->GetFirstPos()->z);

	m_pLoadingScene = new cLoadingScene;
	m_pLoadingScene->AutoRelease();
	m_pLoadingScene->Setup();

	//�ĵ� ����
	m_pSinWave = new cSinWave;
	m_pSinWave->AutoRelease();
	m_pSinWave->Init();
	m_pSinWave->SetWaveHeight(0.45f);
	m_pSinWave->SetWaveSize(4.415f, 1, 4.415f);
	m_pSinWave->SetWavePosition(0, 4.0f, 0);

	//�ĵ���
	m_pWaveWall = new cWaveWall;
	m_pWaveWall->AutoRelease();
	m_pWaveWall->Setup(0);

	m_pShowMenu = new cShowMenu;
	m_pShowMenu->AutoRelease();
	m_pShowMenu->Setup();

	m_pShowClear = new cShowClear;
	m_pShowClear->AutoRelease();
	m_pShowClear->Setup();

	m_pShowYouDie = new cShowYouDie;
	m_pShowYouDie->AutoRelease();
	m_pShowYouDie->Setup();

	//�������� UI
	m_pShowStageNum = new cShowStageNum;
	m_pShowStageNum->AutoRelease();
	m_pShowStageNum->Setup(g_pStageManager->GetStageNum());

	//�޼�UI
	cAchievementBox* pAchieveBox1 = new cAchievementBox;
	pAchieveBox1->AutoRelease();
	pAchieveBox1->Setup();
	pAchieveBox1->SetQuestName("Button1 Clear");
	m_vecAchieveBox.push_back(pAchieveBox1);

	cAchievementBox* pAchieveBox2 = new cAchievementBox;
	pAchieveBox2->AutoRelease();
	pAchieveBox2->Setup();
	pAchieveBox2->SetQuestName("Button2 Clear");
	m_vecAchieveBox.push_back(pAchieveBox2);

	cAchievementBox* pAchieveBox3 = new cAchievementBox;
	pAchieveBox3->AutoRelease();
	pAchieveBox3->Setup();
	pAchieveBox3->SetQuestName("Button3 Clear");
	m_vecAchieveBox.push_back(pAchieveBox3);

	cAchievementBox* pAchieveBox4 = new cAchievementBox;
	pAchieveBox4->AutoRelease();
	pAchieveBox4->Setup();
	pAchieveBox4->SetQuestName("Button4 Clear");
	m_vecAchieveBox.push_back(pAchieveBox4);

	cAchievementBox* pAchieveBox5 = new cAchievementBox;
	pAchieveBox5->AutoRelease();
	pAchieveBox5->Setup();
	pAchieveBox5->SetQuestName("Button5 Clear");
	m_vecAchieveBox.push_back(pAchieveBox5);

	cAchievementBox* pAchieveBox6 = new cAchievementBox;
	pAchieveBox6->AutoRelease();
	pAchieveBox6->Setup();
	pAchieveBox6->SetQuestName("Yeti Hunter!");
	m_vecAchieveBox.push_back(pAchieveBox6);
	/*===================================================================================*/

	/*===================================================================================*/
	/*                                New Effect, This Area                              */
	/*===================================================================================*/
	HRESULT hr = 0;
	ID3DXBuffer* errorBuffer = 0;
	hr = D3DXCreateEffectFromFile(
		g_pD3DDevice,
		"./fx/fog.txt",
		0,                // no preprocessor definitions
		0,                // no ID3DXInclude interface
		D3DXSHADER_DEBUG, // compile flags
		0,                // don't share parameters
		&m_FogEffect,
		&errorBuffer);

	SAFE_RELEASE(errorBuffer);
	m_FogTechHandle = m_FogEffect->GetTechniqueByName("Fog");

	BoundingBox boundingBox;
	boundingBox.min = D3DXVECTOR3(m_pController->GetPosition()->x - 30.0f, m_pController->GetPosition()->y - 30.0f, m_pController->GetPosition()->z - 30.0f);
	boundingBox.max = D3DXVECTOR3(m_pController->GetPosition()->x + 30.0f, m_pController->GetPosition()->y + 30.0f, m_pController->GetPosition()->z + 30.0f);

	m_pP_Snow = new cParticle_Snow(boundingBox, 500);
	m_pP_Snow->Setup("Textures/snowflake.dds");


	boundingBox.min = D3DXVECTOR3(m_pController->GetPosition()->x - 5.0f, m_pController->GetPosition()->y - 0.0f, m_pController->GetPosition()->z - 5.0f);
	boundingBox.max = D3DXVECTOR3(m_pController->GetPosition()->x + 5.0f, m_pController->GetPosition()->y + 0.25f, m_pController->GetPosition()->z + 5.0f);

	m_pP_Walk = new cParticle_Walk(boundingBox, 10, *m_pController->GetPosition());
	m_pP_Walk->Setup("Textures/life.png");

	//����̵�
	SetTornado();
	/*===================================================================================*/

	//A*
	m_pAStar = new cAStar;
	m_pAStar->AutoRelease();
	m_pAStar->Setup(m_pMap);
	m_pMonsterController->SetAStar(m_pAStar);
	m_pMonsterController->Setup(&D3DXVECTOR3(62, 0, 105));

	//�������� �ø�
	m_pFrustum = new cFrustum;
	m_pFrustum->AutoRelease();
	m_pFrustum->Setup();

	/*
	//////////// �浹 ���� �ʱ�ȭ ////////////
	*/
	SetPortal();				//��Ż ����
	SetMapObjBoundingBox();		//ĳ���� X �ʿ�����Ʈ �浹üũ�� �ٿ���ڽ� ����
	SetButtonBoundingSphere();	//ĳ���� X ��ư �浹üũ�� �ٿ����ü ����
	/*
	//////////////////////////////////////////
	*/

	//������
	for (int i = 0; i < m_vecBoundingSphereButton.size(); ++i)
	{
		cSemicircleFX* pSemicircleFX = new cSemicircleFX;
		pSemicircleFX->Setup();
		pSemicircleFX->SetPosition(
			m_vecBoundingSphereButton[i].stSphere.vCenter.x,
			m_vecBoundingSphereButton[i].stSphere.vCenter.y,
			m_vecBoundingSphereButton[i].stSphere.vCenter.z);
		m_vecSemicircleFX.push_back(pSemicircleFX);
	}

	//����
	//Stage1 BGM ����
	g_pSoundManager->stop("./Sound/Stage1/mushroom dance_0.mp3");
	//Ÿ��Ʋ BGM ����
	g_pSoundManager->stop("./Sound/Title/one_0.mp3");

	//BossMap BGM ���
	g_pSoundManager->addSound("./Sound/BossMap/FranticLevel.wav", "./Sound/BossMap/FranticLevel.wav", true, true);
	g_pSoundManager->play("./Sound/BossMap/FranticLevel.wav", 0.7f);

	return S_OK;
}

void cBossMap::Release()
{
	SAFE_RELEASE(m_BoxMesh);
	SAFE_RELEASE(m_FogEffect);
	m_vecBoundingBox.clear();
	m_vecBoundingSphereButton.clear();
	m_vecSnowBall.clear();
	m_vecSemicircleFX.clear();
	m_vecTornado.clear();
	m_vecAchieveBox.clear();

	m_pP_Snow->Destroy();
	m_pP_Walk->Destroy();
}

void cBossMap::update()
{
	if (!m_pShowMenu->GetIsShowMenu())
	{
		//ĳ���� ��Ʈ�ѷ�
		if (m_pController)
		{
			m_pController->Update(m_pCamera->GetfRotationY());
			m_pController->SlideSlopeStage(m_pController->GetPosition(), 5.0f);	//��� �̲�����
		}

		//ī�޶�
		if (m_pCamera)
			m_pCamera->Update(m_pController->GetPosition());

		//���� ��Ʈ�ѷ�
		if (m_pMonsterController)
			m_pMonsterController->Update(m_pController->GetPosition(), m_pController->GetCharacterState());

		//ĳ���� ��Ų��޽�
		if (m_pSkinnedCharacter)
		{
			D3DXMATRIXA16 matR, matW;
			D3DXMatrixRotationY(&matR, D3DX_PI);
			matW = matR * *m_pController->GetWorldTM();
			m_pSkinnedCharacter->SetCharacterState(m_pController->GetCharacterState());
			m_pSkinnedCharacter->SetCharacterPosition(m_pController->GetPosition());
			m_pSkinnedCharacter->Update(&matW);
			m_pController->SetTrackPeriod(m_pSkinnedCharacter->GetTrackPeriod());
		}

		//���� ��Ų��޽�
		if (m_pSkinnedMonster)
		{
			if (m_pMonsterController->GetIsDiePlay())
			{
				D3DXMATRIXA16 matR, matW;
				D3DXMatrixRotationY(&matR, D3DX_PI);
				matW = matR * *m_pMonsterController->GetWorldTM();
				m_pSkinnedMonster->SetCharacterState(m_pMonsterController->GetCharacterState());
				m_pSkinnedMonster->SetCharacterPosition(m_pMonsterController->GetPosition());
				m_pSkinnedMonster->Update(&matW);	//�޽�������(���� �޽��� ���� �׷����� ��ġ ����)
				m_pSkinnedMonster->SetCharControllerPos(m_pController->GetPosition());
				m_pMonsterController->SetTrackPeriod(m_pSkinnedMonster->GetTrackPeriod());
			}
		}

		//����̵�
		for (int i = 0; i < m_vecTornadoBarrier.size(); ++i)
		{
			m_vecTornadoBarrier[i]->Update();
		}
		for (int i = 0; i < m_vecTornado.size(); ++i)
		{
			m_vecTornado[i]->Update();
		}

	}
	m_pCamera->SetIsShowMenu(m_pShowMenu->GetIsShowMenu());


	//�������� �ø�
	if (m_pFrustum)
		m_pFrustum->Update();

	/*
	//////////// �浹 ���� ������Ʈ �Լ� ////////////
	*/
	CollisionCharacter_MapObj();		//ĳ���� X �� ������Ʈ �浹 ó��
	CollisionCharacter_Button();		//ĳ���� X ��ư �浹 ó��
	UpdateTornado();					//����̵� ���� ������Ʈ �� �浹ó��
	CollisionCharacter_Monster();		//ĳ���� X ���� �浹ó��
	CollisionMonster_Character();		//���� X ĳ���� �浹ó��
	CollisionMonster_MapObj();			//���� X �� ������Ʈ �浹ó�� �Լ�

	UpdateSnowBallBoundingPosition();	//����ġ �浹���� �ٿ����ü ������ ������Ʈ
	CollisionSnowBall_MapObj();			//����ġ X �� ������Ʈ �浹ó��
	CollisionSnowBall_Button();			//����ġ X ��ư �浹ó��
	CollisionSnowBall_Monster();		//����ġ X ���� �浹ó��
	CollisionSnowBall_SnowBall();		//����ġ X ����ġ �浹ó��
	/*
	/////////////////////////////////////////////////
	*/

	/*===================================================================================*/
	/*                                New Source, This Area                              */
	/*===================================================================================*/
	if (IsOpenPortal())
	{
		//���� �߰�
		if (m_nEffectSoundPorta == 0 && !g_pSoundManager->isPlaySound("fanfare"))
		{
			g_pSoundManager->play("fanfare", 0.3f);		//�߰�
			++m_nEffectSoundPorta;
		}

		//��Ż ����Ʈ
		if (m_pTeleporter)
			m_pTeleporter->Update();

		if (m_boundingBoxPortal.IsPointIn(*m_pController->GetPosition()) ||
			g_pKeyManager->isOnceKeyDown('0'))		//��Ż�����ϰų� 0�� ������ ����ȯ
			m_pLoadingScene->LoadingScene("EndingScene");//Village���� ��ȯ
	}
	else
	{
		m_nEffectSoundPorta = 0;
		if (g_pKeyManager->isOnceKeyDown('0'))		//0�� ������ ����ȯ
			m_pLoadingScene->LoadingScene("Village");//Village���� ��ȯ
	}

	if (m_pLoadingScene)
		m_pLoadingScene->Update();

	if (m_pSinWave)
		m_pSinWave->Update();

	if (m_pP_Snow)
	{
		BoundingBox boundingBox;
		boundingBox.min = D3DXVECTOR3(m_pController->GetPosition()->x - 15.0f, m_pController->GetPosition()->y - 15.0f, m_pController->GetPosition()->z - 15.0f);
		boundingBox.max = D3DXVECTOR3(m_pController->GetPosition()->x + 15.0f, m_pController->GetPosition()->y + 15.0f, m_pController->GetPosition()->z + 15.0f);

		m_pP_Snow->SetBoundingBox(boundingBox);
		m_pP_Snow->Update();
	}

	if (m_pP_Walk)
	{
		BoundingBox boundingBox;
		boundingBox.min = D3DXVECTOR3(m_pController->GetPosition()->x - 2.0f, m_pController->GetPosition()->y - 0.0f, m_pController->GetPosition()->z - 2.0f);
		boundingBox.max = D3DXVECTOR3(m_pController->GetPosition()->x + 2.0f, m_pController->GetPosition()->y + 0.25f, m_pController->GetPosition()->z + 2.0f);

		m_pP_Walk->SetBoundingBox(boundingBox);
		m_pP_Walk->SetPlayerPosition(*m_pController->GetPosition());
		m_pP_Walk->Update();
	}

	//������
	for (int i = 0; i < m_vecSemicircleFX.size(); ++i)
	{
		m_vecSemicircleFX[i]->SetRotationY(D3DX_PI / 5 * g_pTimeManager->getElapsedTime());
	}

	//�޴�
	if (m_pShowMenu)
		m_pShowMenu->Update();

	if (m_pShowClear)
	{
		if (m_isOpenPortal)
			m_pShowClear->SetUnlock(true);
		m_pShowClear->Update();
	}

	if (m_pShowYouDie)
	{
		if (m_pController->GetCharacterState() == E_DEATH1)
		{
			m_pShowYouDie->SetisDie(true);

			for (int i = 0; i < m_vecBoundingSphereButton.size(); ++i)
			{
				m_vecBoundingSphereButton[i].isClear = false;
				m_vecBoundingSphereButton[i].nEndurance = 3;			
			}
			for (int i = 0; i < m_vecAchieveBox.size(); ++i)
			{
				m_vecAchieveBox[i]->SetisisQuestSuccess(false);
				m_vecAchieveBox[i]->SetPlayCnt(0);
			}
		}
		m_pShowYouDie->Update();

		if (m_pShowYouDie->GetIsRevival())
		{
			//ĳ���� ���� �ʱ�ȭ
			m_pController->Revival(m_pController->GetFirstPos());
			m_pShowYouDie->SetIsRevival(false);
			m_pShowYouDie->SetisDie(false);
			m_pShowYouDie->SetStackTime(0.0f);
			m_vecSnowBall.clear();

			m_isOpenPortal = false;
			m_pShowClear->SetUnlock(false);
			m_pShowClear->SetPlayCnt(0);

			//���� ���� �ʱ�ȭ
			m_pMonsterController->Reset();
		}
	}

	//��������UI
	m_pShowStageNum->Update();

	//�޼�UI
	if (m_pMonsterController->IsDeath())
		m_vecAchieveBox[5]->SetisisQuestSuccess(true);

	for (int i = 0; i < m_vecAchieveBox.size(); ++i)
		m_vecAchieveBox[i]->Update();
	/*===================================================================================*/
}

void cBossMap::render()
{
	g_pD3DDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00CCCCCC, 1.0f, 0);

	m_FogEffect->SetTechnique(m_FogTechHandle);

	UINT numPasses = 0;
	m_FogEffect->Begin(&numPasses, 0);
	m_FogEffect->BeginPass(0);

	/*===================================================================================*/
	/*                                New Source, This Area                              */
	/*===================================================================================*/
	//SetStreamSource �Ǿ� �ֱ� ������, SkyBox���� �׷����Ѵ�. (���� �ڷΰ��� UI ©��)
	if (m_pSkyBox)
		m_pSkyBox->render();

	if (m_pLoadMap)
		m_pLoadMap->Render();

	if (m_pLoadingScene)
		m_pLoadingScene->Render();

	if (m_pSinWave)
		m_pSinWave->Render();

	//�ĵ���
	if (m_pWaveWall)
		m_pWaveWall->Render();

	if (m_pP_Snow)
		m_pP_Snow->Render();

	if (m_pP_Walk)
		m_pP_Walk->Render();

	/*if (m_pSemicircleFX)
		m_pSemicircleFX->Render();*/
	/*===================================================================================*/

	//ĳ���� ��Ʈ�ѷ�
	if (m_pController)
		m_pController->Render();

	//���� ��Ʈ�ѷ�
	if (m_pMonsterController)
		m_pMonsterController->Render();

	//ĳ���� ��Ų��޽�
	if (m_pSkinnedCharacter)
		m_pSkinnedCharacter->Render();

	//���� ��Ų��޽�
	if (m_pSkinnedMonster)
		m_pSkinnedMonster->Render();

	//������
	for (int i = 0; i < m_vecSemicircleFX.size(); ++i)
	{
		if (m_vecBoundingSphereButton[i].nEndurance > 0)
			m_vecSemicircleFX[i]->Render();
	}

	//����̵�
	for (int i = 0; i < m_vecTornado.size(); ++i)
		m_vecTornado[i]->Render();
	for (int i = 0; i < m_vecTornadoBarrier.size(); ++i)
		m_vecTornadoBarrier[i]->Render();

	if (m_pShowClear && !m_pShowMenu->GetIsShowMenu())
		m_pShowClear->Render();

	if (m_pShowYouDie && !m_pShowMenu->GetIsShowMenu())
		m_pShowYouDie->Render();

	//�޴� ����
	if (m_pShowMenu)
		m_pShowMenu->Render();

	//��������UI
	m_pShowStageNum->Render();

	//�޼�UI
	for (int i = 0; i < m_vecAchieveBox.size(); ++i)
		m_vecAchieveBox[i]->Render();

	//��Ż
	RenderOpenPortal();

	m_FogEffect->EndPass();
	m_FogEffect->End();

	///////////////////����� ����/////////////////////////
	DebugRender();

	//A* �߰�
	//if (m_pAStar)
	//	m_pAStar->DebugRender();
	///////////////////////////////////////////////////////
}

void cBossMap::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (m_pCamera)
		m_pCamera->WndProc(hWnd, message, wParam, lParam);

	if (m_pController)
		m_pController->WndProc(hWnd, message, wParam, lParam);
}

//����̵� ���� �Լ�
void cBossMap::SetTornado()
{
	cTornado* pTornado = new cTornado;
	pTornado->Setup(&D3DXVECTOR3(36.13, 14.7f, 37.11));
	m_vecTornadoBarrier.push_back(pTornado);

	cTornado* pTornado2 = new cTornado;
	pTornado2->Setup(&D3DXVECTOR3(36.13, 14.7f, 39.11));
	m_vecTornadoBarrier.push_back(pTornado2);

	cTornado* pTornado3 = new cTornado;
	pTornado3->Setup(&D3DXVECTOR3(36.13, 14.7f, 41.11));
	m_vecTornadoBarrier.push_back(pTornado3);

	cTornado* pTornado4 = new cTornado;
	pTornado4->Setup(&D3DXVECTOR3(36.13, 14.7f, 43.11));
	m_vecTornadoBarrier.push_back(pTornado4);

	cTornado* pTornado5 = new cTornado;
	pTornado5->Setup(&D3DXVECTOR3(36.13, 14.7f, 45.11));
	m_vecTornadoBarrier.push_back(pTornado5);

	cTornado* pTornado6 = new cTornado;
	pTornado6->Setup(&D3DXVECTOR3(36.13, 14.7f, 47.11));
	m_vecTornadoBarrier.push_back(pTornado6);

	////////////////////////////////////////////////////

	cTornado* pTornado7 = new cTornado;
	pTornado7->Setup(&D3DXVECTOR3(11.54, 14.7f, 88.12), &D3DXVECTOR3(11.54, 14.7f, 100.78));
	m_vecTornado.push_back(pTornado7);

	cTornado* pTornado8 = new cTornado;
	pTornado8->Setup(&D3DXVECTOR3(24.72, 14.7f, 120.26), &D3DXVECTOR3(24.72, 14.7f, 105.3));
	m_vecTornado.push_back(pTornado8);

	cTornado* pTornado9 = new cTornado;
	pTornado9->Setup(&D3DXVECTOR3(113.63, 14.7f, 98.74), &D3DXVECTOR3(113.63, 14.7f, 87.39));
	m_vecTornado.push_back(pTornado9);

	cTornado* pTornado10 = new cTornado;
	pTornado10->Setup(&D3DXVECTOR3(94.84, 14.7f, 102.28), &D3DXVECTOR3(94.84, 14.7f, 114.87));
	m_vecTornado.push_back(pTornado10);
}

/*
//////////// ��Ż ���� �Լ� ////////////
*/
//��Ż ���� �Լ�
void cBossMap::SetPortal()
{
	m_isOpenPortal = false;
	m_nEffectSoundPorta = 0;

	m_boundingBoxPortal.min = D3DXVECTOR3(63.27, 5, 123.06);
	m_boundingBoxPortal.max = D3DXVECTOR3(65.37, 9, 125.16);

	// ��Ż ����Ʈ
	m_pTeleporter = new cTeleporter;
	m_pTeleporter->Setup();
	D3DXVECTOR3 vPortalPos = (m_boundingBoxPortal.min + m_boundingBoxPortal.max) / 2;
	m_pTeleporter->SetPosition(vPortalPos.x, vPortalPos.y, vPortalPos.z);
}

//��Ż ���ȴ��� Ȯ�� �Լ�
bool cBossMap::IsOpenPortal()
{
	for (int i = 0; i < m_vecBoundingSphereButton.size(); ++i)
	{
		if (!m_pMonsterController->GetIsDead()|| !m_vecBoundingSphereButton[i].isClear)
		{
			return false;
		}
	}
	m_isOpenPortal = true;
	return true;
}

//��Ż ��� �Լ�
void cBossMap::RenderOpenPortal()
{
	if (IsOpenPortal())
	{
		//��Ż ����Ʈ
		if (m_pTeleporter)
			m_pTeleporter->Render();
	}
}
/*
////////////////////////////////////////
*/

/*
//////////// �浹 ���� �Լ� ////////////
*/
//�� ������Ʈ �浹���� �ٿ���ڽ� ���� �Լ�
void cBossMap::SetMapObjBoundingBox()
{
	BoundingBox boundingBox;

	//ū����
	boundingBox.min = D3DXVECTOR3(73.23, 5, 11.38);
	boundingBox.max = D3DXVECTOR3(79.75, 11, 16.74);
	m_vecBoundingBox.push_back(boundingBox);

	//��������1
	boundingBox.min = D3DXVECTOR3(75.62, 5, 22.56);
	boundingBox.max = D3DXVECTOR3(78.55, 11, 25.25);
	m_vecBoundingBox.push_back(boundingBox);

	//��������2
	boundingBox.min = D3DXVECTOR3(51.54, 5, 2.67);
	boundingBox.max = D3DXVECTOR3(54.43, 11, 5.2);
	m_vecBoundingBox.push_back(boundingBox);

	//��������3
	boundingBox.min = D3DXVECTOR3(52.58, 5, 6.59);
	boundingBox.max = D3DXVECTOR3(55.38, 11, 9.39);
	m_vecBoundingBox.push_back(boundingBox);

	//��������4
	boundingBox.min = D3DXVECTOR3(48.67, 5, 11.48);
	boundingBox.max = D3DXVECTOR3(51.51, 11, 14.36);
	m_vecBoundingBox.push_back(boundingBox);

	//��������5
	boundingBox.min = D3DXVECTOR3(75.57, 5, 45.45);
	boundingBox.max = D3DXVECTOR3(78.42, 11, 48.33);
	m_vecBoundingBox.push_back(boundingBox);

	//��������6
	boundingBox.min = D3DXVECTOR3(82.59, 5, 40.81);
	boundingBox.max = D3DXVECTOR3(85.48, 11, 43.18);
	m_vecBoundingBox.push_back(boundingBox);

	//��������7
	boundingBox.min = D3DXVECTOR3(92.65, 5, 45.45);
	boundingBox.max = D3DXVECTOR3(95.42, 11, 48.26);
	m_vecBoundingBox.push_back(boundingBox);

	//��������8
	boundingBox.min = D3DXVECTOR3(115.6, 5, 59.48);
	boundingBox.max = D3DXVECTOR3(118.4, 11, 62.26);
	m_vecBoundingBox.push_back(boundingBox);

	//��������9
	boundingBox.min = D3DXVECTOR3(116.7, 5, 68.5);
	boundingBox.max = D3DXVECTOR3(119.36, 11, 71.2);
	m_vecBoundingBox.push_back(boundingBox);

	//��������10
	boundingBox.min = D3DXVECTOR3(111.67, 5, 71.61);
	boundingBox.max = D3DXVECTOR3(114.38, 11, 74.19);
	m_vecBoundingBox.push_back(boundingBox);

	//��������11
	boundingBox.min = D3DXVECTOR3(43.52, 5, 34.72);
	boundingBox.max = D3DXVECTOR3(46.33, 11, 37.28);
	m_vecBoundingBox.push_back(boundingBox);

	//��������12
	boundingBox.min = D3DXVECTOR3(17.62, 5, 40.54);
	boundingBox.max = D3DXVECTOR3(20.42, 11, 43.33);
	m_vecBoundingBox.push_back(boundingBox);

	//��������13
	boundingBox.min = D3DXVECTOR3(13.66, 5, 47.53);
	boundingBox.max = D3DXVECTOR3(16.39, 11, 50.3);
	m_vecBoundingBox.push_back(boundingBox);

	//��������14
	boundingBox.min = D3DXVECTOR3(12.8, 5, 58.45);
	boundingBox.max = D3DXVECTOR3(15.39, 11, 61.23);
	m_vecBoundingBox.push_back(boundingBox);

	//��������15
	boundingBox.min = D3DXVECTOR3(28.5, 5, 58.56);
	boundingBox.max = D3DXVECTOR3(31.45, 11, 61.26);
	m_vecBoundingBox.push_back(boundingBox);

	//��������16
	boundingBox.min = D3DXVECTOR3(15.6, 5, 68.46);
	boundingBox.max = D3DXVECTOR3(18.4, 11, 71.33);
	m_vecBoundingBox.push_back(boundingBox);

	//��������17
	boundingBox.min = D3DXVECTOR3(30.67, 5, 79.56);
	boundingBox.max = D3DXVECTOR3(33.47, 11, 82.28);
	m_vecBoundingBox.push_back(boundingBox);

	//��������18
	boundingBox.min = D3DXVECTOR3(27.62, 5, 87.51);
	boundingBox.max = D3DXVECTOR3(30.34, 11, 90.3);
	m_vecBoundingBox.push_back(boundingBox);

	//��������19
	boundingBox.min = D3DXVECTOR3(15.64, 5, 94.46);
	boundingBox.max = D3DXVECTOR3(18.49, 11, 97.35);
	m_vecBoundingBox.push_back(boundingBox);

	//��������20
	boundingBox.min = D3DXVECTOR3(43.51, 5, 102.76);
	boundingBox.max = D3DXVECTOR3(46.47, 11, 105.22);
	m_vecBoundingBox.push_back(boundingBox);

	//��������21
	boundingBox.min = D3DXVECTOR3(38.54, 5, 104.82);
	boundingBox.max = D3DXVECTOR3(41.47, 11, 107.23);
	m_vecBoundingBox.push_back(boundingBox);

	//��������22
	boundingBox.min = D3DXVECTOR3(32.53, 5, 106.7);
	boundingBox.max = D3DXVECTOR3(35.48, 11, 109.29);
	m_vecBoundingBox.push_back(boundingBox);

	//��������23
	boundingBox.min = D3DXVECTOR3(23.56, 5, 120.65);
	boundingBox.max = D3DXVECTOR3(26.51, 11, 123.37);
	m_vecBoundingBox.push_back(boundingBox);

	//��������24
	boundingBox.min = D3DXVECTOR3(9.6, 5, 114.52);
	boundingBox.max = D3DXVECTOR3(12.41, 11, 117.35);
	m_vecBoundingBox.push_back(boundingBox);

	//��������25
	boundingBox.min = D3DXVECTOR3(47.54, 5, 67.58);
	boundingBox.max = D3DXVECTOR3(50.35, 11, 70.25);
	m_vecBoundingBox.push_back(boundingBox);

	//��������26
	boundingBox.min = D3DXVECTOR3(71.64, 5, 73.62);
	boundingBox.max = D3DXVECTOR3(74.47, 11, 76.3);
	m_vecBoundingBox.push_back(boundingBox);

	//��������27
	boundingBox.min = D3DXVECTOR3(80.57, 5, 66.57);
	boundingBox.max = D3DXVECTOR3(83.43, 11, 69.27);
	m_vecBoundingBox.push_back(boundingBox);

	//��������28
	boundingBox.min = D3DXVECTOR3(89.62, 5, 95.67);
	boundingBox.max = D3DXVECTOR3(92.35, 11, 98.35);
	m_vecBoundingBox.push_back(boundingBox);

	//��������29
	boundingBox.min = D3DXVECTOR3(99.45, 5, 84.57);
	boundingBox.max = D3DXVECTOR3(102.34, 11, 87.32);
	m_vecBoundingBox.push_back(boundingBox);

	//��������30
	boundingBox.min = D3DXVECTOR3(83.56, 5, 105.54);
	boundingBox.max = D3DXVECTOR3(86.45, 11, 108.33);
	m_vecBoundingBox.push_back(boundingBox);

	//��������31
	boundingBox.min = D3DXVECTOR3(96.57, 5, 117.5);
	boundingBox.max = D3DXVECTOR3(99.37, 11, 120.36);
	m_vecBoundingBox.push_back(boundingBox);

	//��������32
	boundingBox.min = D3DXVECTOR3(109.64, 5, 110.45);
	boundingBox.max = D3DXVECTOR3(112.44, 11, 113.38);
	m_vecBoundingBox.push_back(boundingBox);

	//��������33
	boundingBox.min = D3DXVECTOR3(109.56, 5, 122.54);
	boundingBox.max = D3DXVECTOR3(112.43, 11, 125.41);
	m_vecBoundingBox.push_back(boundingBox);

	D3DXCreateBox(
		g_pD3DDevice,
		m_boundingBoxPortal.max.x - m_boundingBoxPortal.min.x,
		m_boundingBoxPortal.max.y - m_boundingBoxPortal.min.y,
		m_boundingBoxPortal.max.z - m_boundingBoxPortal.min.z,
		&m_BoxMesh,
		0);
}

//ĳ���� X �ʿ�����Ʈ �浹ó�� �Լ�
void cBossMap::CollisionCharacter_MapObj()
{
	for (int i = 0; i < m_vecBoundingBox.size(); ++i)
	{
		if (m_vecBoundingBox[i].IsPointIn(*m_pController->GetPosition()))
		{
			m_pController->SetIsCollision(true);

			D3DXVECTOR3 vBoxPos = (m_vecBoundingBox[i].min + m_vecBoundingBox[i].max) / 2;
			vBoxPos.y = m_pController->GetPosition()->y;

			D3DXVECTOR3 vDir = *m_pController->GetPosition() - vBoxPos;
			D3DXVec3Normalize(&vDir, &vDir);

			*m_pController->GetPosition() = *m_pController->GetPosition() + vDir * m_pController->GetMoveSpeed() * g_pTimeManager->getElapsedTime();
		}
		else m_pController->SetIsCollision(false);
	}
}

//��ư �浹���� �ٿ����ü ���� �Լ�
void cBossMap::SetButtonBoundingSphere()
{
	ST_SPHERE stSphere;
	ST_BUTTON stButton;

	//��ư1
	stSphere.fRadius = 1.1f;
	stSphere.vCenter = D3DXVECTOR3(111.39, 5, 67.78);
	stSphere.isInFrustum = false;
	stSphere.isPicked = false;
	stButton.stSphere = stSphere;
	stButton.isClear = false;
	stButton.isHasBarrier = true;
	stButton.nEndurance = 3;
	m_vecBoundingSphereButton.push_back(stButton);

	//��ư2
	stSphere.vCenter = D3DXVECTOR3(5.02, 5, 92.96);
	stButton.stSphere = stSphere;
	m_vecBoundingSphereButton.push_back(stButton);

	//��ư3
	stSphere.vCenter = D3DXVECTOR3(18, 5, 118.95);
	stButton.stSphere = stSphere;
	m_vecBoundingSphereButton.push_back(stButton);

	//��ư4
	stSphere.vCenter = D3DXVECTOR3(121.98, 5, 93);
	stButton.stSphere = stSphere;
	m_vecBoundingSphereButton.push_back(stButton);

	//��ư5
	stSphere.vCenter = D3DXVECTOR3(106, 5, 117);
	stButton.stSphere = stSphere;
	m_vecBoundingSphereButton.push_back(stButton);
}

//ĳ���� X ��ư �浹ó�� �Լ�
void cBossMap::CollisionCharacter_Button()
{
	for (int i = 0; i < m_vecBoundingSphereButton.size(); ++i)
	{
		D3DXVECTOR3 vDir = *m_pController->GetPosition() - m_vecBoundingSphereButton[i].stSphere.vCenter;
		float fDis = D3DXVec3Length(&vDir);

		if (m_vecBoundingSphereButton[i].nEndurance <= 0)
		{
			if (fDis <= m_vecBoundingSphereButton[i].stSphere.fRadius +
				m_pSkinnedCharacter->GetCollisionSphere()->m_fRadiusHead)
			{
				m_pController->SetIsCollision(true);

				if (m_pController->GetCharacterDirState() != E_NOTMOVE)
				{
					D3DXVec3Normalize(&vDir, &vDir);
					*m_pController->GetPosition() = *m_pController->GetPosition() + vDir * m_pController->GetMoveSpeed() * g_pTimeManager->getElapsedTime();
				}

				break;
			}
			else
			{
				m_pController->SetIsCollision(false);
			}
		}
		else
		{
			if (fDis <= m_vecBoundingSphereButton[i].stSphere.fRadius +
				m_pSkinnedCharacter->GetCollisionSphere()->m_fRadiusHead + 1.0f)
			{
				m_pController->SetIsCollision(true);

				if (m_pController->GetCharacterDirState() != E_NOTMOVE)
				{
					D3DXVec3Normalize(&vDir, &vDir);
					*m_pController->GetPosition() = *m_pController->GetPosition() + vDir * m_pController->GetMoveSpeed() * g_pTimeManager->getElapsedTime();
				}

				break;
			}
			else
			{
				m_pController->SetIsCollision(false);
			}
		}
	}
}

//����̵� ���� ������Ʈ �Լ�
void cBossMap::UpdateTornado()
{
	if (m_vecBoundingSphereButton[0].isClear)
	{
		m_vecTornadoBarrier.clear();
	}

	CollisionCharacter_Tornado();
}

//ĳ���� X ����̵� �浹ó�� �Լ�
void cBossMap::CollisionCharacter_Tornado()
{
	if (m_pController->IsDeath()) return;
	
	for (int i = 0; i < m_vecTornado.size(); ++i)
	{
		D3DXVECTOR3 vTornadoPos = (m_vecTornado[i]->GetBoundingBox().min + m_vecTornado[i]->GetBoundingBox().max) / 2;
		vTornadoPos.y = m_pController->GetPosition()->y;
		D3DXVECTOR3 vDir = *m_pController->GetPosition() - vTornadoPos;
		float fDis = D3DXVec3Length(&vDir);
		D3DXVec3Normalize(&vDir, &vDir);

		if (fDis <= m_pSkinnedCharacter->GetCollisionSphere()->m_fRadiusAll + 0.5f)
		{
			if (!m_pController->GetIsFallByTornado() && !m_pController->GetIsCollisionTornado())
			{
				m_pController->SetIsColiisionTornado(true);
				m_pController->SetStackDeltaTime(0.0f);
				m_pController->SetIsHit(true);
			}
		}
	}

	for (int i = 0; i < m_vecTornadoBarrier.size(); ++i)
	{
		D3DXVECTOR3 vTornadoPos = (m_vecTornadoBarrier[i]->GetBoundingBox().min + 
			m_vecTornadoBarrier[i]->GetBoundingBox().max) / 2;
		vTornadoPos.y = m_pController->GetPosition()->y;
		D3DXVECTOR3 vDir = *m_pController->GetPosition() - vTornadoPos;
		float fDis = D3DXVec3Length(&vDir);
		D3DXVec3Normalize(&vDir, &vDir);

		if (fDis <= m_pSkinnedCharacter->GetCollisionSphere()->m_fRadiusAll + 0.5f)
		{
			if (!m_pController->GetIsFallByTornado() && !m_pController->GetIsCollisionTornado())
			{
				m_pController->SetIsColiisionTornado(true);
				m_pController->SetStackDeltaTime(0.0f);
				m_pController->SetIsHit(true);
			}
		}
	}
}

//ĳ���� X ���� �浹ó�� �Լ�(���� Hit)
void cBossMap::CollisionCharacter_Monster()
{
	D3DXVECTOR3 vDirMAll = m_pSkinnedCharacter->GetCollisionSphere()->m_vPositionAll - m_pSkinnedMonster->GetCollisionSphere()->m_vPositionAll;
	float fDisMAll = D3DXVec3Length(&vDirMAll);
	D3DXVec3Normalize(&vDirMAll, &vDirMAll);

	D3DXVECTOR3 vDirMHead = m_pSkinnedCharacter->GetCollisionSphere()->m_vPositionAll - m_pSkinnedMonster->GetCollisionSphere()->m_vPositionHead;
	float fDisMHead = D3DXVec3Length(&vDirMHead);
	D3DXVec3Normalize(&vDirMHead, &vDirMHead);

	D3DXVECTOR3 vDirMBody = m_pSkinnedCharacter->GetCollisionSphere()->m_vPositionAll - m_pSkinnedMonster->GetCollisionSphere()->m_vPositionBody;
	float fDisMBody = D3DXVec3Length(&vDirMBody);
	D3DXVec3Normalize(&vDirMBody, &vDirMBody);

	if (fDisMAll <= m_pSkinnedMonster->GetCollisionSphere()->m_fRadiusAll +
		m_pSkinnedCharacter->GetCollisionSphere()->m_fRadiusAll)
	{
		static int cntMonsterHit = 0;
		static int resetTimeCnt = 0;

		if (m_pController->GetIsAttack() && cntMonsterHit == 0)
		{
			m_pMonsterController->SetIsHit(true);

			if (resetTimeCnt == 0)
			{
				m_pMonsterController->SetIsResetStackDeltaTime(true);
				++resetTimeCnt;
			}

			++cntMonsterHit;
		}
		else if (!m_pController->GetIsAttack())
		{
			cntMonsterHit = 0;
			resetTimeCnt = 0;
		}
	}

	static int cntMonsterJumpHit = 0;
	static int resetTimeCnt2 = 0;

	if (m_pController->GetCharacterState() == E_ATTACK2 &&
		fDisMHead <= m_pSkinnedMonster->GetCollisionSphere()->m_fRadiusHead +
		m_pSkinnedCharacter->GetCollisionSphere()->m_fRadiusAll)
	{

		if (cntMonsterJumpHit == 0)
		{
			m_pMonsterController->SetIsHit(true);
			++cntMonsterJumpHit;

			if (resetTimeCnt2 == 0)
			{
				m_pMonsterController->SetIsResetStackDeltaTime(true);
				++resetTimeCnt2;
			}
		}
	}
	else
	{
		cntMonsterJumpHit = 0;
		resetTimeCnt2 = 0;
	}

	if (fDisMBody <= m_pSkinnedMonster->GetCollisionSphere()->m_fRadiusBody +
						  m_pSkinnedCharacter->GetCollisionSphere()->m_fRadiusAll)
	{
		if (m_pMonsterController->GetCharacterState() == E_MONSTER_RUN_FRONT)
		{
			*m_pController->GetPosition() = *m_pController->GetPosition() + vDirMBody * SPEED_CHARGE * g_pTimeManager->getElapsedTime();
		}
		else
		{
			*m_pController->GetPosition() = *m_pController->GetPosition() + vDirMBody * m_pController->GetMoveSpeed() * g_pTimeManager->getElapsedTime();
		}
	}
	else if (fDisMHead <= m_pSkinnedMonster->GetCollisionSphere()->m_fRadiusHead +
		m_pSkinnedCharacter->GetCollisionSphere()->m_fRadiusAll - 0.2f)
	{
		if (m_pMonsterController->GetCharacterState() == E_MONSTER_RUN_FRONT)
		{
			*m_pController->GetPosition() = *m_pController->GetPosition() + vDirMHead * SPEED_CHARGE * g_pTimeManager->getElapsedTime();
		}
		else
		{
			*m_pController->GetPosition() = *m_pController->GetPosition() + vDirMHead * m_pController->GetMoveSpeed() * g_pTimeManager->getElapsedTime();
		}
	}
}

//���� X ĳ���� �浹ó�� �Լ�(ĳ���� Hit)
void cBossMap::CollisionMonster_Character()
{
	D3DXVECTOR3 vDirCAllWeapon = m_pSkinnedCharacter->GetCollisionSphere()->m_vPositionAll - m_pSkinnedMonster->GetCollisionWeapon()->m_vPosition;
	float fDisCAllWeapon = D3DXVec3Length(&vDirCAllWeapon);
	D3DXVec3Normalize(&vDirCAllWeapon, &vDirCAllWeapon);

	D3DXVECTOR3 vDirCAllCharge = m_pSkinnedCharacter->GetCollisionSphere()->m_vPositionAll - m_pSkinnedMonster->GetCollisionSphere()->m_vPositionAll;
	float fDisCAllCharge = D3DXVec3Length(&vDirCAllCharge);
	D3DXVec3Normalize(&vDirCAllCharge, &vDirCAllCharge);

	//�����⿡ ���� ���
	if (fDisCAllCharge <= m_pSkinnedMonster->GetCollisionSphere()->m_fRadiusAll +
		m_pSkinnedCharacter->GetCollisionSphere()->m_fRadiusHead)
	{
		if (m_pMonsterController->GetCharacterState() == E_MONSTER_RUN_FRONT)
		{
			m_pController->SetIsHit(true);
		}
	}
	//����̿� ���� ���
	else if (fDisCAllWeapon <= m_pSkinnedMonster->GetCollisionWeapon()->m_fRadius +
		m_pSkinnedCharacter->GetCollisionSphere()->m_fRadiusAll)
	{
		static int cntCharacterHit = 0;

		if (/*m_pMonsterController->GetIsAttack()*/
			(m_pMonsterController->GetCharacterState() == E_MONSTER_ATTACK1 ||
			m_pMonsterController->GetCharacterState() == E_MONSTER_ATTACK2 ||
			m_pMonsterController->GetCharacterState() == E_MONSTER_ATTACK3) && cntCharacterHit == 0)
		{
			m_pController->SetIsHit(true);
			++cntCharacterHit;
		}
		else if (!(m_pMonsterController->GetCharacterState() == E_MONSTER_ATTACK1 ||
			m_pMonsterController->GetCharacterState() == E_MONSTER_ATTACK2 ||
			m_pMonsterController->GetCharacterState() == E_MONSTER_ATTACK3))
		{
			cntCharacterHit = 0;
		}
	}
}

//���� X �� ������Ʈ �浹ó�� �Լ�
void cBossMap::CollisionMonster_MapObj()
{
	for (int i = 0; i < m_vecBoundingBox.size(); ++i)
	{
		if (m_vecBoundingBox[i].IsPointIn(*m_pMonsterController->GetPosition()))
		{
			m_pMonsterController->SetIsCollision(true);

			D3DXVECTOR3 vBoxPos = (m_vecBoundingBox[i].min + m_vecBoundingBox[i].max) / 2;
			vBoxPos.y = m_pMonsterController->GetPosition()->y;

			D3DXVECTOR3 vDir = *m_pMonsterController->GetPosition() - vBoxPos;
			D3DXVec3Normalize(&vDir, &vDir);

			if (!m_pMonsterController->GetCharacterState() == E_MONSTER_RUN_FRONT)
			{
				*m_pMonsterController->GetPosition() = *m_pMonsterController->GetPosition() + vDir * SPEED_WALK * g_pTimeManager->getElapsedTime();
			}
			else
			{
				*m_pMonsterController->GetPosition() = *m_pMonsterController->GetPosition() + vDir * SPEED_CHARGE * g_pTimeManager->getElapsedTime();
				/*m_pMonsterController->SetIsCharge(false);
				m_pMonsterController->SetIsHit(true);*/
			}
		}
		else m_pController->SetIsCollision(false);
	}
}

//����ġ �浹���� �ٿ����ü ������ ������Ʈ �Լ�
void cBossMap::UpdateSnowBallBoundingPosition()
{
	m_vecSnowBall = m_pController->GetVecSnowBall();
}

//����ġ X �� ������Ʈ �浹ó�� �Լ�
void cBossMap::CollisionSnowBall_MapObj()
{
	for (int i = 0; i < m_vecBoundingBox.size(); ++i)
	{
		for (int j = 0; j < m_vecSnowBall.size(); ++j)
		{
			if (m_vecBoundingBox[i].IsPointIn(m_vecSnowBall[j]->GetSphere().vCenter))
			{
				m_vecSnowBall[j]->SetIsCollision(true);

				if (m_vecSnowBall[j]->GetState() == E_SNOW_BIG)
					m_pController->SetBigSnowBallNum(m_pController->GetBigSnowBallNum() - 1);
			}
		}
	}
}

//����ġ X ��ư �浹ó�� �Լ�
void cBossMap::CollisionSnowBall_Button()
{
	if (m_isOpenPortal) return;
	
	for (int i = 0; i < m_vecBoundingSphereButton.size(); ++i)
	{
		if (m_vecBoundingSphereButton[i].isClear) continue;

		for (int j = 0; j < m_vecSnowBall.size(); ++j)
		{
			if (m_vecBoundingSphereButton[i].nEndurance <= 0)
			{
				if (m_vecSnowBall[j]->GetState() != E_SNOW_BIG) continue;
				if (m_vecSnowBall[j]->GetIsUsedForPortal()) continue;

				D3DXVECTOR3 vDis = m_vecBoundingSphereButton[i].stSphere.vCenter - m_vecSnowBall[j]->GetSphere().vCenter;
				float fDis = D3DXVec3Length(&vDis);

				if (fDis <= m_vecBoundingSphereButton[i].stSphere.fRadius + m_vecSnowBall[j]->GetSphere().fRadius)
				{
					m_vecBoundingSphereButton[i].isClear = true;
					m_vecSnowBall[j]->SetIsUsedForPortal(true);
					m_vecAchieveBox[i]->SetisisQuestSuccess(true);
					//m_pMonsterController->SetHp(m_pMonsterController->GetHp() - 10);
					m_pMonsterController->DamageOn();
				}
			}
			else
			{
				if (m_vecSnowBall[j]->GetState() == E_SNOW_PICKED) continue;

				D3DXVECTOR3 vDis = m_vecBoundingSphereButton[i].stSphere.vCenter - m_vecSnowBall[j]->GetSphere().vCenter;
				float fDis = D3DXVec3Length(&vDis);

				if (fDis <= m_vecBoundingSphereButton[i].stSphere.fRadius + m_vecSnowBall[j]->GetSphere().fRadius + 1.0f)
				{
					if (m_vecSnowBall[j]->GetState() == E_SNOW_BUILD)
					{
						m_vecSnowBall[j]->SetIsCollision(true);
					}
					else if (m_vecSnowBall[j]->GetState() == E_SNOW_BIG)
					{
						m_vecSnowBall[j]->SetIsCollision(true);
						m_pController->SetBigSnowBallNum(m_pController->GetBigSnowBallNum() - 1);
					}
					else if (m_vecSnowBall[j]->GetState() == E_SNOW_SMALL)
					{
						m_vecSnowBall[j]->SetIsCollision(true);
						--m_vecBoundingSphereButton[i].nEndurance;
					}
				}
			}
		}
	}
}

//����ġ X ���� �浹ó�� �Լ�
void cBossMap::CollisionSnowBall_Monster()
{
	for (int j = 0; j < m_vecSnowBall.size(); ++j)
	{
		if (m_vecSnowBall[j]->GetState() != E_SNOW_SMALL &&
			m_vecSnowBall[j]->GetState() != E_SNOW_BIG) continue;

		D3DXVECTOR3& vHeadPos = m_pSkinnedMonster->GetCollisionSphere()->m_vPositionHead;
		D3DXVECTOR3& vBodyPos = m_pSkinnedMonster->GetCollisionSphere()->m_vPositionBody;

		D3DXVECTOR3 vDirHead = vHeadPos - m_vecSnowBall[j]->GetSphere().vCenter;
		D3DXVECTOR3 vDirBody = vBodyPos - m_vecSnowBall[j]->GetSphere().vCenter;

		float fDisHead = D3DXVec3Length(&vDirHead);
		float fDisBody = D3DXVec3Length(&vDirBody);
		static int resetTimeCnt3 = 0;

		if (fDisHead <= m_vecSnowBall[j]->GetSphere().fRadius + m_pSkinnedMonster->GetCollisionSphere()->m_fRadiusHead || fDisBody <= m_vecSnowBall[j]->GetSphere().fRadius + m_pSkinnedMonster->GetCollisionSphere()->m_fRadiusBody)
		{
			m_vecSnowBall[j]->SetIsCollision(true);
			m_pMonsterController->SetIsHit(true);

			if (resetTimeCnt3 == 0)
			{
				m_pMonsterController->SetIsResetStackDeltaTime(true);
				++resetTimeCnt3;
			}
		}
		else resetTimeCnt3 = 0;
	}
}

//����ġ X ����ġ �浹ó�� �Լ�
void cBossMap::CollisionSnowBall_SnowBall()
{
	for (int i = 0; i < m_vecSnowBall.size(); ++i)
	{
		if (m_vecSnowBall[i]->GetState() == E_SNOW_PICKED) continue;

		for (int j = 0; j < m_vecSnowBall.size(); ++j)
		{
			if (i >= j) continue;
			if (m_vecSnowBall[j]->GetState() == E_SNOW_PICKED) continue;

			D3DXVECTOR3 vDir = m_vecSnowBall[i]->GetSphere().vCenter - m_vecSnowBall[j]->GetSphere().vCenter;
			float fDis = D3DXVec3Length(&vDir);

			if (fDis <= m_vecSnowBall[i]->GetSphere().fRadius + m_vecSnowBall[j]->GetSphere().fRadius)
			{
				if (m_vecSnowBall[i]->GetState() == E_SNOW_BIG && m_vecSnowBall[j]->GetState() == E_SNOW_BIG)
				{
					if (m_vecSnowBall[i]->GetIsUsedForPortal())
					{
						m_vecSnowBall[j]->SetIsCollision(true);
						m_pController->SetBigSnowBallNum(m_pController->GetBigSnowBallNum() - 1);
					}
					else if (m_vecSnowBall[j]->GetIsUsedForPortal())
					{
						m_vecSnowBall[i]->SetIsCollision(true);
						m_pController->SetBigSnowBallNum(m_pController->GetBigSnowBallNum() - 1);
					}
					else
					{
						m_vecSnowBall[i]->SetIsCollision(true);
						m_vecSnowBall[j]->SetIsCollision(true);
						m_pController->SetBigSnowBallNum(m_pController->GetBigSnowBallNum() - 2);
					}				
				}
				else if (m_vecSnowBall[i]->GetState() == E_SNOW_BIG)
				{
					m_vecSnowBall[j]->SetIsCollision(true);
				}
				else if (m_vecSnowBall[j]->GetState() == E_SNOW_BIG)
				{
					m_vecSnowBall[i]->SetIsCollision(true);
				}
			}
		}
	}
}
/*
///////////////////////////////////////
*/


//����� ���� �Լ�
void cBossMap::DebugRender()
{

	//�浹 ��ü: ĳ����, ����
	if (g_pKeyManager->isOnceKeyDown('1'))
	{
		m_isOnKey1 = !m_isOnKey1;
	}

	if (m_isOnKey1)
	{
		m_pSkinnedCharacter->DebugRender_Collision();
		m_pSkinnedMonster->DebugRender_Collision();
		DebugRender_Portal();

		for (int i = 0; i < m_vecTornado.size(); ++i)
		{
			m_vecTornado[i]->DebugRender_Tornado();
		}
	}
}

//��Ż ������Լ�
void cBossMap::DebugRender_Portal()
{
	char szStr[1024];
	if (m_isOpenPortal)
		sprintf(szStr, "\nisOpenPortal: true");
	else sprintf(szStr, "\nisOpenPortal: false");


	char szStr2[1024];
	if (m_vecBoundingSphereButton[0].isClear)
		sprintf(szStr2, "\nisButton1Clear: true");
	else sprintf(szStr2, "\nisButton1Clear: false");
	strcat(szStr, szStr2);

	if (m_vecBoundingSphereButton[1].isClear)
		sprintf(szStr2, "\nisButton2Clear: true");
	else sprintf(szStr2, "\nisButton2Clear: false");
	strcat(szStr, szStr2);

	if (m_vecBoundingSphereButton[2].isClear)
		sprintf(szStr2, "\nisButton3Clear: true");
	else sprintf(szStr2, "\nisButton3Clear: false");
	strcat(szStr, szStr2);

	if (m_vecBoundingSphereButton[3].isClear)
		sprintf(szStr2, "\nisButton4Clear: true");
	else sprintf(szStr2, "\nisButton4Clear: false");
	strcat(szStr, szStr2);

	if (m_vecBoundingSphereButton[4].isClear)
		sprintf(szStr2, "\nisButton5Clear: true");
	else sprintf(szStr2, "\nisButton5Clear: false");
	strcat(szStr, szStr2);

	m_pFont->DrawTextA(NULL, szStr, strlen(szStr),
		m_pRect, DT_TOP | DT_LEFT | DT_NOCLIP, D3DCOLOR_XRGB(255, 0, 0));

	//���̾�� �׸���
	g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, false);

	g_pD3DDevice->SetMaterial(&WHITE_MTRL);
	D3DXMATRIXA16 mat;
	D3DXVECTOR3 center = (m_boundingBoxPortal.min + m_boundingBoxPortal.max) / 2;
	D3DXMatrixTranslation(&mat, center.x, center.y, center.z);
	g_pD3DDevice->SetTransform(D3DTS_WORLD, &mat);
	m_BoxMesh->DrawSubset(0);

	g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, true);
}