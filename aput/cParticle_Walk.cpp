#include "stdafx.h"
#include "cParticle_Walk.h"


cParticle_Walk::cParticle_Walk(BoundingBox& boundingBox, int numParticles, D3DXVECTOR3 vPos)
{
	m_stBoundingBox = boundingBox;
	m_fSize = 0.25f;
	m_vPos = vPos;

	for (int i = 0; i < numParticles; ++i)
	{
		AddParticle();
	}
}

cParticle_Walk::~cParticle_Walk()
{
}

void cParticle_Walk::Update()
{
	for (auto iter = m_lstParticles.begin(); iter != m_lstParticles.end(); ++iter)
	{
		iter->pos += (iter->velocity * g_pTimeManager->getElapsedTime());

		// �ٿ�� �ڽ��� ��� ���
		if (m_stBoundingBox.IsPointIn(iter->pos) == false)
			ResetParticle(*iter);
	}
}

void cParticle_Walk::ResetParticle(ParticleAttribute& attribute)
{
	attribute.isAlive = true;

	// �������� ���� x, z ��ġ�� ���� �ȿ��� �����Ѵ�.
	attribute.pos.x = m_vPos.x;
	attribute.pos.z = m_vPos.z;

	// y���� ������ ���� ���� ������ �����Ѵ�.
	attribute.pos.y = m_stBoundingBox.min.y + 0.05f;

	// �������� ����
	attribute.velocity.x = ((rand() % 4 + 1) - 2);
	attribute.velocity.y = 0.1f;
	attribute.velocity.z = ((rand() % 4 + 1) - 2);

	// ������� ����
	attribute.color = D3DCOLOR_XRGB(255, 255, 255);
}
