#include "stdafx.h"
#include "cMainGame.h"

cMainGame::cMainGame()
{
}


cMainGame::~cMainGame()
{

}

HRESULT cMainGame::init()
{
	cGameNode::init();
	g_pD3DDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);

	D3DXVECTOR3 dir(1.0f, -1.0f, 0.0f);
	D3DXVec3Normalize(&dir, &dir);
	D3DXCOLOR c = WHITE;
	D3DLIGHT9 light = InitDirectionalLight(&dir, &c);
	g_pD3DDevice->SetLight(0, &light);
	g_pD3DDevice->LightEnable(0, true);

	//g_pSceneManager->addScene("mainScene", new cMainScene); //연습용
	g_pSceneManager->addScene("Village", new cVillage);
	g_pSceneManager->addScene("Stage1", new cStage1);
	g_pSceneManager->addScene("Stage2", new cStage2);
	g_pSceneManager->addScene("Stage3", new cStage3);
	g_pSceneManager->addScene("BossMap", new cBossMap);

	g_pSceneManager->addScene("TitleScene", new cTitleScene);
	g_pSceneManager->addScene("CreditScene", new cCreditScene);
	g_pSceneManager->addScene("EndingScene", new cEndingScene);

	g_pSceneManager->changeScene("EndingScene"); //TitleScene

	return S_OK;
}

void cMainGame::update()
{
	D3DXMATRIXA16 matProj;
	RECT rc;
	GetClientRect(g_hWnd, &rc);
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, rc.right / (float)rc.bottom, 1, 1000);
	g_pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);

	g_pSceneManager->update();

	cGameNode::update();
}

void cMainGame::render()
{
	g_pD3DDevice->Clear(NULL, NULL,
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(47, 121, 112), 1.0f, 0);

	g_pD3DDevice->BeginScene();


	g_pSceneManager->render();

	//프레임 출력
	g_pTimeManager->render();
	g_pD3DDevice->EndScene();

	g_pD3DDevice->Present(0, 0, 0, 0);
}

void cMainGame::Release()
{
	g_pAutoReleasePool->Drain();
	g_pSkinnedMeshManager->Destroy();
	g_pObjectManager->Destroy();

	cGameNode::Release();
}

void cMainGame::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	g_pSceneManager->WndProc(hWnd, message, wParam, lParam);
}