#include "stdafx.h"
#include "cHPBar.h"
#include "cUIImageView.h"


cHPBar::cHPBar()
	: m_pSprite(NULL)
	, m_pUIRoot(NULL)
	, m_pUIHP(NULL)
	, m_isDamaged(false)
	, m_isOneKill(false)
	, m_fCurrentHP(HPENDX)
	, m_eDamagedState(NoneDamaged)
	, m_isDeath(false)
	, m_isDamageOnUpdate(false)
{
	ZeroMemory(&m_stImageInfo, sizeof(D3DXIMAGE_INFO));
}


cHPBar::~cHPBar()
{
	SAFE_RELEASE(m_pSprite);
}

void cHPBar::Setup()
{
	D3DXCreateSprite(g_pD3DDevice, &m_pSprite);

	LPDIRECT3DTEXTURE9 pTexture;
	D3DXIMAGE_INFO stImageInfo;

	char str0[1024] = "./UI/characterFaceUI.png";
	g_pTextureManager->AddTexture(str0, str0, &pTexture, &stImageInfo);
	cUIImageView* pImageView = new cUIImageView;
	pImageView->AutoRelease();
	pImageView->SetTexture(pTexture);
	pImageView->SetSize(ST_SIZE(stImageInfo.Width, stImageInfo.Height));
	pImageView->SetLocalPos(D3DXVECTOR3(5, 0, 0));
	pImageView->SetDebugRender(false);
	m_pUIRoot = pImageView;

	char str[1024] = "./UI/HPBAR_Frame_resize.png";
	g_pTextureManager->AddTexture(str, str, &pTexture, &stImageInfo);
	pImageView = new cUIImageView;
	pImageView->AutoRelease();
	pImageView->SetTexture(pTexture);
	pImageView->SetSize(ST_SIZE(stImageInfo.Width, stImageInfo.Height));
	pImageView->SetLocalPos(D3DXVECTOR3(160, 80, 0));
	pImageView->SetDebugRender(false);
	m_pUIRoot->AddChild(pImageView);

	char str2[1024] = "./UI/HPBAR_HP_resize.png";
	g_pTextureManager->AddTexture(str2, str2, &pTexture, &stImageInfo);
	pImageView = new cUIImageView;
	pImageView->AutoRelease();
	pImageView->SetTexture(pTexture);
	pImageView->SetSize(ST_SIZE(stImageInfo.Width, stImageInfo.Height));
	pImageView->SetLocalPos(D3DXVECTOR3(160, 80, 0));
	pImageView->SetDebugRender(false);
	m_pUIRoot->AddChild(pImageView);
	//HP 상태를 조절하기 위해 HP이미지만 따로 저장한다.
	m_pUIHP = pImageView;

	char str3[1024] = "./UI/HPBAR_Cover_Heart_resize.png";
	g_pTextureManager->AddTexture(str3, str3, &pTexture, &stImageInfo);
	pImageView = new cUIImageView;
	pImageView->AutoRelease();
	pImageView->SetTexture(pTexture);
	pImageView->SetSize(ST_SIZE(stImageInfo.Width, stImageInfo.Height));
	pImageView->SetLocalPos(D3DXVECTOR3(160, 80, 0));
	pImageView->SetDebugRender(false);
	m_pUIRoot->AddChild(pImageView);
}

void cHPBar::Update(D3DXVECTOR3* pos)
{
	//HP 애니메이션 및 HP 설정
	HPUpdate();

	if (m_pUIRoot)
		m_pUIRoot->Update();
}

void cHPBar::Render()
{
	if (m_pUIRoot)
		m_pUIRoot->Render(m_pSprite);
}

void cHPBar::SetFullHP()
{
	//Full HP로 변경
	m_pUIHP->SetSize(ST_SIZE(HPENDX, 50));
}

void cHPBar::HPUpdate()
{
	if (m_isDamaged)
	{
		//데미지 설정 상태로 변경 (감소시킬 데미지 설정을 위해)
		m_eDamagedState = SetDamaged;
		//enum을 설정했으면, 외부에서 설정한 변수는 초기화
		m_isDamaged = false;
	}


	if (m_eDamagedState == SetDamaged)
	{

		if (m_isOneKill)
		{
			m_fCurrentHP = 0;
			m_isOneKill = false;
		}
		else
		{
			//현재 HP를 10/1을 감소시킨다.
			m_fCurrentHP = m_pUIHP->GetSize().fWidth - (HPPERCENT / 10);
		}

		//데미지 받은 상태로 변경
		m_eDamagedState = Damaged;
	}


	//데미지 받은 상태라면
	if (m_eDamagedState == Damaged || m_isDamageOnUpdate)
	{
		//감소시킨 HP만큼 애니메이션을 보여준다.
		if (!m_isOneKill)
			m_pUIHP->SetSize(ST_SIZE(m_pUIHP->GetSize().fWidth - 70.0f * g_pTimeManager->getElapsedTime(), 50));

		//감소 시킨 HP 애니메이션이 현재 HP보다 작아지면
		if (m_pUIHP->GetSize().fWidth <= m_fCurrentHP)
		{
			//현재 HP로 고정
			m_pUIHP->SetSize(ST_SIZE(m_fCurrentHP, 50));
			//데미지 받은 상태 해제
			m_eDamagedState = NoneDamaged;
			m_isDamageOnUpdate = false;
		}

		if (m_isOneKill)
		{
			//현재 HP로 고정
			m_pUIHP->SetSize(ST_SIZE(m_fCurrentHP, 50));
			//데미지 받은 상태 해제
			m_eDamagedState = NoneDamaged;
			m_isDamageOnUpdate = false;
		}
	}

	if (m_isSetHp)
	{
		//현재 HP로 고정
		m_pUIHP->SetSize(ST_SIZE(m_fCurrentHP, 50));
		m_isSetHp = false;
	}

	//최소 HP, 최대 HP 설정
	//HP가 0%보다 작아지는 경우
	if (m_pUIHP->GetSize().fWidth < HPSTARTX)
	{
		m_pUIHP->SetSize(ST_SIZE(HPSTARTX, 50)); //0%으로 고정
		m_isDeath = true; //죽은 상태로 변경
	}

	//HP가 100%이 넘어가는 경우
	if (m_pUIHP->GetSize().fWidth > HPENDX)
		m_pUIHP->SetSize(ST_SIZE(HPENDX, 50)); //100%으로 고정
}
