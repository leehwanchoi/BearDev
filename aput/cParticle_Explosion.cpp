#include "stdafx.h"
#include "cParticle_Explosion.h"


cParticle_Explosion::cParticle_Explosion(D3DXVECTOR3& origin, int numParticles)
{
	m_vOrigin = origin;
	m_fSize = 1.0f;

	for (int i = 0; i < numParticles; ++i)
	{
		AddParticle();
	}
}

cParticle_Explosion::~cParticle_Explosion()
{
}

void cParticle_Explosion::Update()
{
	m_nAlpha += m_nDelta;
	if (m_nAlpha > 255)
	{
		m_nAlpha = 255;
		m_nDelta *= -1;
	}
	if (m_nAlpha < 0)
	{
		m_nAlpha = 0;
		m_nDelta *= -1;
	}

	for (auto iter = m_lstParticles.begin(); iter != m_lstParticles.end(); ++iter)
	{
		// 살아있는 것만 업데이트
		if (iter->isAlive)
		{
			iter->color = D3DCOLOR_ARGB(m_nAlpha, 180, 70, 20);

			iter->age += g_pTimeManager->getElapsedTime();

			if (iter->age > iter->lifeTime)	//수명이 끝났음
			{
				iter->isAlive = false;
			}
		}
	}
}


void cParticle_Explosion::Update(bool& explosion)
{
	Update();

	//파티클이 다 죽으면 리셋
	if (IsDead())
	{
		explosion = false;
		Reset();
	}
}

void cParticle_Explosion::ResetParticle(ParticleAttribute& attribute)
{
	attribute.isAlive = true;
	attribute.pos = m_vOrigin;

	attribute.age = 0.0f;
	attribute.lifeTime = 3.0f;
	m_nAlpha = 0;
	m_nDelta = 4;

	float fRadius = rand() % 100 / 10.0f;

	attribute.pos = D3DXVECTOR3(0, 0, fRadius);

	D3DXMATRIX matRX, matRY, matRZ, matT, matWorld;
	D3DXMatrixRotationX(&matRX, D3DXToRadian(rand() % 3600 / 10.0f));
	D3DXMatrixRotationY(&matRY, D3DXToRadian(rand() % 3600 / 10.0f));
	D3DXMatrixRotationZ(&matRZ, D3DXToRadian(rand() % 3600 / 10.0f));
	D3DXMatrixTranslation(&matT, m_vOrigin.x, m_vOrigin.y, m_vOrigin.z);
	matWorld = matRX * matRY * matRZ * matT;

	D3DXVec3TransformCoord(
		&attribute.pos,
		&attribute.pos,
		&matWorld);

	attribute.color = D3DCOLOR_ARGB(255, 180, 70, 20);
}

void cParticle_Explosion::PreRender()
{
	cParticleSystem::PreRender();

	// 알파블랜딩 방식 결정.
	g_pD3DDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	g_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	g_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);

	// 텍스쳐 알파 옵션 설정
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
}

void cParticle_Explosion::PostRender()
{
	cParticleSystem::PostRender();

	g_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);
}