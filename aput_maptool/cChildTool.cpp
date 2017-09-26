#include "stdafx.h"
#include "cChildTool.h"
#include "cChildWindowUIImageView.h"
#include "cChildWindowUITextView.h"

cChildTool::cChildTool()
	: m_pUIRoot(NULL)
	, m_pSprite(NULL)
	, m_isActivate(false)
	, m_isLoad(false)
	, m_isSave(false)
	, m_isXFile(false)
	, m_isTexture(false)
	, m_isMod(false)
{
}


cChildTool::~cChildTool()
{
	SAFE_RELEASE(m_pSprite);
}

void cChildTool::Setup()
{
	D3DXCreateSprite(g_pD3DDevice2, &m_pSprite);

	LPDIRECT3DTEXTURE9 pTexture;
	D3DXIMAGE_INFO stImageInfo;

	//제목 UI
	char str[1024] = "./MapToolUI/TextUI.png";
	g_pTextureManager2->AddTexture(str, str, &pTexture, &stImageInfo);

	//맵툴 UI
	cChildWindowUIImageView* pImageView = new cChildWindowUIImageView;
	pImageView->AutoRelease();
	pImageView->SetTexture(pTexture);
	pImageView->SetSize(childST_SIZE(stImageInfo.Width, stImageInfo.Height));
	pImageView->SetLocalPos(D3DXVECTOR3(0, 0, 0));
	pImageView->SetDebugRender(false);
	m_pUIRoot = pImageView;

	//맵툴 UI 텍스트
	cChildWindowUITextView* pTextView = new cChildWindowUITextView;
	pTextView->AutoRelease();
	pTextView->SetText("MapTool ver 0.1");
	pTextView->SetSize(childST_SIZE(100, 100));
	pTextView->SetLocalPos(D3DXVECTOR3(CHILDSIZEX / 5, 0, 0));
	pTextView->SetFont(g_pFontManager2->GetFont(cChildWindowFontManager::E_childQUEST));
	pTextView->SetDebugRender(false);
	pTextView->SetDrawTextFormat(DT_CENTER | DT_VCENTER | DT_WORDBREAK);
	pTextView->SetColor(D3DCOLOR_XRGB(0, 175, 128));
	m_pUIRoot->AddChild(pTextView);

	//그리드 UI
	char str2[1024] = "./MapToolUI/TextUI2.png";
	g_pTextureManager2->AddTexture(str2, str2, &pTexture, &stImageInfo);
	pImageView = new cChildWindowUIImageView;
	pImageView->AutoRelease();
	pImageView->SetTexture(pTexture);
	pImageView->SetSize(childST_SIZE(stImageInfo.Width, stImageInfo.Height));
	pImageView->SetLocalPos(D3DXVECTOR3(0, 100, 0));
	pImageView->SetDebugRender(false);
	m_pUIRoot->AddChild(pImageView);

	//그리드 설정 텍스트
	pTextView = new cChildWindowUITextView;
	pTextView->AutoRelease();
	pTextView->SetText("Grid Number");
	pTextView->SetSize(childST_SIZE(100, 100));
	pTextView->SetLocalPos(D3DXVECTOR3(CHILDSIZEX / 5, 100, 0));
	pTextView->SetFont(g_pFontManager2->GetFont(cChildWindowFontManager::E_childQUEST));
	pTextView->SetDebugRender(false);
	pTextView->SetDrawTextFormat(DT_CENTER | DT_VCENTER | DT_WORDBREAK);
	pTextView->SetColor(D3DCOLOR_XRGB(0, 175, 128));
	pTextView->SetTag(E_childGridTEXTVIEW);
	m_pUIRoot->AddChild(pTextView);

	//그리드 설정 버튼1
	cChildWindowUIButton* pButton = NULL;
	pButton = new cChildWindowUIButton;
	pButton->AutoRelease();
	pButton->SetTexture("./MapToolUI/LeftButton.png", "./MapToolUI/LeftButton.png", "./MapToolUI/LeftButton.png");
	pButton->SetSize(childST_SIZE(30, 30));
	pButton->SetLocalPos(D3DXVECTOR3(6, 135, 0));
	pButton->SetDebugRender(false);
	pButton->SetTag(E_childGridDownBUTTON);
	pButton->SetDelegate(this);
	m_pUIRoot->AddChild(pButton);

	//그리드 설정 버튼2
	pButton = new cChildWindowUIButton;
	pButton->AutoRelease();
	pButton->SetTexture("./MapToolUI/RightButton.png", "./MapToolUI/RightButton.png", "./MapToolUI/RightButton.png");
	pButton->SetSize(childST_SIZE(30, 30));
	pButton->SetLocalPos(D3DXVECTOR3(142, 135, 0));
	pButton->SetDebugRender(false);
	pButton->SetTag(E_childGridUpBUTTON);
	pButton->SetDelegate(this);
	m_pUIRoot->AddChild(pButton);

	//그리드 최소 높이 UI
	g_pTextureManager2->AddTexture(str2, str2, &pTexture, &stImageInfo);
	pImageView = new cChildWindowUIImageView;
	pImageView->AutoRelease();
	pImageView->SetTexture(pTexture);
	pImageView->SetSize(childST_SIZE(stImageInfo.Width, stImageInfo.Height));
	pImageView->SetLocalPos(D3DXVECTOR3(0, 200, 0));
	pImageView->SetDebugRender(false);
	m_pUIRoot->AddChild(pImageView);

	//그리드 최소 높이 설정 텍스트
	pTextView = new cChildWindowUITextView;
	pTextView->AutoRelease();
	pTextView->SetText("Min Height");
	pTextView->SetSize(childST_SIZE(100, 100));
	pTextView->SetLocalPos(D3DXVECTOR3(CHILDSIZEX / 5, 200, 0));
	pTextView->SetFont(g_pFontManager2->GetFont(cChildWindowFontManager::E_childQUEST));
	pTextView->SetDebugRender(false);
	pTextView->SetDrawTextFormat(DT_CENTER | DT_VCENTER | DT_WORDBREAK);
	pTextView->SetColor(D3DCOLOR_XRGB(0, 175, 128));
	pTextView->SetTag(E_childGridMinHeightTEXTVIEW);
	m_pUIRoot->AddChild(pTextView);

	//그리드 최소 높이 설정 버튼1
	pButton = NULL;
	pButton = new cChildWindowUIButton;
	pButton->AutoRelease();
	pButton->SetTexture("./MapToolUI/LeftButton.png", "./MapToolUI/LeftButton.png", "./MapToolUI/LeftButton.png");
	pButton->SetSize(childST_SIZE(30, 30));
	pButton->SetLocalPos(D3DXVECTOR3(6, 235, 0));
	pButton->SetDebugRender(false);
	pButton->SetTag(E_childGridMinHeightDownBUTTON);
	pButton->SetDelegate(this);
	m_pUIRoot->AddChild(pButton);

	//그리드 최소 높이 설정 버튼2
	pButton = new cChildWindowUIButton;
	pButton->AutoRelease();
	pButton->SetTexture("./MapToolUI/RightButton.png", "./MapToolUI/RightButton.png", "./MapToolUI/RightButton.png");
	pButton->SetSize(childST_SIZE(30, 30));
	pButton->SetLocalPos(D3DXVECTOR3(142, 235, 0));
	pButton->SetDebugRender(false);
	pButton->SetTag(E_childGridMinHeightUpBUTTON);
	pButton->SetDelegate(this);
	m_pUIRoot->AddChild(pButton);

	//그리드 최대 높이 UI
	g_pTextureManager2->AddTexture(str2, str2, &pTexture, &stImageInfo);
	pImageView = new cChildWindowUIImageView;
	pImageView->AutoRelease();
	pImageView->SetTexture(pTexture);
	pImageView->SetSize(childST_SIZE(stImageInfo.Width, stImageInfo.Height));
	pImageView->SetLocalPos(D3DXVECTOR3(0, 300, 0));
	pImageView->SetDebugRender(false);
	m_pUIRoot->AddChild(pImageView);

	//그리드 최대 높이 설정 텍스트
	pTextView = new cChildWindowUITextView;
	pTextView->AutoRelease();
	pTextView->SetText("Max Height");
	pTextView->SetSize(childST_SIZE(100, 100));
	pTextView->SetLocalPos(D3DXVECTOR3(CHILDSIZEX / 5, 300, 0));
	pTextView->SetFont(g_pFontManager2->GetFont(cChildWindowFontManager::E_childQUEST));
	pTextView->SetDebugRender(false);
	pTextView->SetDrawTextFormat(DT_CENTER | DT_VCENTER | DT_WORDBREAK);
	pTextView->SetColor(D3DCOLOR_XRGB(0, 175, 128));
	pTextView->SetTag(E_childGridMaxHeightTEXTVIEW);
	m_pUIRoot->AddChild(pTextView);

	//그리드 최대 높이 설정 버튼1
	pButton = NULL;
	pButton = new cChildWindowUIButton;
	pButton->AutoRelease();
	pButton->SetTexture("./MapToolUI/LeftButton.png", "./MapToolUI/LeftButton.png", "./MapToolUI/LeftButton.png");
	pButton->SetSize(childST_SIZE(30, 30));
	pButton->SetLocalPos(D3DXVECTOR3(6, 335, 0));
	pButton->SetDebugRender(false);
	pButton->SetTag(E_childGridMaxHeightDownBUTTON);
	pButton->SetDelegate(this);
	m_pUIRoot->AddChild(pButton);

	//그리드 최대 높이 설정 버튼2
	pButton = new cChildWindowUIButton;
	pButton->AutoRelease();
	pButton->SetTexture("./MapToolUI/RightButton.png", "./MapToolUI/RightButton.png", "./MapToolUI/RightButton.png");
	pButton->SetSize(childST_SIZE(30, 30));
	pButton->SetLocalPos(D3DXVECTOR3(142, 335, 0));
	pButton->SetDebugRender(false);
	pButton->SetTag(E_childGridMaxHeightUpBUTTON);
	pButton->SetDelegate(this);
	m_pUIRoot->AddChild(pButton);

	//그리드 평탄화 높이 UI
	g_pTextureManager2->AddTexture(str2, str2, &pTexture, &stImageInfo);
	pImageView = new cChildWindowUIImageView;
	pImageView->AutoRelease();
	pImageView->SetTexture(pTexture);
	pImageView->SetSize(childST_SIZE(stImageInfo.Width, stImageInfo.Height));
	pImageView->SetLocalPos(D3DXVECTOR3(0, 400, 0));
	pImageView->SetDebugRender(false);
	m_pUIRoot->AddChild(pImageView);

	//그리드 평탄화 높이 설정 텍스트
	pTextView = new cChildWindowUITextView;
	pTextView->AutoRelease();
	pTextView->SetText("Flat Height");
	pTextView->SetSize(childST_SIZE(100, 100));
	pTextView->SetLocalPos(D3DXVECTOR3(CHILDSIZEX / 5, 400, 0));
	pTextView->SetFont(g_pFontManager2->GetFont(cChildWindowFontManager::E_childQUEST));
	pTextView->SetDebugRender(false);
	pTextView->SetDrawTextFormat(DT_CENTER | DT_VCENTER | DT_WORDBREAK);
	pTextView->SetColor(D3DCOLOR_XRGB(0, 175, 128));
	pTextView->SetTag(E_childGridFlatHeightTEXTVIEW);
	m_pUIRoot->AddChild(pTextView);

	//그리드 평탄화 높이 설정 버튼1
	pButton = NULL;
	pButton = new cChildWindowUIButton;
	pButton->AutoRelease();
	pButton->SetTexture("./MapToolUI/LeftButton.png", "./MapToolUI/LeftButton.png", "./MapToolUI/LeftButton.png");
	pButton->SetSize(childST_SIZE(30, 30));
	pButton->SetLocalPos(D3DXVECTOR3(6, 435, 0));
	pButton->SetDebugRender(false);
	pButton->SetTag(E_childGridFlatHeightDownBUTTON);
	pButton->SetDelegate(this);
	m_pUIRoot->AddChild(pButton);

	//그리드 평탄화 높이 설정 버튼2
	pButton = new cChildWindowUIButton;
	pButton->AutoRelease();
	pButton->SetTexture("./MapToolUI/RightButton.png", "./MapToolUI/RightButton.png", "./MapToolUI/RightButton.png");
	pButton->SetSize(childST_SIZE(30, 30));
	pButton->SetLocalPos(D3DXVECTOR3(142, 435, 0));
	pButton->SetDebugRender(false);
	pButton->SetTag(E_childGridFlatHeightUpBUTTON);
	pButton->SetDelegate(this);
	m_pUIRoot->AddChild(pButton);

	//그리드 스피드 UI
	g_pTextureManager2->AddTexture(str2, str2, &pTexture, &stImageInfo);
	pImageView = new cChildWindowUIImageView;
	pImageView->AutoRelease();
	pImageView->SetTexture(pTexture);
	pImageView->SetSize(childST_SIZE(stImageInfo.Width, stImageInfo.Height));
	pImageView->SetLocalPos(D3DXVECTOR3(0, 500, 0));
	pImageView->SetDebugRender(false);
	m_pUIRoot->AddChild(pImageView);

	//그리드 스피드 설정 텍스트
	pTextView = new cChildWindowUITextView;
	pTextView->AutoRelease();
	pTextView->SetText("Terrain Speed");
	pTextView->SetSize(childST_SIZE(100, 100));
	pTextView->SetLocalPos(D3DXVECTOR3(CHILDSIZEX / 5, 500, 0));
	pTextView->SetFont(g_pFontManager2->GetFont(cChildWindowFontManager::E_childQUEST));
	pTextView->SetDebugRender(false);
	pTextView->SetDrawTextFormat(DT_CENTER | DT_VCENTER | DT_WORDBREAK);
	pTextView->SetColor(D3DCOLOR_XRGB(0, 175, 128));
	pTextView->SetTag(E_childGridSpeedTEXTVIEW);
	m_pUIRoot->AddChild(pTextView);

	//그리드 스피드 설정 버튼1
	pButton = NULL;
	pButton = new cChildWindowUIButton;
	pButton->AutoRelease();
	pButton->SetTexture("./MapToolUI/LeftButton.png", "./MapToolUI/LeftButton.png", "./MapToolUI/LeftButton.png");
	pButton->SetSize(childST_SIZE(30, 30));
	pButton->SetLocalPos(D3DXVECTOR3(6, 535, 0));
	pButton->SetDebugRender(false);
	pButton->SetTag(E_childGridSpeedDownBUTTON);
	pButton->SetDelegate(this);
	m_pUIRoot->AddChild(pButton);

	//그리드 스피드 설정 버튼2
	pButton = new cChildWindowUIButton;
	pButton->AutoRelease();
	pButton->SetTexture("./MapToolUI/RightButton.png", "./MapToolUI/RightButton.png", "./MapToolUI/RightButton.png");
	pButton->SetSize(childST_SIZE(30, 30));
	pButton->SetLocalPos(D3DXVECTOR3(142, 535, 0));
	pButton->SetDebugRender(false);
	pButton->SetTag(E_childGridSpeedUpBUTTON);
	pButton->SetDelegate(this);
	m_pUIRoot->AddChild(pButton);

	//X파일 로드 UI
	char str5[1024] = "./MapToolUI/TextureUI.png";
	g_pTextureManager2->AddTexture(str5, str5, &pTexture, &stImageInfo);
	pImageView = new cChildWindowUIImageView;
	pImageView->AutoRelease();
	pImageView->SetTexture(pTexture);
	pImageView->SetSize(childST_SIZE(150, 30));
	pImageView->SetLocalPos(D3DXVECTOR3(0, CHILDSIZEY - 255, 0));
	pImageView->SetDebugRender(false);
	m_pUIRoot->AddChild(pImageView);

	//X파일 로드 텍스트
	pTextView = new cChildWindowUITextView;
	pTextView->AutoRelease();
	pTextView->SetText("X File");
	pTextView->SetSize(childST_SIZE(150, 100));
	pTextView->SetLocalPos(D3DXVECTOR3(0, CHILDSIZEY - 290, 0));
	pTextView->SetFont(g_pFontManager2->GetFont(cChildWindowFontManager::E_childQUEST));
	pTextView->SetDebugRender(false);
	pTextView->SetDrawTextFormat(DT_CENTER | DT_VCENTER | DT_WORDBREAK);
	pTextView->SetColor(D3DCOLOR_XRGB(0, 175, 128));
	pTextView->SetTag(E_childXFileTEXTVIEW);
	m_pUIRoot->AddChild(pTextView);

	//X파일 로드 버튼
	pButton = new cChildWindowUIButton;
	pButton->AutoRelease();
	pButton->SetTexture("./MapToolUI/open.png", "./MapToolUI/open.png", "./MapToolUI/open.png");
	pButton->SetSize(childST_SIZE(30, 30));
	pButton->SetLocalPos(D3DXVECTOR3(150, CHILDSIZEY - 255, 0));
	pButton->SetDebugRender(false);
	pButton->SetTag(E_childXFileLoadBUTTON);
	pButton->SetDelegate(this);
	m_pUIRoot->AddChild(pButton);

	//텍스쳐 로드 UI
	g_pTextureManager2->AddTexture(str5, str5, &pTexture, &stImageInfo);
	pImageView = new cChildWindowUIImageView;
	pImageView->AutoRelease();
	pImageView->SetTexture(pTexture);
	pImageView->SetSize(childST_SIZE(150, 30));
	pImageView->SetLocalPos(D3DXVECTOR3(0, CHILDSIZEY - 225, 0));
	pImageView->SetDebugRender(false);
	m_pUIRoot->AddChild(pImageView);

	//텍스쳐 로드 텍스트
	pTextView = new cChildWindowUITextView;
	pTextView->AutoRelease();
	pTextView->SetText("Grid Texture");
	pTextView->SetSize(childST_SIZE(150, 100));
	pTextView->SetLocalPos(D3DXVECTOR3(0, CHILDSIZEY - 260, 0));
	pTextView->SetFont(g_pFontManager2->GetFont(cChildWindowFontManager::E_childQUEST));
	pTextView->SetDebugRender(false);
	pTextView->SetDrawTextFormat(DT_CENTER | DT_VCENTER | DT_WORDBREAK);
	pTextView->SetColor(D3DCOLOR_XRGB(0, 175, 128));
	pTextView->SetTag(E_childTextureTEXTVIEW);
	m_pUIRoot->AddChild(pTextView);

	//텍스쳐 로드 버튼
	pButton = new cChildWindowUIButton;
	pButton->AutoRelease();
	pButton->SetTexture("./MapToolUI/open.png", "./MapToolUI/open.png", "./MapToolUI/open.png");
	pButton->SetSize(childST_SIZE(30, 30));
	pButton->SetLocalPos(D3DXVECTOR3(150, CHILDSIZEY - 225, 0));
	pButton->SetDebugRender(false);
	pButton->SetTag(E_childTextureLoadBUTTON);
	pButton->SetDelegate(this);
	m_pUIRoot->AddChild(pButton);

	//로드 버튼
	pButton = new cChildWindowUIButton;
	pButton->AutoRelease();
	pButton->SetTexture("./MapToolUI/resize_yellowButton.png", "./MapToolUI/resize_yellowButton.png", "./MapToolUI/resize_yellowButton.png");
	pButton->SetSize(childST_SIZE(45, 45));
	pButton->SetLocalPos(D3DXVECTOR3(0, CHILDSIZEY - 190, 0));
	pButton->SetDebugRender(false);
	pButton->SetTag(E_childLoadBUTTON);
	pButton->SetDelegate(this);
	m_pUIRoot->AddChild(pButton);

	//세이브 버튼
	pButton = new cChildWindowUIButton;
	pButton->AutoRelease();
	pButton->SetTexture("./MapToolUI/resize_greenButton.png", "./MapToolUI/resize_greenButton.png", "./MapToolUI/resize_greenButton.png");
	pButton->SetSize(childST_SIZE(45, 45));
	pButton->SetLocalPos(D3DXVECTOR3(50, CHILDSIZEY - 190, 0));
	pButton->SetDebugRender(false);
	pButton->SetTag(E_childSaveBUTTON);
	pButton->SetDelegate(this);
	m_pUIRoot->AddChild(pButton);

	//맵툴 모드 버튼
	pButton = new cChildWindowUIButton;
	pButton->AutoRelease();
	pButton->SetTexture("./MapToolUI/resize_off.png", "./MapToolUI/resize_off.png", "./MapToolUI/resize_on.png");
	pButton->SetSize(childST_SIZE(80, 45));
	pButton->SetLocalPos(D3DXVECTOR3(100, CHILDSIZEY - 190, 0));
	pButton->SetDebugRender(false);
	pButton->SetTag(E_childModBUTTON);
	pButton->SetDelegate(this);
	m_pUIRoot->AddChild(pButton);

	//적용 버튼
	pButton = new cChildWindowUIButton;
	pButton->AutoRelease();
	pButton->SetTexture("./MapToolUI/off2.png", "./MapToolUI/off2.png", "./MapToolUI/on2.png");
	pButton->SetSize(childST_SIZE(180, 100));
	pButton->SetLocalPos(D3DXVECTOR3(0, CHILDSIZEY - 140, 0));
	pButton->SetDebugRender(false);
	pButton->SetTag(E_childActivateBUTTON);
	pButton->SetDelegate(this);
	m_pUIRoot->AddChild(pButton);
}

