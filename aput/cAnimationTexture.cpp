#include "stdafx.h"
#include "cAnimationTexture.h"


cAnimationTexture::cAnimationTexture()
	: m_pTexture(NULL)
	, m_fX(0)
	, m_fY(0)
	, m_isPause(false)
	, m_isFront(true)
	, m_nFullFrame(0)
	, m_fOneFrame(0)
	, m_fTime(0.0f)
	, m_fSetTime(0.0f)
{
	D3DXMatrixIdentity(&m_matW);
}


cAnimationTexture::~cAnimationTexture()
{
}

void cAnimationTexture::Load(char* lpFileName, int FrameNum)
{
	g_pTextureManager->AddTexture(lpFileName, lpFileName, &m_pTexture);
	m_nFullFrame = FrameNum;
	m_fOneFrame = 1.0f / m_nFullFrame;
	m_fSetTime = 1.0f / m_nFullFrame;

	ST_PT_VERTEX stVertex[4];
	stVertex[0].p = D3DXVECTOR3(-3, -3, 0);
	stVertex[0].t = D3DXVECTOR2(0, 1);

	stVertex[1].p = D3DXVECTOR3(-3, 3, 0);
	stVertex[1].t = D3DXVECTOR2(0, 0);

	stVertex[2].p = D3DXVECTOR3(3, 3, 0);
	stVertex[2].t = D3DXVECTOR2(m_fOneFrame, 0);

	stVertex[3].p = D3DXVECTOR3(3, -3, 0);
	stVertex[3].t = D3DXVECTOR2(m_fOneFrame, 1);

	m_vecPTVertex.push_back(stVertex[0]);
	m_vecPTVertex.push_back(stVertex[1]);
	m_vecPTVertex.push_back(stVertex[2]);

	m_vecPTVertex.push_back(stVertex[0]);
	m_vecPTVertex.push_back(stVertex[2]);
	m_vecPTVertex.push_back(stVertex[3]);

	//�̹����� �����ϱ� ���� ��ǥ�� ���� ����
	m_textureCoor[0] = stVertex[0].t;
	m_textureCoor[1] = stVertex[1].t;
	m_textureCoor[2] = stVertex[2].t;
	m_textureCoor[3] = stVertex[3].t;
}

void cAnimationTexture::Update()
{
	//�ð� ����
	m_fTime += g_pTimeManager->getElapsedTime();

	//������ �ƴϰ�
	if (!m_isPause)
	{
		//�ð��� �����ð����� Ŀ����
		if (m_fSetTime < m_fTime)
		{
			//������ ����̸�
			if (m_isFront)
				NextImage();

			//�ڷ� ����̸�
			else
				PrevImage();

			m_fTime = 0;
		}
	}
}

void cAnimationTexture::Render()
{
	g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);		// ���� ���� �ɼ� Ų��.
	g_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);		// Z����(���� ����)�� ���ؼ� �׸��� ������ �������� �ɼ��� ����. : �������� ǥ���� �� �ӵ� ��� ȿ��

	g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	g_pD3DDevice->SetFVF(ST_PT_VERTEX::FVF);
	g_pD3DDevice->SetMaterial(&WHITE_MTRL);
	g_pD3DDevice->SetTexture(0, m_pTexture);
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, false);
	g_pD3DDevice->SetTransform(D3DTS_WORLD, &m_matW);

	g_pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_vecPTVertex.size() / 3,
		&m_vecPTVertex[0], sizeof(ST_PT_VERTEX));

	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, true);
	g_pD3DDevice->SetFVF(ST_PNT_VERTEX::FVF);
	g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);	// �⺻ �� ����
	g_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);			// �⺻ �� ����
}

void cAnimationTexture::PrevImage()
{
	//�� ������ ����
	m_textureCoor[0].x -= m_fOneFrame;
	m_textureCoor[1].x -= m_fOneFrame;
	m_textureCoor[2].x -= m_fOneFrame;
	m_textureCoor[3].x -= m_fOneFrame;

	//�� ó�� �������̸� ������ ����������
	if (m_textureCoor[3].x <= 0)
	{
		m_textureCoor[0].x = 1.0f - m_fOneFrame;
		m_textureCoor[1].x = 1.0f - m_fOneFrame;
		m_textureCoor[2].x = 1.0f;
		m_textureCoor[3].x = 1.0f;
	}

	//������ ����
	m_vecPTVertex[0].t.x = m_textureCoor[0].x;
	m_vecPTVertex[1].t.x = m_textureCoor[1].x;
	m_vecPTVertex[2].t.x = m_textureCoor[2].x;

	m_vecPTVertex[3].t.x = m_textureCoor[0].x;
	m_vecPTVertex[4].t.x = m_textureCoor[2].x;
	m_vecPTVertex[5].t.x = m_textureCoor[3].x;
}

void cAnimationTexture::NextImage()
{
	//�� ������ ����
	m_textureCoor[0].x += m_fOneFrame;
	m_textureCoor[1].x += m_fOneFrame;
	m_textureCoor[2].x += m_fOneFrame;
	m_textureCoor[3].x += m_fOneFrame;

	//������ �������̸� ó������
	if (m_textureCoor[0].x >= 1.0f)
	{
		m_textureCoor[0].x = 0;
		m_textureCoor[1].x = 0;
		m_textureCoor[2].x = m_fOneFrame;
		m_textureCoor[3].x = m_fOneFrame;
	}

	//������ ����
	m_vecPTVertex[0].t.x = m_textureCoor[0].x;
	m_vecPTVertex[1].t.x = m_textureCoor[1].x;
	m_vecPTVertex[2].t.x = m_textureCoor[2].x;

	m_vecPTVertex[3].t.x = m_textureCoor[0].x;
	m_vecPTVertex[4].t.x = m_textureCoor[2].x;
	m_vecPTVertex[5].t.x = m_textureCoor[3].x;
}