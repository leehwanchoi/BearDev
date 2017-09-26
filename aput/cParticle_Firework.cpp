#include "stdafx.h"
#include "cParticle_Firework.h"


cParticle_Firework::cParticle_Firework(D3DXVECTOR3& origin, int numParticles)
{
	m_vOrigin = origin;
	m_fSize = 0.1f;
	

	for (int i = 0; i < numParticles; ++i)
		AddParticle();

	for (auto iter = m_lstParticles.begin(); iter != m_lstParticles.end(); ++iter)
	{
		iter->lifeTime = 3.0f;
	}
}


cParticle_Firework::~cParticle_Firework()
{
}

void cParticle_Firework::Update(bool& fire)
{
	Update();

	if (IsDead())
	{
		fire = false;
		Reset();
	}
}

void cParticle_Firework::Update()
{
	for (auto iter = m_lstParticles.begin(); iter != m_lstParticles.end(); ++iter)
	{
		if (iter->isAlive)
		{
			iter->pos += (iter->velocity * g_pTimeManager->getElapsedTime());
			iter->age += g_pTimeManager->getElapsedTime();

			if (iter->age > iter->lifeTime)
				iter->isAlive = false;
		}
	}
}

void cParticle_Firework::ResetParticle(ParticleAttribute & attribute)
{
	attribute.isAlive = true;
	attribute.pos = m_vOrigin;

	D3DXVECTOR3 min = D3DXVECTOR3(-1.0f,-1.0f,-1.0f);
	D3DXVECTOR3 max = D3DXVECTOR3( 1.0f, 1.0f, 1.0f);

	attribute.velocity.x = rand() % (int)((max.x - min.x) * 10) / 10.0f + min.x;
	attribute.velocity.y = rand() % (int)((max.y - min.y) * 10) / 10.0f + min.y;
	attribute.velocity.z = rand() % (int)((max.z - min.z) * 10) / 10.0f + min.z;

	D3DXVec3Normalize(&attribute.velocity, &attribute.velocity);
	
	attribute.velocity *= 1.0f;

	attribute.color = D3DXCOLOR(rand() % 11 / 10.0f, rand() % 11 / 10.0f, rand() % 11 / 10.0f, 1.0f);

	attribute.age = 0.0f;
	attribute.lifeTime = 20.0f;
}

void cParticle_Firework::PreRender()
{
	cParticleSystem::PreRender();

	g_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	g_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// z-buffer 를 읽기만 하고 쓰기는 금지
	g_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

void cParticle_Firework::PostRender()
{
	cParticleSystem::PostRender();
}