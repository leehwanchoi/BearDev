#include "stdafx.h"
#include "cMainScene.h"

#include "cCamera.h"
#include "cCharacterController.h"
#include "cMonsterController.h"	//�߰�
#include "cNPCController.h"		//�߰�
#include "cHeightMap.h"
#include "cSkinnedMesh.h"
#include "cSkinnedCharacter.h"	//�߰�
#include "cSkinnedMonster.h"	//�߰�
#include "cAStar.h"		//�߰�
#include "cFrustum.h"	//�߰�

/*===================================================================================*/
/*                                New Header, This Area                              */
/*===================================================================================*/
#include "cSkyBox.h"
#include "cLoadMap.h"
/*===================================================================================*/

cMainScene::cMainScene()
	: m_pCamera(NULL)
	, m_pController(NULL)
	, m_pMonsterController(NULL)	
	, m_pNPCController(NULL)	
	, m_pNPCController2(NULL)		//�߰�
	, m_pNPCController3(NULL)		//�߰�
	, m_pSkinnedCharacter(NULL)		
	, m_pSkinnedMonster(NULL)		
	, m_pSkinnedNPC(NULL)		
	, m_pSkinnedNPC2(NULL)			//�߰�
	, m_pSkinnedNPC3(NULL)			//�߰�
	//�߰�
	, m_pXMap(NULL)
	, m_isShowXMap(false)
	//�߰�
	, m_pAStar(NULL)
	, m_pFrustum(NULL)
	//�߰�
	, m_isOnKey1(false)
	, m_isOnKey3(false)
	, m_isObjMap(false)
	/*===================================================================================*/
	/*                           New Initialization, This Area                           */
	/*===================================================================================*/
	, m_pSkyBox(NULL)  // ��ī�� �ڽ� �߰�
	, m_pLoadMap(NULL)
	/*===================================================================================*/
{
	srand(time(NULL));
	rand();

	//m_vecSkinnedNPC.clear();	//NPC ���� �ʱ�ȭ �߰�
}


cMainScene::~cMainScene()
{
	/*������ g_p ������� ���õ� �κ��� cMainGame�� Release�� �Ű�����.	*/
}

