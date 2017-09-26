#include "stdafx.h"
#include "cMainScene.h"

#include "cCamera.h"
#include "cCharacterController.h"
#include "cMonsterController.h"	//추가
#include "cNPCController.h"		//추가
#include "cHeightMap.h"
#include "cSkinnedMesh.h"
#include "cSkinnedCharacter.h"	//추가
#include "cSkinnedMonster.h"	//추가
#include "cAStar.h"		//추가
#include "cFrustum.h"	//추가

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
	, m_pNPCController2(NULL)		//추가
	, m_pNPCController3(NULL)		//추가
	, m_pSkinnedCharacter(NULL)		
	, m_pSkinnedMonster(NULL)		
	, m_pSkinnedNPC(NULL)		
	, m_pSkinnedNPC2(NULL)			//추가
	, m_pSkinnedNPC3(NULL)			//추가
	//추가
	, m_pXMap(NULL)
	, m_isShowXMap(false)
	//추가
	, m_pAStar(NULL)
	, m_pFrustum(NULL)
	//추가
	, m_isOnKey1(false)
	, m_isOnKey3(false)
	, m_isObjMap(false)
	/*===================================================================================*/
	/*                           New Initialization, This Area                           */
	/*===================================================================================*/
	, m_pSkyBox(NULL)  // 스카이 박스 추가
	, m_pLoadMap(NULL)
	/*===================================================================================*/
{
	srand(time(NULL));
	rand();

	//m_vecSkinnedNPC.clear();	//NPC 벡터 초기화 추가
}


cMainScene::~cMainScene()
{
	/*기존의 g_p 릴리즈와 관련된 부분은 cMainGame의 Release로 옮겨졌다.	*/
}

HRESULT cMainScene::init()
{
	g_pD3DDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);

	// 디렉셔날 라이트 셋팅 및 설정
	D3DLIGHT9 light = InitDirectionalLight(&D3DXVECTOR3(1, -1, 0), &WHITE);
	g_pD3DDevice->SetLight(0, &light);
	g_pD3DDevice->LightEnable(0, true);

	//카메라
	m_pCamera = new cCamera;
	m_pCamera->AutoRelease();
	m_pCamera->Setup();

	//캐릭터 컨트롤러
	m_pController = new cCharacterController;
	m_pController->AutoRelease();

	//몬스터 컨트롤러
	m_pMonsterController = new cMonsterController;
	m_pMonsterController->AutoRelease();

	//NPC 컨트롤러
	m_pNPCController = new cNPCController;
	m_pNPCController->AutoRelease();
	m_pNPCController->Setup(&D3DXVECTOR3(5, 0, 5), D3DX_PI / 2, E_NPC_NOT_MOVE);
	m_pNPCController->SetTalkTextBox("Penguin", "Hi! Nice to meet you! What's your name?", false);

	//NPC2 컨트롤러 추가
	m_pNPCController2 = new cNPCController;
	m_pNPCController2->AutoRelease();
	m_pNPCController2->Setup(&D3DXVECTOR3(8, 0, 3), D3DX_PI / 2, E_NPC_MOVE_ONEWAY);
	m_pNPCController2->SetMoveDistance(5.0f);
	m_pNPCController2->SetTalkTextBox("Penguin2", ".........?", false);

	//NPC3 컨트롤러 추가
	m_pNPCController3 = new cNPCController;
	m_pNPCController3->AutoRelease();
	m_pNPCController3->Setup(&D3DXVECTOR3(3, 0, 8), D3DX_PI, E_NPC_MOVE_ONEWAY);
	m_pNPCController3->SetMoveDistance(5.0f);
	m_pNPCController3->SetTalkTextBox("Penguin3", "Krrrr", false);

	//스킨드 메쉬
	m_pSkinnedCharacter = g_pSkinnedMeshManager->GetSkinnedMesh("character", "eskimo_01.X", E_CHARACTER);
	m_pSkinnedMonster = g_pSkinnedMeshManager->GetSkinnedMesh("character", "yeti.X", E_MONSTER);
	m_pSkinnedNPC = g_pSkinnedMeshManager->GetSkinnedMesh("character", "pinguine_01.X", E_CHARACTER);
	m_pSkinnedNPC2 = g_pSkinnedMeshManager->GetSkinnedMesh("character", "pinguine_02.X", E_CHARACTER);
	m_pSkinnedNPC3 = g_pSkinnedMeshManager->GetSkinnedMesh("character", "pinguine_03.X", E_CHARACTER);

	m_pSkinnedNPC->SetRandomTrackPosition();
	m_pSkinnedNPC2->SetRandomTrackPosition();
	m_pSkinnedNPC3->SetRandomTrackPosition();

	//NPC 벡터 설정
	/*cSkinnedMesh* pSkinMesh = new cSkinnedMesh("character", "pinguine_01.X", E_CHARACTER);
	pSkinMesh->AutoRelease();
	pSkinMesh->SetRandomTrackPosition();  // 칼 군무 방지용
	m_vecSkinnedNPC.push_back(pSkinMesh);
	
	cSkinnedMesh* pSkinMesh2 = new cSkinnedMesh("character", "pinguine_02.X", E_CHARACTER);
	pSkinMesh2->AutoRelease();
	pSkinMesh2->SetRandomTrackPosition();  // 칼 군무 방지용
	m_vecSkinnedNPC.push_back(pSkinMesh2);*/

	////////////////////////////////////// MAP ////////////////////////////////////////
	//Height맵
	D3DXMATRIXA16 matS;
	D3DXMatrixScaling(&matS, 1.0f, 1.0f, 1.0f);
	m_pHeight = new cHeightMap;
	m_pHeight->AutoRelease();
	m_pHeight->Load("HeightMapData/HeightMap.raw", &matS);
	//m_pMap = m_pHeight;
	//m_pController->SetMap(m_pMap);
	//m_pMonsterController->SetMap(m_pMap);
	//m_pNPCController->SetMap(m_pMap);		//NPC컨트롤러 셋맵 추가

	//A*추가
	//m_pAStar = new cAStar;
	//m_pAStar->AutoRelease();
	//m_pAStar->Setup(m_pMap);
	//m_pMonsterController->SetAStar(m_pAStar);
	//m_pMonsterController->Setup();

	//프러스텀 컬링
	m_pFrustum = new cFrustum;
	m_pFrustum->AutoRelease();
	m_pFrustum->Setup();

	/*===================================================================================*/
	/*                                New Source, This Area                              */
	/*===================================================================================*/
	//스카이박스 추가 및 초기화
	m_pSkyBox = new cSkyBox;
	m_pSkyBox->init();
	m_pSkyBox->AutoRelease();
	/*===================================================================================*/

	/*
	//추가: cLoadMap을 이용한 맵그리기
	*/
	///////////////////////////////////////////////////////////////////////
	m_pLoadMap = new cLoadMap;
	m_pLoadMap->AutoRelease();
	m_pLoadMap->RealLoad_MainScene();
	m_pMap = m_pLoadMap;
	m_pController->SetMap(m_pMap);
	//m_pMonsterController->SetMap(m_pMap);
	m_pNPCController->SetMap(m_pMap);
	m_pNPCController2->SetMap(m_pMap);		//NPC2컨트롤러 셋맵 추가
	///////////////////////////////////////////////////////////////////////

	return S_OK;
}

