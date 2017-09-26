#include "stdafx.h"
#include "cLoadingScene.h"
#include "cUIImageView.h"


cLoadingScene::cLoadingScene()
	: m_pSprite(NULL)
	, m_pUIRoot(NULL)
	, m_isLoadingRender(false)
	, m_isChange(false)
{
	ZeroMemory(&m_stImageInfo, sizeof(D3DXIMAGE_INFO));
}


cLoadingScene::~cLoadingScene()
{
	SAFE_RELEASE(m_pSprite);
}

void cLoadingScene::Setup()
{
	D3DXCreateSprite(g_pD3DDevice, &m_pSprite);

	LPDIRECT3DTEXTURE9 pTexture;
	D3DXIMAGE_INFO stImageInfo;

	char str[1024] = "./UI/loadingscreen.png";
	g_pTextureManager->AddTexture(str, str, &pTexture, &stImageInfo);

	cUIImageView* pImageView = new cUIImageView;
	pImageView->AutoRelease();
	pImageView->SetTexture(pTexture);
	pImageView->SetSize(ST_SIZE(stImageInfo.Width, stImageInfo.Height));
	pImageView->SetLocalPos(D3DXVECTOR3(0, 0, 0));
	pImageView->SetDebugRender(true);
	m_pUIRoot = pImageView;

	m_sSceneName = ""; // ó���� �� �̸�����.
}

void cLoadingScene::Update(D3DXVECTOR3* pos)
{
	LoadingUpdate();

	if (m_pUIRoot)
		m_pUIRoot->Update();
}

void cLoadingScene::Render()
{
	//�� ��ȯ �̸�
	if (m_isLoadingRender)
	{
		if (m_pUIRoot)
			m_pUIRoot->Render(m_pSprite);

		//�ε�ȭ������ �ٲٰ� ���� ���
		m_isChange = true;
	}
}

void cLoadingScene::LoadingUpdate()
{
	//�� ����
	if (m_isChange)
		g_pSceneManager->changeScene(m_sSceneName);	
}

void cLoadingScene::LoadingScene(string Scene)
{
	m_isLoadingRender = true;
	m_sSceneName = Scene;
}