HRESULT cMainScene::init()
{
	g_pD3DDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);

	// �𷺼ų� ����Ʈ ���� �� ����
	D3DLIGHT9 light = InitDirectionalLight(&D3DXVECTOR3(1, -1, 0), &WHITE);
	g_pD3DDevice->SetLight(0, &light);
	g_pD3DDevice->LightEnable(0, true);

	//ī�޶�
	m_pCamera = new cCamera;
	m_pCamera->AutoRelease();
	m_pCamera->Setup();

	//ĳ���� ��Ʈ�ѷ�
	m_pController = new cCharacterController;
	m_pController->AutoRelease();

	//���� ��Ʈ�ѷ�
	m_pMonsterController = new cMonsterController;
	m_pMonsterController->AutoRelease();

	//NPC ��Ʈ�ѷ�
	m_pNPCController = new cNPCController;
	m_pNPCController->AutoRelease();
	m_pNPCController->Setup(&D3DXVECTOR3(5, 0, 5), D3DX_PI / 2, E_NPC_NOT_MOVE);
	m_pNPCController->SetTalkTextBox("Penguin", "Hi! Nice to meet you! What's your name?", false);

	//NPC2 ��Ʈ�ѷ� �߰�
	m_pNPCController2 = new cNPCController;
	m_pNPCController2->AutoRelease();
	m_pNPCController2->Setup(&D3DXVECTOR3(8, 0, 3), D3DX_PI / 2, E_NPC_MOVE_ONEWAY);
	m_pNPCController2->SetMoveDistance(5.0f);
	m_pNPCController2->SetTalkTextBox("Penguin2", ".........?", false);

	//NPC3 ��Ʈ�ѷ� �߰�
	m_pNPCController3 = new cNPCController;
	m_pNPCController3->AutoRelease();
	m_pNPCController3->Setup(&D3DXVECTOR3(3, 0, 8), D3DX_PI, E_NPC_MOVE_ONEWAY);
	m_pNPCController3->SetMoveDistance(5.0f);
	m_pNPCController3->SetTalkTextBox("Penguin3", "Krrrr", false);

	//��Ų�� �޽�
	m_pSkinnedCharacter = g_pSkinnedMeshManager->GetSkinnedMesh("character", "eskimo_01.X", E_CHARACTER);
	m_pSkinnedMonster = g_pSkinnedMeshManager->GetSkinnedMesh("character", "yeti.X", E_MONSTER);
	m_pSkinnedNPC = g_pSkinnedMeshManager->GetSkinnedMesh("character", "pinguine_01.X", E_CHARACTER);
	m_pSkinnedNPC2 = g_pSkinnedMeshManager->GetSkinnedMesh("character", "pinguine_02.X", E_CHARACTER);
	m_pSkinnedNPC3 = g_pSkinnedMeshManager->GetSkinnedMesh("character", "pinguine_03.X", E_CHARACTER);

	m_pSkinnedNPC->SetRandomTrackPosition();
	m_pSkinnedNPC2->SetRandomTrackPosition();
	m_pSkinnedNPC3->SetRandomTrackPosition();

	//NPC ���� ����
	/*cSkinnedMesh* pSkinMesh = new cSkinnedMesh("character", "pinguine_01.X", E_CHARACTER);
	pSkinMesh->AutoRelease();
	pSkinMesh->SetRandomTrackPosition();  // Į ���� ������
	m_vecSkinnedNPC.push_back(pSkinMesh);
	
	cSkinnedMesh* pSkinMesh2 = new cSkinnedMesh("character", "pinguine_02.X", E_CHARACTER);
	pSkinMesh2->AutoRelease();
	pSkinMesh2->SetRandomTrackPosition();  // Į ���� ������
	m_vecSkinnedNPC.push_back(pSkinMesh2);*/

	////////////////////////////////////// MAP ////////////////////////////////////////
	//Height��
	D3DXMATRIXA16 matS;
	D3DXMatrixScaling(&matS, 1.0f, 1.0f, 1.0f);
	m_pHeight = new cHeightMap;
	m_pHeight->AutoRelease();
	m_pHeight->Load("HeightMapData/HeightMap.raw", &matS);
	//m_pMap = m_pHeight;
	//m_pController->SetMap(m_pMap);
	//m_pMonsterController->SetMap(m_pMap);
	//m_pNPCController->SetMap(m_pMap);		//NPC��Ʈ�ѷ� �¸� �߰�

	//A*�߰�
	//m_pAStar = new cAStar;
	//m_pAStar->AutoRelease();
	//m_pAStar->Setup(m_pMap);
	//m_pMonsterController->SetAStar(m_pAStar);
	//m_pMonsterController->Setup();

	//�������� �ø�
	m_pFrustum = new cFrustum;
	m_pFrustum->AutoRelease();
	m_pFrustum->Setup();

	/*===================================================================================*/
	/*                                New Source, This Area                              */
	/*===================================================================================*/
	//��ī�̹ڽ� �߰� �� �ʱ�ȭ
	m_pSkyBox = new cSkyBox;
	m_pSkyBox->init();
	m_pSkyBox->AutoRelease();
	/*===================================================================================*/

	/*
	//�߰�: cLoadMap�� �̿��� �ʱ׸���
	*/
	///////////////////////////////////////////////////////////////////////
	m_pLoadMap = new cLoadMap;
	m_pLoadMap->AutoRelease();
	m_pLoadMap->RealLoad_MainScene();
	m_pMap = m_pLoadMap;
	m_pController->SetMap(m_pMap);
	//m_pMonsterController->SetMap(m_pMap);
	m_pNPCController->SetMap(m_pMap);
	m_pNPCController2->SetMap(m_pMap);		//NPC2��Ʈ�ѷ� �¸� �߰�
	///////////////////////////////////////////////////////////////////////

	return S_OK;
}

void cMainScene::Release()
{
}

