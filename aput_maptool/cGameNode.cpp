#include "stdafx.h"
#include "cGameNode.h"


cGameNode::cGameNode()
{
}


cGameNode::~cGameNode()
{
}

HRESULT cGameNode::Init()
{
	g_pTXTDATAManager->init();
	g_pTimeManager->init();
	g_pSoundManager->init();
	g_pKeyManager->init();
	g_pSceneManager->init();
	

	return S_OK;
}

void cGameNode::Update()
{
}

void cGameNode::Render()
{
}

void cGameNode::Release()
{
	g_pTXTDATAManager->release();
	g_pFontManager->Destroy();
	g_pKeyManager->release();
	g_pSceneManager->release();
	g_pTextureManager->Destroy();
	g_pTimeManager->Release();
	g_pSoundManager->release();
	
	//새로 만듬.
	g_pTextureManager2->Destroy();
	g_pFontManager2->Destroy();

	g_pAutoReleasePool->Drain();
	g_pDeviceManager->Destroy();
	
	//새로 만듬.
	g_pDeviceManager2->Destroy();

	//위에 2가 붙는것은 새로만든것.
}

void cGameNode::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
}
