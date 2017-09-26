#include "stdafx.h"
#include "cParticle_Snow.h"


cParticle_Snow::cParticle_Snow(BoundingBox& boundingBox, int numParticles)
{
	m_stBoundingBox = boundingBox;
	m_fSize = 0.25f;

	for (int i = 0; i < numParticles; ++i)
	{
		AddParticle();
	}
}

cParticle_Snow::~cParticle_Snow()
{
}

void cParticle_Snow::Update()
{
	for (auto iter = m_lstParticles.begin(); iter != m_lstParticles.end(); ++iter)
	{
		iter->pos += (iter->velocity * g_pTimeManager->getElapsedTime());

		// 바운딩 박스를 벗어난 경우
		if (m_stBoundingBox.IsPointIn(iter->pos) == false)
			ResetParticle(*iter);
	}
}

void cParticle_Snow::ResetParticle(ParticleAttribute& attribute)
{
	attribute.isAlive = true;

	// 랜덤으로 눈의 x, z 위치를 범위 안에서 결정한다.
	attribute.pos.x = (rand() % (int)((m_stBoundingBox.max.x - m_stBoundingBox.min.x) * 10))
		/ 10.0f + m_stBoundingBox.min.x;
	attribute.pos.z = (rand() % (int)((m_stBoundingBox.max.z - m_stBoundingBox.min.z) * 10))
		/ 10.0f + m_stBoundingBox.min.z;

	// y값은 무조건 제일 높은 곳에서 시작한다.
	attribute.pos.y = m_stBoundingBox.max.y;

	// 떨어지는 방향
	attribute.velocity.x = (rand() % 10 + 1) / 10.0f * -3.0f;
	attribute.velocity.y = (rand() % 10 + 1) / 10.0f * -10.0f;
	attribute.velocity.z = 0.0f;

	// 흰색으로 셋팅
	attribute.color = D3DCOLOR_XRGB(255, 255, 255);
}