void cMainScene::update()
{
	//ĳ���� ��Ʈ�ѷ�
	if (m_pController)
	{
		m_pController->Update(m_pCamera->GetfRotationY());
	}

	//ī�޶�
	if (m_pCamera)
		m_pCamera->Update(m_pController->GetPosition());

	//���� ��Ʈ�ѷ�
	//if (m_pMonsterController)
	//{
	//	m_pMonsterController->Update(m_pController->GetPosition(), m_pController->GetCharacterState());
	//}

	//NPC ��Ʈ�ѷ� ������Ʈ
	if (m_pNPCController)
	{
		m_pNPCController->Update(m_pController->GetPosition());
	}

	//NPC2 ��Ʈ�ѷ� ������Ʈ �߰�
	if (m_pNPCController2)
	{
		m_pNPCController2->Update(m_pController->GetPosition());
	}

	//NPC3 ��Ʈ�ѷ� ������Ʈ �߰�
	if (m_pNPCController3)
	{
		m_pNPCController3->Update(m_pController->GetPosition());
	}

	//ĳ���� ��Ų��޽�
	if (m_pSkinnedCharacter)
	{
		D3DXMATRIXA16 matR, matW;
		D3DXMatrixRotationY(&matR, D3DX_PI);
		matW = matR * *m_pController->GetWorldTM();
		m_pSkinnedCharacter->SetCharacterState(m_pController->GetCharacterState());
		m_pSkinnedCharacter->SetCharacterPosition(m_pController->GetPosition());
		m_pSkinnedCharacter->Update(&matW);
		m_pController->SetTrackPeriod(m_pSkinnedCharacter->GetTrackPeriod());	//�߰�
	}

	//���� ��Ų��޽�
	/*if (m_pSkinnedMonster)
	{
		D3DXMATRIXA16 matR, matW;
		D3DXMatrixRotationY(&matR, D3DX_PI);
		matW = matR * *m_pMonsterController->GetWorldTM();
		m_pSkinnedMonster->SetCharacterState(m_pMonsterController->GetCharacterState());
		m_pSkinnedMonster->SetCharacterPosition(m_pMonsterController->GetPosition());
		m_pSkinnedMonster->Update(&matW);	//�޽�������(���� �޽��� ���� �׷����� ��ġ ����)
		m_pMonsterController->SetTrackPeriod(m_pSkinnedMonster->GetTrackPeriod());
	}*/

	//NPC ��Ų��޽� ������Ʈ
	if (m_pSkinnedNPC)
	{
		D3DXMATRIXA16 matW;
		matW = *m_pNPCController->GetWorldTM();
		m_pSkinnedNPC->SetCharacterState(m_pNPCController->GetCharacterState());
		m_pSkinnedNPC->SetCharacterPosition(m_pNPCController->GetPosition());
		m_pSkinnedNPC->Update(&matW);
		m_pNPCController->SetTrackPeriod(m_pSkinnedNPC->GetTrackPeriod());
	}

	//NPC2 ��Ų��޽� ������Ʈ
	if (m_pSkinnedNPC2)
	{
		D3DXMATRIXA16 matW;
		matW = *m_pNPCController2->GetWorldTM();
		m_pSkinnedNPC2->SetCharacterState(m_pNPCController2->GetCharacterState());
		m_pSkinnedNPC2->SetCharacterPosition(m_pNPCController2->GetPosition());
		m_pSkinnedNPC2->Update(&matW);
		m_pNPCController2->SetTrackPeriod(m_pSkinnedNPC2->GetTrackPeriod());
	}

	//NPC3 ��Ų��޽� ������Ʈ
	if (m_pSkinnedNPC3)
	{
		D3DXMATRIXA16 matW;
		matW = *m_pNPCController3->GetWorldTM();
		m_pSkinnedNPC3->SetCharacterState(m_pNPCController3->GetCharacterState());
		m_pSkinnedNPC3->SetCharacterPosition(m_pNPCController3->GetPosition());
		m_pSkinnedNPC3->Update(&matW);
		m_pNPCController3->SetTrackPeriod(m_pSkinnedNPC3->GetTrackPeriod());
	}

	//NPC ���� ������Ʈ �߰�
	/*if (!m_vecSkinnedNPC.empty())
	{
		//NPC1
		D3DXMATRIXA16 matW;
		matW = *m_pNPCController->GetWorldTM();
		m_vecSkinnedNPC[0]->SetCharacterState(m_pNPCController->GetCharacterState());
		m_vecSkinnedNPC[0]->SetCharacterPosition(m_pNPCController->GetPosition());
		m_vecSkinnedNPC[0]->Update(&matW);
		m_pNPCController->SetTrackPeriod(m_vecSkinnedNPC[0]->GetTrackPeriod());

		//NPC2
		matW = *m_pNPCController2->GetWorldTM();
		m_vecSkinnedNPC[1]->SetCharacterState(m_pNPCController2->GetCharacterState());
		m_vecSkinnedNPC[1]->SetCharacterPosition(m_pNPCController2->GetPosition());
		m_vecSkinnedNPC[1]->Update(&matW);
		m_pNPCController2->SetTrackPeriod(m_vecSkinnedNPC[1]->GetTrackPeriod());
	}*/

	//ĳ����&���� �浹ó��
	//Collision();

	//�߰�
	//�������� �ø�
	if (m_pFrustum)
		m_pFrustum->Update();

	//�߰�
	//XMap ��� ����
	if (g_pKeyManager->isOnceKeyDown('M'))
	{
		m_isShowXMap = !m_isShowXMap;
	}

	/*Ű���� ���콺 ���̽�ƽ �߰�*/
	/*
	��� �� (�����ϴ� ������ �Ʒ��� ���� ���)
	if (g_DI_Keyboard->KeyDown(DIKEYBOARD_UP))
		m_vPosition.z++;
	if (g_DI_Keyboard->KeyDown(DIKEYBOARD_DOWN))
		m_vPosition.z--;

	if (g_DI_Mouse->PushLeft())
		m_bAttack = true;

	int temp = g_DI_Joystick->m_Controllers[0].state.Gamepad.sThumbLX / 1000;
	m_vPosition.z = temp;

	if (g_DI_Joystick->GetButtonPressed(1))
	m_vPosition.z++;
	*/
}