void cChildTool::Update()
{
	if (m_pUIRoot)
		m_pUIRoot->Update();
}

void cChildTool::Render()
{
	g_pD3DDevice2->Clear(NULL, NULL,
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0);
	g_pD3DDevice2->BeginScene();

	if (m_pUIRoot)
		m_pUIRoot->Render(m_pSprite);

	g_pD3DDevice2->EndScene();
	g_pD3DDevice2->Present(0, 0, 0, 0);
}

void cChildTool::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
}

void cChildTool::OnClick(cChildWindowUIButton* pSender)
{
	cChildWindowUITextView* pGridTextView = (cChildWindowUITextView*)m_pUIRoot->GetChildByTag(E_childGridTEXTVIEW);
	cChildWindowUITextView* pGridMinHeightTextView = (cChildWindowUITextView*)m_pUIRoot->GetChildByTag(E_childGridMinHeightTEXTVIEW);
	cChildWindowUITextView* pGridMaxHeightTextView = (cChildWindowUITextView*)m_pUIRoot->GetChildByTag(E_childGridMaxHeightTEXTVIEW);
	cChildWindowUITextView* pGridFlatHeightTextView = (cChildWindowUITextView*)m_pUIRoot->GetChildByTag(E_childGridFlatHeightTEXTVIEW);
	cChildWindowUITextView* pGridSpeedTextView = (cChildWindowUITextView*)m_pUIRoot->GetChildByTag(E_childGridSpeedTEXTVIEW);

	//그리드 설정 Left 버튼
	if (pSender->GetTag() == E_childGridDownBUTTON)
	{
		m_nVertexNum = ((m_nVertexNum - 1) / 2) + 1;
		m_nTileNum = m_nVertexNum - 1;

		SelectGrid();
		m_stringVertexNum = to_string(m_nVertexNum);
		m_stringTileNum = to_string(m_nTileNum);

		pGridTextView->SetText(m_stringTileNum);
	}
	//그리드 설정 Right 버튼
	else if (pSender->GetTag() == E_childGridUpBUTTON)
	{
		m_nVertexNum = ((m_nVertexNum - 1) * 2) + 1;
		m_nTileNum = m_nVertexNum - 1;

		SelectGrid();
		m_stringVertexNum = to_string(m_nVertexNum);
		m_stringTileNum = to_string(m_nTileNum);

		pGridTextView->SetText(m_stringTileNum);
	}
	//그리드 최소 높이 설정 Left 버튼
	else if (pSender->GetTag() == E_childGridMinHeightDownBUTTON)
	{
		m_fVertexMinHeight = m_fVertexMinHeight - 0.5f;

		if (m_fVertexMinHeight < -5.0f)
			m_fVertexMinHeight = -5.0f;

		char temp[100];
		sprintf_s(temp, "%.1ff", m_fVertexMinHeight);
		m_stringVertexHeight = temp;

		pGridMinHeightTextView->SetText(m_stringVertexHeight);
	}
	//그리드 최소 높이 설정 Right 버튼
	else if (pSender->GetTag() == E_childGridMinHeightUpBUTTON)
	{
		m_fVertexMinHeight = m_fVertexMinHeight + 0.5f;

		if (m_fVertexMinHeight > 0.0f)
			m_fVertexMinHeight = 0.0f;

		char temp[100];
		sprintf_s(temp, "%.1ff", m_fVertexMinHeight);
		m_stringVertexHeight = temp;

		pGridMinHeightTextView->SetText(m_stringVertexHeight);
	}
	//그리드 최대 높이 설정 Left 버튼
	else if (pSender->GetTag() == E_childGridMaxHeightDownBUTTON)
	{
		m_fVertexMaxHeight = m_fVertexMaxHeight - 0.5f;

		if (m_fVertexMaxHeight < 0.0f)
			m_fVertexMaxHeight = 0.0f;

		char temp[100];
		sprintf_s(temp, "%.1ff", m_fVertexMaxHeight);
		m_stringVertexHeight = temp;

		pGridMaxHeightTextView->SetText(m_stringVertexHeight);
	}
	//그리드 최대 높이 설정 Right 버튼
	else if (pSender->GetTag() == E_childGridMaxHeightUpBUTTON)
	{
		m_fVertexMaxHeight = m_fVertexMaxHeight + 0.5f;

		if (m_fVertexMaxHeight > 10.0f)
			m_fVertexMaxHeight = 10;

		char temp[100];
		sprintf_s(temp, "%.1ff", m_fVertexMaxHeight);
		m_stringVertexHeight = temp;

		pGridMaxHeightTextView->SetText(m_stringVertexHeight);
	}
	//그리드 평탄화 높이 설정 Left 버튼
	else if (pSender->GetTag() == E_childGridFlatHeightDownBUTTON)
	{
		m_fFlatteningHeight = m_fFlatteningHeight - 0.5f;

		if (m_fFlatteningHeight < 0.0f)
			m_fFlatteningHeight = 0.0f;

		char temp[100];
		sprintf_s(temp, "%.1ff", m_fFlatteningHeight);
		m_stringVertexHeight = temp;

		pGridFlatHeightTextView->SetText(m_stringVertexHeight);
	}
	//그리드 평탄화 높이 설정 Right 버튼
	else if (pSender->GetTag() == E_childGridFlatHeightUpBUTTON)
	{
		m_fFlatteningHeight = m_fFlatteningHeight + 0.5f;

		if (m_fFlatteningHeight > 5.0f)
			m_fFlatteningHeight = 5;

		char temp[100];
		sprintf_s(temp, "%.1ff", m_fFlatteningHeight);
		m_stringVertexHeight = temp;

		pGridFlatHeightTextView->SetText(m_stringVertexHeight);
	}
	//그리드 스피드 설정 Left 버튼
	else if (pSender->GetTag() == E_childGridSpeedDownBUTTON)
	{
		m_fTerrainSpeed = m_fTerrainSpeed - 0.25f;

		if (m_fTerrainSpeed < 0.0f)
			m_fTerrainSpeed = 0.0f;

		char temp[100];
		sprintf_s(temp, "%.1ff", m_fTerrainSpeed);
		m_stringVertexHeight = temp;

		pGridSpeedTextView->SetText(m_stringVertexHeight);
	}
	//그리드 스피드 설정 Right 버튼
	else if (pSender->GetTag() == E_childGridSpeedUpBUTTON)
	{
		m_fTerrainSpeed = m_fTerrainSpeed + 0.25f;

		if (m_fTerrainSpeed > 5.0f)
			m_fTerrainSpeed = 5;

		char temp[100];
		sprintf_s(temp, "%.1ff", m_fTerrainSpeed);
		m_stringVertexHeight = temp;

		pGridSpeedTextView->SetText(m_stringVertexHeight);
	}
	//X 파일 버튼
	else if (pSender->GetTag() == E_childXFileLoadBUTTON)
	{
		m_isXFile = true;
	}
	//텍스쳐 버튼
	else if (pSender->GetTag() == E_childTextureLoadBUTTON)
	{
		m_isTexture = true;
	}
	//세이브 버튼
	else if (pSender->GetTag() == E_childSaveBUTTON)
	{
		m_isSave = true;
	}
	//로드 버튼
	else if (pSender->GetTag() == E_childLoadBUTTON)
	{
		m_isLoad = true;
	}
	//모드 버튼
	else if (pSender->GetTag() == E_childModBUTTON)
	{
		m_isMod = !m_isMod;
	}
	//적용 버튼
	else if (pSender->GetTag() == E_childActivateBUTTON)
	{
		m_isActivate = true;
	}
}

void cChildTool::SelectGrid()
{
	//최대값 설정
	if (m_nVertexNum >= 129)
		m_nVertexNum = 129;
	if (m_nTileNum >= 128)
		m_nTileNum = 128;
	//최소값 설정
	if (m_nVertexNum <= 17)
		m_nVertexNum = 17;
	if (m_nTileNum <= 16)
		m_nTileNum = 16;
}
