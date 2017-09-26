#include "stdafx.h"
#include "cAchievementBox.h"
#include "cUIImageView.h"
#include "cUITextView.h"


cAchievementBox::cAchievementBox()
	: m_pSprite2(NULL)
	, m_pUIRoot(NULL)
	, m_isQuestSuccess(false)
	, m_isAnimationEnd(false)
	, m_nDir(+2)
	, m_fCount(0.0f)
	, m_fTime(1.0f)
	, m_nPlayCnt(0)
{
	ZeroMemory(&m_stImageInfo, sizeof(D3DXIMAGE_INFO));
}


cAchievementBox::~cAchievementBox()
{
	SAFE_RELEASE(m_pSprite2);
}

void cAchievementBox::Setup()
{
	D3DXCreateSprite(g_pD3DDevice, &m_pSprite2);

	LPDIRECT3DTEXTURE9 pTexture;
	D3DXIMAGE_INFO stImageInfo;

	char str[1024] = "./UI/AchUI.png";
	g_pTextureManager->AddTexture(str, str, &pTexture, &stImageInfo);

	cUIImageView* pImageView = new cUIImageView;
	pImageView->AutoRelease();
	pImageView->SetTexture(pTexture);
	pImageView->SetSize(ST_SIZE(stImageInfo.Width, stImageInfo.Height));
	pImageView->SetLocalPos(D3DXVECTOR3(WINSIZEX / 2 - (stImageInfo.Width / 2), 0, 0));
	pImageView->SetDebugRender(false);
	m_pUIRoot = pImageView;

	cUITextView* pTextView = new cUITextView;
	pTextView->AutoRelease();
	m_sQuestName = "Achievement";
	pTextView->SetText(m_sQuestName);
	pTextView->SetSize(ST_SIZE(250, 50));
	pTextView->SetLocalPos(D3DXVECTOR3(stImageInfo.Width / 2 - 125, stImageInfo.Height / 2 - 25, 0));
	pTextView->SetFont(g_pFontManager->GetFont(cFontManager::E_QUEST));
	pTextView->SetDebugRender(false);
	pTextView->SetDrawTextFormat(DT_CENTER | DT_VCENTER | DT_WORDBREAK);
	pTextView->SetColor(D3DCOLOR_XRGB(255, 255, 255));
	pTextView->SetTag(E_AchievementTEXT);
	m_pUIRoot->AddChild(pTextView);

	InitPosition();
}

void cAchievementBox::Update(D3DXVECTOR3* pos)
{
	if (m_pUIRoot)
		m_pUIRoot->Update();

	//퀘스트가 성공하면
	if (m_isQuestSuccess)
	{
		AchievementBarUpdate(); //업적상태바 움직이기
	}
}

void cAchievementBox::Render()
{
	if (m_pUIRoot)
		m_pUIRoot->Render(m_pSprite2);
}

void cAchievementBox::InitPosition()
{
	m_pUIRoot->SetLocalPos(D3DXVECTOR3(m_pUIRoot->GetLocalPos().x, -100, 0));
}

void cAchievementBox::SetQuestName(string QuestName)
{
	cUITextView* pTextView = (cUITextView*)m_pUIRoot->GetChildByTag(E_AchievementTEXT);
	m_sQuestName = QuestName;
	pTextView->SetText(m_sQuestName);
}

void cAchievementBox::AchievementBarUpdate()
{
	if (m_nPlayCnt > 0) return;

	//업적바 움직이기
	m_pUIRoot->SetLocalPos(D3DXVECTOR3(m_pUIRoot->GetLocalPos().x, m_pUIRoot->GetLocalPos().y + m_nDir, 0));

	//사운드 추가
	static int soundPlayCntAchieve = 0;
	if (soundPlayCntAchieve == 0 && !g_pSoundManager->isPlaySound("achievement"))
	{
		g_pSoundManager->play("achievement", 0.5f);
		++soundPlayCntAchieve;
	}

	//화면에 업적바가 다 내려왔으면
	if (m_pUIRoot->GetLocalPos().y > 0)
	{
		//0으로 고정
		m_pUIRoot->SetLocalPos(D3DXVECTOR3(m_pUIRoot->GetLocalPos().x, 0, 0));
		//애니메이션 끝
		m_isAnimationEnd = true;
	}

	//애니메이션이 끝났으면
	if (m_isAnimationEnd)
		m_fCount += g_pTimeManager->getElapsedTime(); //업적 보여주는 시간을 잰다.

	//애니메이션이 끝났고, 업적 보여준 시간이 m_fTime가 지나면
	if (m_isAnimationEnd && m_fCount > m_fTime)
		m_nDir = -2; //위로 올라가기
	else
		m_nDir = 2;  //아래로 내려가기

	//애니메이션이 끝났고, 업적바가 위로 완전히 다 올라갔을때
	if (m_isAnimationEnd && m_pUIRoot->GetLocalPos().y < -100)
	{
		//-100으로 고정시켜주고
		InitPosition();
		//퀘스트 성공을 false해준다. (다음 퀘스트 업적 표시를 위해서 초기화)
		m_isQuestSuccess = false;
		m_isAnimationEnd = false;
		m_fCount = 0.0f;

		++m_nPlayCnt;

		if (soundPlayCntAchieve > 0 && !g_pSoundManager->isPlaySound("achievement"))
		{
			g_pSoundManager->stop("achievement");
			soundPlayCntAchieve = 0;
		}
	}
}