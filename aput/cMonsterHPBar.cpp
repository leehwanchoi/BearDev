#include "stdafx.h"
#include "cMonsterHPBar.h"
#include "cUIImageView.h"


cMonsterHPBar::cMonsterHPBar()
	: m_pSprite(NULL)
	, m_pUIRoot(NULL)
	, m_pUIHP(NULL)
	, m_isDamaged(false)
	, m_fCurrentHP(HPENDX)
	, m_eDamagedState(NoneDamaged)
	, m_isDeath(false)
	, m_isDamageOnUpdate(false)
{
	ZeroMemory(&m_stImageInfo, sizeof(D3DXIMAGE_INFO));
}


cMonsterHPBar::~cMonsterHPBar()
{
	SAFE_RELEASE(m_pSprite);
}

void cMonsterHPBar::Setup()
{
	D3DXCreateSprite(g_pD3DDevice, &m_pSprite);

	LPDIRECT3DTEXTURE9 pTexture;
	D3DXIMAGE_INFO stImageInfo;

	char str0[1024] = "./UI/MonsterFaceUI.png";
	g_pTextureManager->AddTexture(str0, str0, &pTexture, &stImageInfo);
	cUIImageView* pImageView = new cUIImageView;
	pImageView->AutoRelease();
	pImageView->SetTexture(pTexture);
	pImageView->SetSize(ST_SIZE(stImageInfo.Width, stImageInfo.Height));
	pImageView->SetLocalPos(D3DXVECTOR3(WINSIZEX - 465, 0, 0));
	pImageView->SetDebugRender(false);
	m_pUIRoot = pImageView;

	char str[1024] = "./UI/HPBAR_Frame_resize.png";
	g_pTextureManager->AddTexture(str, str, &pTexture, &stImageInfo);
	pImageView = new cUIImageView;
	pImageView->AutoRelease();
	pImageView->SetTexture(pTexture);
	pImageView->SetSize(ST_SIZE(stImageInfo.Width, stImageInfo.Height));
	pImageView->SetLocalPos(D3DXVECTOR3(160/*-280*/, 80, 0));
	pImageView->SetDebugRender(false);
	m_pUIRoot->AddChild(pImageView);

	char str2[1024] = "./UI/HPBAR_HP_resize.png";
	g_pTextureManager->AddTexture(str2, str2, &pTexture, &stImageInfo);
	pImageView = new cUIImageView;
	pImageView->AutoRelease();
	pImageView->SetTexture(pTexture);
	pImageView->SetSize(ST_SIZE(stImageInfo.Width, stImageInfo.Height));
	pImageView->SetLocalPos(D3DXVECTOR3(160/*-280*/, 80, 0));
	pImageView->SetDebugRender(false);
	m_pUIRoot->AddChild(pImageView);
	//HP ���¸� �����ϱ� ���� HP�̹����� ���� �����Ѵ�.
	m_pUIHP = pImageView;

	char str3[1024] = "./UI/HPBAR_Cover_Heart_resize.png";
	g_pTextureManager->AddTexture(str3, str3, &pTexture, &stImageInfo);
	pImageView = new cUIImageView;
	pImageView->AutoRelease();
	pImageView->SetTexture(pTexture);
	pImageView->SetSize(ST_SIZE(stImageInfo.Width, stImageInfo.Height));
	pImageView->SetLocalPos(D3DXVECTOR3(160/*-280*/, 80, 0));
	pImageView->SetDebugRender(false);
	m_pUIRoot->AddChild(pImageView);
}

void cMonsterHPBar::Update(D3DXVECTOR3* pos)
{
	//HP �ִϸ��̼� �� HP ����
	HPUpdate();

	if (m_pUIRoot)
		m_pUIRoot->Update();
}

void cMonsterHPBar::Render()
{
	if (m_pUIRoot)
		m_pUIRoot->Render(m_pSprite);
}

void cMonsterHPBar::SetFullHP()
{
	//Full HP�� ����
	m_pUIHP->SetSize(ST_SIZE(HPENDX, 50));
}

void cMonsterHPBar::HPUpdate()
{
	if (m_isDamaged)
	{
		//������ ���� ���·� ���� (���ҽ�ų ������ ������ ����)
		m_eDamagedState = SetDamaged;
		//enum�� ����������, �ܺο��� ������ ������ �ʱ�ȭ
		m_isDamaged = false;
	}


	if (m_eDamagedState == SetDamaged)
	{

		//���� HP�� 10/1�� ���ҽ�Ų��.
		m_fCurrentHP = m_pUIHP->GetSize().fWidth - (HPPERCENT / 10);

		//������ ���� ���·� ����
		m_eDamagedState = Damaged;
	}


	//������ ���� ���¶��
	if (m_eDamagedState == Damaged || m_isDamageOnUpdate)
	{
		//���ҽ�Ų HP��ŭ �ִϸ��̼��� �����ش�.
		m_pUIHP->SetSize(ST_SIZE(m_pUIHP->GetSize().fWidth - 70.0f * g_pTimeManager->getElapsedTime(), 50));

		//���� ��Ų HP �ִϸ��̼��� ���� HP���� �۾�����
		if (m_pUIHP->GetSize().fWidth <= m_fCurrentHP)
		{
			//���� HP�� ����
			m_pUIHP->SetSize(ST_SIZE(m_fCurrentHP, 50));
			//������ ���� ���� ����
			m_eDamagedState = NoneDamaged;
			m_isDamageOnUpdate = false;
		}
	}

	if (m_isSetHp)
	{
		//���� HP�� ����
		m_pUIHP->SetSize(ST_SIZE(m_fCurrentHP, 50));
		m_isSetHp = false;
	}

	//�ּ� HP, �ִ� HP ����
	//HP�� 0%���� �۾����� ���
	if (m_pUIHP->GetSize().fWidth < HPSTARTX)
	{
		m_pUIHP->SetSize(ST_SIZE(HPSTARTX, 50)); //0%���� ����
		m_isDeath = true; //���� ���·� ����
	}

	//HP�� 100%�� �Ѿ�� ���
	if (m_pUIHP->GetSize().fWidth > HPENDX)
		m_pUIHP->SetSize(ST_SIZE(HPENDX, 50)); //100%���� ����
}