void cMainScene::render()
{
	g_pD3DDevice->Clear(NULL,
		NULL,
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(47, 121, 112),
		1.0f, 0);

	g_pD3DDevice->BeginScene();

	/*===================================================================================*/
	/*                                New Source, This Area                              */
	/*===================================================================================*/
	//SetStreamSource �Ǿ� �ֱ� ������, SkyBox���� �׷����Ѵ�. (���� �ڷΰ��� UI ©��)
	if (m_pSkyBox)
		m_pSkyBox->render();
	/*===================================================================================*/

	//Height��
	//if (m_pHeight)
	//	m_pHeight->Render();

	//ĳ���� ��Ʈ�ѷ�
	if (m_pController)
		m_pController->Render();

	//���� ��Ʈ�ѷ�
	//if (m_pMonsterController)
	//	m_pMonsterController->Render();

	//NPC ��Ʈ�ѷ� ����
	if (m_pNPCController)
		m_pNPCController->Render();

	//NPC2 ��Ʈ�ѷ� ���� �߰�
	if (m_pNPCController2)
		m_pNPCController2->Render();

	//NPC3 ��Ʈ�ѷ� ���� �߰�
	if (m_pNPCController3)
		m_pNPCController3->Render();

	//ĳ���� ��Ų��޽�
	if (m_pSkinnedCharacter)
		m_pSkinnedCharacter->Render();

	//���� ��Ų��޽�
	//if (m_pSkinnedMonster)
	//	m_pSkinnedMonster->Render();

	//NPC ��Ų��޽� ����
	if (m_pSkinnedNPC)
		m_pSkinnedNPC->Render();

	//NPC2 ��Ų��޽� ����
	if (m_pSkinnedNPC2)
		m_pSkinnedNPC2->Render();

	//NPC3 ��Ų��޽� ����
	if (m_pSkinnedNPC3)
		m_pSkinnedNPC3->Render();

	//NPC ���� ���� �߰�
	/*for (int i = 0; i < m_vecSkinnedNPC.size(); ++i)
	{
		m_vecSkinnedNPC[i]->UpdateAndRender();
	}*/

	//A* �߰�
	//if (m_pAStar)
	//	m_pAStar->DebugRender();


	///////////////////����� ����/////////////////////////
	DebugRender();
	///////////////////////////////////////////////////////

	if (m_pLoadMap)
		m_pLoadMap->Render();

	g_pD3DDevice->EndScene();

	g_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}

//����� ���� �Լ�
void cMainScene::DebugRender()
{

	//�浹 ��ü: ĳ����, ����
	if (g_pKeyManager->isOnceKeyDown('3'))
	{
		m_isOnKey3 = !m_isOnKey3;
	}

	if (m_isOnKey3)
	{
		m_pSkinnedCharacter->DebugRender_Collision();
		m_pSkinnedMonster->DebugRender_Collision();
		m_pSkinnedNPC->DebugRender_Collision();
		m_pSkinnedNPC2->DebugRender_Collision();		//NPC2 ����׷��� �߰�
	}
}


//ĳ����&���� �浹ó�� �Լ�
void cMainScene::Collision()
{
	if (m_pSkinnedCharacter && m_pSkinnedMonster)
	{
		D3DXVECTOR3 vDis = m_pSkinnedCharacter->GetCollisionSphere()->m_vPositionAll -
			m_pSkinnedMonster->GetCollisionSphere()->m_vPositionAll;

		float fDis = D3DXVec3Length(&vDis);

		if (fDis <= m_pSkinnedCharacter->GetCollisionSphere()->m_fRadiusAll + m_pSkinnedMonster->GetCollisionSphere()->m_fRadiusAll - D3DX_16F_EPSILON)
		{
			m_pSkinnedCharacter->SetIsCollision(true);
			m_pController->SetIsCollision(true);

			m_pSkinnedMonster->SetIsCollision(true);
		}
		else
		{
			m_pSkinnedCharacter->SetIsCollision(false);
			m_pController->SetIsCollision(false);

			m_pSkinnedMonster->SetIsCollision(false);
		}
	}
}

void cMainScene::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (m_pCamera)
		m_pCamera->WndProc(hWnd, message, wParam, lParam);

	if (m_pController)
		m_pController->WndProc(hWnd, message, wParam, lParam);
}