void cMainScene::Release()
{
}

void cMainScene::update()
{
	//캐릭터 컨트롤러
	if (m_pController)
	{
		m_pController->Update(m_pCamera->GetfRotationY());
	}

	//카메라
	if (m_pCamera)
		m_pCamera->Update(m_pController->GetPosition());

	//몬스터 컨트롤러
	//if (m_pMonsterController)
	//{
	//	m_pMonsterController->Update(m_pController->GetPosition(), m_pController->GetCharacterState());
	//}

	//NPC 컨트롤러 업데이트
	if (m_pNPCController)
	{
		m_pNPCController->Update(m_pController->GetPosition());
	}

	//NPC2 컨트롤러 업데이트 추가
	if (m_pNPCController2)
	{
		m_pNPCController2->Update(m_pController->GetPosition());
	}

	//NPC3 컨트롤러 업데이트 추가
	if (m_pNPCController3)
	{
		m_pNPCController3->Update(m_pController->GetPosition());
	}

	//캐릭터 스킨드메쉬
	if (m_pSkinnedCharacter)
	{
		D3DXMATRIXA16 matR, matW;
		D3DXMatrixRotationY(&matR, D3DX_PI);
		matW = matR * *m_pController->GetWorldTM();
		m_pSkinnedCharacter->SetCharacterState(m_pController->GetCharacterState());
		m_pSkinnedCharacter->SetCharacterPosition(m_pController->GetPosition());
		m_pSkinnedCharacter->Update(&matW);
		m_pController->SetTrackPeriod(m_pSkinnedCharacter->GetTrackPeriod());	//추가
	}

	//몬스터 스킨드메쉬
	/*if (m_pSkinnedMonster)
	{
		D3DXMATRIXA16 matR, matW;
		D3DXMatrixRotationY(&matR, D3DX_PI);
		matW = matR * *m_pMonsterController->GetWorldTM();
		m_pSkinnedMonster->SetCharacterState(m_pMonsterController->GetCharacterState());
		m_pSkinnedMonster->SetCharacterPosition(m_pMonsterController->GetPosition());
		m_pSkinnedMonster->Update(&matW);	//메쉬조정값(현재 메쉬가 실제 그려지는 위치 조정)
		m_pMonsterController->SetTrackPeriod(m_pSkinnedMonster->GetTrackPeriod());
	}*/

	//NPC 스킨드메쉬 업데이트
	if (m_pSkinnedNPC)
	{
		D3DXMATRIXA16 matW;
		matW = *m_pNPCController->GetWorldTM();
		m_pSkinnedNPC->SetCharacterState(m_pNPCController->GetCharacterState());
		m_pSkinnedNPC->SetCharacterPosition(m_pNPCController->GetPosition());
		m_pSkinnedNPC->Update(&matW);
		m_pNPCController->SetTrackPeriod(m_pSkinnedNPC->GetTrackPeriod());
	}

	//NPC2 스킨드메쉬 업데이트
	if (m_pSkinnedNPC2)
	{
		D3DXMATRIXA16 matW;
		matW = *m_pNPCController2->GetWorldTM();
		m_pSkinnedNPC2->SetCharacterState(m_pNPCController2->GetCharacterState());
		m_pSkinnedNPC2->SetCharacterPosition(m_pNPCController2->GetPosition());
		m_pSkinnedNPC2->Update(&matW);
		m_pNPCController2->SetTrackPeriod(m_pSkinnedNPC2->GetTrackPeriod());
	}

	//NPC3 스킨드메쉬 업데이트
	if (m_pSkinnedNPC3)
	{
		D3DXMATRIXA16 matW;
		matW = *m_pNPCController3->GetWorldTM();
		m_pSkinnedNPC3->SetCharacterState(m_pNPCController3->GetCharacterState());
		m_pSkinnedNPC3->SetCharacterPosition(m_pNPCController3->GetPosition());
		m_pSkinnedNPC3->Update(&matW);
		m_pNPCController3->SetTrackPeriod(m_pSkinnedNPC3->GetTrackPeriod());
	}

	//NPC 벡터 업데이트 추가
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

	//캐릭터&몬스터 충돌처리
	//Collision();

	//추가
	//프러스텀 컬링
	if (m_pFrustum)
		m_pFrustum->Update();

	//추가
	//XMap 출력 여부
	if (g_pKeyManager->isOnceKeyDown('M'))
	{
		m_isShowXMap = !m_isShowXMap;
	}

	/*키보드 마우스 조이스틱 추가*/
	/*
	사용 예 (조작하는 곳에서 아래와 같이 사용)
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
	//SetStreamSource 되어 있기 때문에, SkyBox부터 그려야한다. (순서 뒤로가면 UI 짤림)
	if (m_pSkyBox)
		m_pSkyBox->render();
	/*===================================================================================*/

	//Height맵
	//if (m_pHeight)
	//	m_pHeight->Render();

	//캐릭터 컨트롤러
	if (m_pController)
		m_pController->Render();

	//몬스터 컨트롤러
	//if (m_pMonsterController)
	//	m_pMonsterController->Render();

	//NPC 컨트롤러 렌더
	if (m_pNPCController)
		m_pNPCController->Render();

	//NPC2 컨트롤러 렌더 추가
	if (m_pNPCController2)
		m_pNPCController2->Render();

	//NPC3 컨트롤러 렌더 추가
	if (m_pNPCController3)
		m_pNPCController3->Render();

	//캐릭터 스킨드메쉬
	if (m_pSkinnedCharacter)
		m_pSkinnedCharacter->Render();

	//몬스터 스킨드메쉬
	//if (m_pSkinnedMonster)
	//	m_pSkinnedMonster->Render();

	//NPC 스킨드메쉬 렌더
	if (m_pSkinnedNPC)
		m_pSkinnedNPC->Render();

	//NPC2 스킨드메쉬 렌더
	if (m_pSkinnedNPC2)
		m_pSkinnedNPC2->Render();

	//NPC3 스킨드메쉬 렌더
	if (m_pSkinnedNPC3)
		m_pSkinnedNPC3->Render();

	//NPC 벡터 렌더 추가
	/*for (int i = 0; i < m_vecSkinnedNPC.size(); ++i)
	{
		m_vecSkinnedNPC[i]->UpdateAndRender();
	}*/

	//A* 추가
	//if (m_pAStar)
	//	m_pAStar->DebugRender();


	///////////////////디버깅 렌더/////////////////////////
	DebugRender();
	///////////////////////////////////////////////////////

	if (m_pLoadMap)
		m_pLoadMap->Render();

	g_pD3DDevice->EndScene();

	g_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}

//디버깅 렌더 함수
void cMainScene::DebugRender()
{

	//충돌 구체: 캐릭터, 몬스터
	if (g_pKeyManager->isOnceKeyDown('3'))
	{
		m_isOnKey3 = !m_isOnKey3;
	}

	if (m_isOnKey3)
	{
		m_pSkinnedCharacter->DebugRender_Collision();
		m_pSkinnedMonster->DebugRender_Collision();
		m_pSkinnedNPC->DebugRender_Collision();
		m_pSkinnedNPC2->DebugRender_Collision();		//NPC2 디버그렌더 추가
	}
}


//캐릭터&몬스터 충돌처리 함수
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