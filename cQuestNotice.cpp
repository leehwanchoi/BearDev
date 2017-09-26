#include "stdafx.h"
#include "cQuestNotice.h"
#include "cUIImageView.h"
#include "cUITextView.h"


cQuestNotice::cQuestNotice()
	: m_pSprite(NULL)
	, m_pUIRoot(NULL)
	, m_nPresentValue(0)
	, m_nQuestValue(0)
	, m_isQuestStart(false)
	, m_isQuestEnd(false)
	, m_nDir(1) //x좌표 움직이는 방향 : +1 (오른쪽) -1 (왼쪽)
{
	ZeroMemory(&m_stImageInfo, sizeof(D3DXIMAGE_INFO));
}


cQuestNotice::~cQuestNotice()
{
	SAFE_RELEASE(m_pSprite);
}

void cQuestNotice::Setup()
{
	D3DXCreateSprite(g_pD3DDevice, &m_pSprite);

	LPDIRECT3DTEXTURE9 pTexture;
	D3DXIMAGE_INFO stImageInfo;

	char str[1024] = "./UI/QUESTUI_Alpha.png";
	g_pTextureManager->AddTexture(str, str, &pTexture, &stImageInfo);

	cUIImageView* pImageView = new cUIImageView;
	pImageView->AutoRelease();
	pImageView->SetTexture(pTexture);
	pImageView->SetSize(ST_SIZE(stImageInfo.Width, stImageInfo.Height));
	pImageView->SetLocalPos(D3DXVECTOR3(-250, (WINSIZEY / 2) / 5, 0));
	pImageView->SetDebugRender(true);
	m_pUIRoot = pImageView;

	cUITextView* pTextView = new cUITextView;
	pTextView->AutoRelease();
	m_sQuestName = "Test Quest";
	m_sFullNotice = "(" + to_string(m_nPresentValue) + "/" + to_string(m_nQuestValue) + ")" + m_sQuestName;
	pTextView->SetText(m_sFullNotice);
	pTextView->SetSize(ST_SIZE(250, 50));
	pTextView->SetLocalPos(D3DXVECTOR3(stImageInfo.Width / 2 - 125, stImageInfo.Height / 2 - 25, 0));
	pTextView->SetFont(g_pFontManager->GetFont(cFontManager::E_QUEST));
	pTextView->SetDebugRender(true);
	pTextView->SetDrawTextFormat(DT_CENTER | DT_VCENTER | DT_WORDBREAK);
	pTextView->SetColor(D3DCOLOR_XRGB(255, 255, 255));
	pTextView->SetTag(E_QuestNotice);
	m_pUIRoot->AddChild(pTextView);
}

void cQuestNotice::Update(D3DXVECTOR3* pos)
{
	QuestUpdate();

	if (m_pUIRoot)
		m_pUIRoot->Update();
}

void cQuestNotice::Render()
{
	if (m_pUIRoot)
		m_pUIRoot->Render(m_pSprite);
}

void cQuestNotice::InitializeString(int QuestValue, string QuestName)
{
	//퀘스트 변수 초기화
	m_isQuestStart = true;	//초기화 시키면 바로 퀘스트 시작(Render 시작)
	m_isQuestEnd = false;   //퀘스트 완료 false
	m_nPresentValue = 0;	//현재 값 0
	m_nQuestValue = QuestValue; //지정값으로 초기화
	m_sQuestName = QuestName;	//지정값으로 초기화

	//아래 형식으로 표시
	m_sFullNotice = "(" + to_string(m_nPresentValue) + "/" + to_string(m_nQuestValue) + ")" + m_sQuestName;
	cUITextView* pTextView = (cUITextView*)m_pUIRoot->GetChildByTag(E_QuestNotice);
	pTextView->SetText(m_sFullNotice);
}

void cQuestNotice::PresentValueIncrement()
{
	//현재 값 증가
	m_nPresentValue++;

	//현재 값이 퀘스트 목표치와 같으면 고정
	if (m_nPresentValue >= m_nQuestValue)
		m_nPresentValue = m_nQuestValue;

	//아래 형식으로 표시
	m_sFullNotice = "(" + to_string(m_nPresentValue) + "/" + to_string(m_nQuestValue) + ")" + m_sQuestName;
	cUITextView* pTextView = (cUITextView*)m_pUIRoot->GetChildByTag(E_QuestNotice);
	pTextView->SetText(m_sFullNotice);
}

void cQuestNotice::QuestUpdate()
{
	//퀘스트가 시작되었으면 퀘스트창 움직이기
	if (m_isQuestStart)
	{
		//위치 변경
		m_pUIRoot->SetLocalPos(D3DXVECTOR3(m_pUIRoot->GetLocalPos().x + (+m_nDir), m_pUIRoot->GetLocalPos().y, 0));

		//화면에 완전히 보여지면 고정
		if (m_pUIRoot->GetLocalPos().x > 0)
			m_pUIRoot->SetLocalPos(D3DXVECTOR3(0, m_pUIRoot->GetLocalPos().y, 0));
	}

	//퀘스트가 끝났으면 퀘스트창 숨기기
	if (m_isQuestEnd)
	{
		//위치 변경
		m_pUIRoot->SetLocalPos(D3DXVECTOR3(m_pUIRoot->GetLocalPos().x + (-m_nDir), m_pUIRoot->GetLocalPos().y, 0));

		//화면에 완전히 가려지면 고정
		if (m_pUIRoot->GetLocalPos().x < -250)
			m_pUIRoot->SetLocalPos(D3DXVECTOR3(-250, m_pUIRoot->GetLocalPos().y, 0));
	}

	//현재값과 퀘스트 목표값이 같으면
	if (m_nPresentValue == m_nQuestValue)
	{
		//퀘스트 끝
		m_isQuestEnd = true;
		m_isQuestStart = false;
	}
}
