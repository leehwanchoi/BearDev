#include "stdafx.h"
#include "cGameNode.h"


cGameNode::cGameNode()
{
}


cGameNode::~cGameNode()
{
}

HRESULT cGameNode::init()
{
	g_pTextDataManager->init();
	g_pTimeManager->init();
	g_pSoundManager->init();
	g_pKeyManager->init();
	g_pSceneManager->init();
	

	return S_OK;
}

void cGameNode::update()
{
}

void cGameNode::render()
{
}

void cGameNode::Release()
{
	g_pTextDataManager->release();
	g_pFontManager->Destroy();
	g_pKeyManager->release();
	g_pSceneManager->release();
	g_pTextureManager->Destroy();
	g_pTimeManager->Release();
	g_pSoundManager->release();
	//g_pAutoReleasePool->Drain();	//¼öÁ¤
	g_pDeviceManager->Destroy();
}

void cGameNode::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
}
