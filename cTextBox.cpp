#include "stdafx.h"
#include "cTextBox.h"
#include "cUIImageView.h"
#include "cUITextView.h"


cTextBox::cTextBox()
	: m_pSprite(NULL)
	, m_pUIRoot(NULL)
	, m_nTextIndex(0)
	, m_fTextCount(0.0f)
	, m_fTextSpeed(1.0f)
	, m_isDialouge(false)
	, m_isOK(false)
{
	ZeroMemory(&m_stImageInfo, sizeof(D3DXIMAGE_INFO));
}


cTextBox::~cTextBox()
{
	SAFE_RELEASE(m_pSprite);
}

void cTextBox::Setup()
{
	D3DXCreateSprite(g_pD3DDevice, &m_pSprite);

	LPDIRECT3DTEXTURE9 pTexture;
	D3DXIMAGE_INFO stImageInfo;

	char str[1024] = "./UI/TextBoxAlpha.png";
	g_pTextureManager->AddTexture(str, str, &pTexture, &stImageInfo);

	cUIImageView* pImageView = new cUIImageView;
	pImageView->AutoRelease();
	pImageView->SetTexture(pTexture);
	pImageView->SetSize(ST_SIZE(stImageInfo.Width, stImageInfo.Height));
	pImageView->SetLocalPos(D3DXVECTOR3(WINSIZEX / 2 - (stImageInfo.Width / 2), WINSIZEY - (stImageInfo.Height + 10), 0));
	pImageView->SetDebugRender(false);
	m_pUIRoot = pImageView;

	char str2[1024] = "./UI/NameBoxAlpha_White.png";
	g_pTextureManager->AddTexture(str2, str2, &pTexture, &stImageInfo);
	pImageView = new cUIImageView;
	pImageView->AutoRelease();
	pImageView->SetTexture(pTexture);
	pImageView->SetSize(ST_SIZE(stImageInfo.Width, stImageInfo.Height));
	pImageView->SetLocalPos(D3DXVECTOR3(50, -40, 0));
	pImageView->SetDebugRender(false);
	m_pUIRoot->AddChild(pImageView);

	cUITextView* pTextView = new cUITextView;
	pTextView->AutoRelease();
	m_sName = "Name";
	pTextView->SetText(m_sName);
	pTextView->SetSize(ST_SIZE(250, 50));
	pTextView->SetLocalPos(D3DXVECTOR3(75, -30, 0));
	pTextView->SetFont(g_pFontManager->GetFont(cFontManager::E_QUEST));
	pTextView->SetDebugRender(false);
	pTextView->SetDrawTextFormat(DT_CENTER | DT_VCENTER | DT_WORDBREAK);
	pTextView->SetColor(D3DCOLOR_XRGB(255, 255, 255));
	pTextView->SetTag(E_NAME);
	m_pUIRoot->AddChild(pTextView);

	pTextView = new cUITextView;
	pTextView->AutoRelease();
	m_sText = "Dialogue";
	m_sEmptyText = "";
	pTextView->SetText(m_sEmptyText);
	pTextView->SetSize(ST_SIZE(1050, 200));
	pTextView->SetLocalPos(D3DXVECTOR3(80, 50, 0));
	pTextView->SetFont(g_pFontManager->GetFont(cFontManager::E_QUEST));
	pTextView->SetDebugRender(false);
	pTextView->SetDrawTextFormat(DT_LEFT | DT_TOP | DT_WORDBREAK);
	pTextView->SetColor(D3DCOLOR_XRGB(255, 255, 255));
	pTextView->SetTag(E_DIALOGUE);
	m_pUIRoot->AddChild(pTextView);

	cUIButton* pButton = NULL;
	pButton = new cUIButton;
	pButton->AutoRelease();
	pButton->SetTexture("./UI/OKButton.png", "./UI/OKButton.png", "./UI/OKButton.png");
	pButton->SetSize(ST_SIZE(150, 50));
	pButton->SetLocalPos(D3DXVECTOR3(975, 200, 0));
	pButton->SetDebugRender(false);
	pButton->SetTag(E_OK);
	pButton->SetDelegate(this);
	m_pUIRoot->AddChild(pButton);

	pTextView = new cUITextView;
	pTextView->AutoRelease();
	pTextView->SetText("OK");
	pTextView->SetSize(ST_SIZE(150, 50));
	pTextView->SetLocalPos(D3DXVECTOR3(975, 200, 0));
	pTextView->SetFont(g_pFontManager->GetFont(cFontManager::E_QUEST));
	pTextView->SetDebugRender(false);
	pTextView->SetDrawTextFormat(DT_CENTER | DT_VCENTER | DT_WORDBREAK);
	pTextView->SetColor(D3DCOLOR_XRGB(255, 255, 0));
	m_pUIRoot->AddChild(pTextView);
}

