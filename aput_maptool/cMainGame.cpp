#include "stdafx.h"
#include "cMainGame.h"

cMainGame::cMainGame()
{
}


cMainGame::~cMainGame()
{

}

HRESULT cMainGame::Init()
{
	cGameNode::Init();
	g_pD3DDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);

	g_pSceneManager->addScene("MainScene", new cMainScene);
	g_pSceneManager->addScene("MapTool", new cMapTool);

	g_pSceneManager->changeScene("MapTool");

	return S_OK;
}

void cMainGame::Update()
{
	D3DXMATRIXA16 matProj;
	RECT rc;
	GetClientRect(g_hWnd, &rc);
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, rc.right / (float)rc.bottom, 1, 1000);
	g_pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);

	g_pSceneManager->update();

	cGameNode::Update();
}

void cMainGame::Render()
{
	g_pD3DDevice->Clear(NULL, NULL,
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(128, 128, 128), 1.0f, 0);

	g_pD3DDevice->BeginScene();


	g_pSceneManager->render();

	//프레임 출력
	g_pTimeManager->render();
	g_pD3DDevice->EndScene();

	g_pD3DDevice->Present(0, 0, 0, 0);
}

void cMainGame::Release()
{
	g_pSceneManager->release();

	cGameNode::Release();
}

void cMainGame::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	g_pSceneManager->WndProc(hWnd, message, wParam, lParam);
}