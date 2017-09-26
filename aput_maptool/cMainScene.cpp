#include "stdafx.h"
#include "cMainScene.h"

#include "cCamera.h"
#include "cCharacterController.h"
#include "cAseCharacter.h"
#include "cHeightMap.h"
#include "cAStar.h"
#include "cUIDraw.h"
#include "cSkinnedMesh.h"
#include "cFrustum.h"
#include "cAlphaBlendTest.h"

cMainScene::cMainScene()
	: m_pCamera(NULL)
	, m_pController(NULL)
	, m_pAseCharacter(NULL)
	, m_pAStar(NULL)
	, m_pUIDraw(NULL)
	, m_pSkinnedMesh(NULL)
	, m_pFont(NULL)
	, m_pRect(NULL)
	, m_pFrustum(NULL)
	, m_pAlphaTest(NULL)
{
	srand(time(NULL));
	rand();
}


cMainScene::~cMainScene()
{
}

HRESULT cMainScene::Init()
{
	g_pD3DDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);

	// 디렉셔날 라이트 셋팅 및 설정
	D3DLIGHT9 light = InitDirectionalLight(&D3DXVECTOR3(1, -1, 0), &WHITE);
	g_pD3DDevice->SetLight(0, &light);
	g_pD3DDevice->LightEnable(0, true);

	m_pCamera = new cCamera;
	m_pCamera->AutoRelease();
	m_pCamera->Setup();

	m_pController = new cCharacterController;
	m_pController->AutoRelease();

	D3DXMATRIXA16 matS;
	D3DXMatrixScaling(&matS, 1.0f, 1.0f, 1.0f);
	m_pHeight = new cHeightMap;
	m_pHeight->AutoRelease();
	m_pHeight->Load("HeightMapData/HeightMap.raw", &matS);
	m_pMap = m_pHeight;
	m_pController->SetMap(m_pMap);

	m_pAseCharacter = new cAseCharacter;
	m_pAseCharacter->Load("woman/woman_01_all.ASE");

	m_pAStar = new cAStar;
	m_pAStar->AutoRelease();
	m_pAStar->Setup(m_pMap);
	m_pController->SetAStar(m_pAStar);
	m_pController->Setup();

	m_pFrustum = new cFrustum;
	m_pFrustum->AutoRelease();
	m_pFrustum->Setup();

	m_pUIDraw = new cUIDraw;
	m_pUIDraw->AutoRelease();
	m_pUIDraw->Setup();

	m_pSkinnedMesh = g_pSkinnedMeshManager->GetSkinnedMesh("modeling", "eskimo_01_DirecX.X");


	for (int i = 0; i < NUM_SKINNEDMESH; ++i)
	{
		for (int j = 0; j < NUM_SKINNEDMESH; ++j)
		{
			cSkinnedMesh* pSkinnedMesh = new cSkinnedMesh("modeling", "yeti_DirecX.X");
			pSkinnedMesh->AutoRelease();
			pSkinnedMesh->SetRandomTrackPosition();  // 칼 군무 방지용

			D3DXVECTOR3 pos(1.0f * j, 0.0f, 1.0f * i);
			m_pHeight->GetHeight(pos.x, OUT pos.y, pos.z);
			pSkinnedMesh->SetPosition(pos);

			m_vecSkinnedMesh.push_back(pSkinnedMesh);
		}
	}

	m_pAlphaTest = new cAlphaBlendTest;
	m_pAlphaTest->AutoRelease();
	m_pAlphaTest->Setup();

	m_pRect = new RECT;
	m_pRect->left = 10;
	m_pRect->top = 10;
	m_pRect->right = 110;
	m_pRect->bottom = 60;

	D3DXCreateFont(g_pD3DDevice, 48, 24, 0, 1, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, false, "궁서체", &m_pFont);

	return S_OK;
}

void cMainScene::Release()
{
	g_pSkinnedMeshManager->Destroy();
	SAFE_RELEASE(m_pFont);
	SAFE_DELETE(m_pRect);
}

void cMainScene::Update()
{
	if (m_pController)
		m_pController->Update();

	if (m_pAseCharacter)
		m_pAseCharacter->Update(m_pController->GetWorldTM());

	if (m_pCamera)
		m_pCamera->Update(m_pController->GetPosition());

	if (m_pFrustum)
		m_pFrustum->Update();

	if (m_pUIDraw)
		m_pUIDraw->Update();

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

void cMainScene::Render()
{
	g_pD3DDevice->Clear(NULL,
		NULL,
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(47, 121, 112),
		1.0f, 0);

	g_pD3DDevice->BeginScene();

	if (m_pHeight)
		m_pHeight->Render();

	if (m_pAseCharacter)
		m_pAseCharacter->Render();

	if (m_pAStar)
		m_pAStar->DebugRender();

	if (m_pController)
		m_pController->Render();

	if (m_pSkinnedMesh)
		m_pSkinnedMesh->UpdateAndRender();

	static bool doingFrustum = false;

	if (GetAsyncKeyState(VK_SPACE) & 0x0001)
		doingFrustum = !doingFrustum;

	for (int i = 0; i < m_vecSkinnedMesh.size(); ++i)
	{
		if (doingFrustum)
			m_pFrustum->IsInFrustum(m_vecSkinnedMesh[i]->GetBoundingSphere());

		if (m_vecSkinnedMesh[i]->GetBoundingSphere()->isInFrustum)
			m_vecSkinnedMesh[i]->UpdateAndRender();
	}

	if (m_pUIDraw)
		m_pUIDraw->Render();

	if (m_pAlphaTest)
		m_pAlphaTest->Render();

	//char szStr[1024];
	//sprintf(szStr, "%d\n%d", g_DI_Joystick->m_Controllers[0].state.Gamepad.sThumbLX, g_DI_Joystick->m_Controllers[0].state.Gamepad.sThumbLY);
	//m_pFont->DrawTextA(NULL, szStr, strlen(szStr),
	//	m_pRect, DT_TOP | DT_LEFT | DT_NOCLIP, D3DCOLOR_XRGB(255, 0, 0));

	g_pD3DDevice->EndScene();

	g_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}

void cMainScene::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (m_pCamera)
		m_pCamera->WndProc(hWnd, message, wParam, lParam);

	if (m_pController)
		m_pController->WndProc(hWnd, message, wParam, lParam);
}