void cTextBox::Update(D3DXVECTOR3* pos)
{
	//시간 증가
	m_fTextCount += g_pTimeManager->getElapsedTime();

	//읽는 시간보다 시간이 커지면
	if (m_fTextCount > m_fTextSpeed)
	{
		//재생 허락 (T/F) && 텍스트 인덱스가 지정된 스트링 사이즈보다 작을때 까지.
		if (m_isDialouge && m_nTextIndex < m_sText.size())
		{
			//한 글자를 넣어준다.
			m_sEmptyText += m_sText[m_nTextIndex];
			
			//?라는 의미는 한글이라는 의미 (한글은 2byte씩 사용되기 때문에)
			//한글 예외처리로 인하여 특수문자 ?은 사용 할수 없다.
			if (m_sText[m_nTextIndex] == '?')
			{
				m_nTextIndex += 2;
			}
			else //영문이면 한개 증가
			{
				//인덱스를 증가시킨다.
				m_nTextIndex++;
			}
			
			//시간을 초기화 시킨다.
			m_fTextCount = 0.0f;

			//E_DIALOGUE권한을 얻어서 텍스트를 설정한다.
			cUITextView* pTextView = (cUITextView*)m_pUIRoot->GetChildByTag(E_DIALOGUE);
			pTextView->SetText(m_sEmptyText);
		}
	}

	if (pos)
	{
		D3DXVECTOR3 screenPos = *pos;
		D3DXMATRIXA16 matView, matProj;
		D3DVIEWPORT9 vp;

		g_pD3DDevice->GetTransform(D3DTS_VIEW, &matView);
		g_pD3DDevice->GetTransform(D3DTS_PROJECTION, &matProj);
		g_pD3DDevice->GetViewport(&vp);

		D3DXVec3TransformCoord(&screenPos, &screenPos, &(matView * matProj));

		float screenX, screenY;
		screenX = screenPos.x * vp.Width / 2;
		screenY = -screenPos.y * vp.Height / 2;

		screenPos = D3DXVECTOR3(screenX, screenY, 0.0f);

		m_pUIRoot->SetLocalPos(screenPos);

		//m_pUIRoot->SetLocalPos(*pos);
	}

	if (m_pUIRoot)
		m_pUIRoot->Update();
}

void cTextBox::Render()
{
	//OK를 누르지 않으면 그려준다.
	if (!m_isOK)
	{
		if (m_pUIRoot)
			m_pUIRoot->Render(m_pSprite);
	}
}

void cTextBox::OnClick(cUIButton* pSender)
{
	if (pSender->GetTag() == E_OK)
	{
		m_isOK = true;	//OK를 누르면 끝낸다.

		//텍스트 초기화
		m_sEmptyText = "";
		m_fTextCount = 0.0f;
		m_nTextIndex = 0;

		cUITextView* pTextView = (cUITextView*)m_pUIRoot->GetChildByTag(E_DIALOGUE);
		pTextView->SetText(m_sEmptyText);
	}
}

void cTextBox::InitializeString(string Name, string Dialouge, bool isDialouge)
{
	//이전에 사용했을 경우를 고려하여 초기화 재설정
	m_isOK = false;				//문자열이 끝나지 않은 상태로 초기화
	m_isDialouge = isDialouge;  //바로 재생할지, 기다릴지
	m_nTextIndex = 0;			//인덱스 초기화

	cUITextView* pTextView = (cUITextView*)m_pUIRoot->GetChildByTag(E_DIALOGUE);
	m_sEmptyText = "";			//빈 문자열로 설정해 놓는다.
	pTextView->SetText(m_sEmptyText);

	m_sText = Dialouge;			//재생 할 문자열을 설정한다.

	pTextView = (cUITextView*)m_pUIRoot->GetChildByTag(E_NAME);
	m_sName = Name;
	pTextView->SetText(m_sName);//빈 문자열로 설정해 놓는다.
}
