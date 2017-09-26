#include "stdafx.h"
#include "cParticleSystem.h"


cParticleSystem::cParticleSystem()
{
}


cParticleSystem::~cParticleSystem()
{
}

void cParticleSystem::Setup(char* textureFileName)
{
	g_pD3DDevice->CreateVertexBuffer(
		m_vbSize * sizeof(ST_PC_VERTEX),
		D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
		ST_PC_VERTEX::FVF,
		D3DPOOL_DEFAULT,
		&m_pVertexBuffer,
		NULL);

	g_pTextureManager->AddTexture(textureFileName, textureFileName, &m_pTexture);

	D3DXMatrixIdentity(&m_matWorld);
}

void cParticleSystem::PreRender()
{
	g_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);		// Z����(���� ����)�� ���ؼ� �׸��� ������ �������� �ɼ��� ����. : �������� ǥ���� �� �ӵ� ��� ȿ��

	// ������ ������� �ʴ´�.
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, false);

	//����Ʈ�� �ؽ��ĸ� ���� �� �ְ� ���ش�.
	g_pD3DDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, true);

	//����Ʈ�� Ȯ�� ��� �� �� �ְ� ���ش�.
	g_pD3DDevice->SetRenderState(D3DRS_POINTSCALEENABLE, true);

	//����Ʈ ������ ����
	g_pD3DDevice->SetRenderState(D3DRS_POINTSIZE, FloatToDWORD(m_fSize));

	//����Ʈ �ּ� ũ��
	g_pD3DDevice->SetRenderState(D3DRS_POINTSIZE_MIN, FloatToDWORD(0.0f));

	//����Ʈ �ִ� ũ��
	g_pD3DDevice->SetRenderState(D3DRS_POINTSIZE_MAX, FloatToDWORD(75.0f));

	//����Ʈ �����ϸ� ���Ͱ� ����
	g_pD3DDevice->SetRenderState(D3DRS_POINTSCALE_A, FloatToDWORD(1.0f));
	g_pD3DDevice->SetRenderState(D3DRS_POINTSCALE_B, FloatToDWORD(1.0f));
	g_pD3DDevice->SetRenderState(D3DRS_POINTSCALE_C, FloatToDWORD(1.0f));

	//�ؽ��� ���� ���
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	//���� �� ��� ����
	g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	g_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

void cParticleSystem::Render()
{
	//��ü ��ƼŬ�� �ѹ��� �׸��°� �ƴ϶� ���� ������ ������ �׸���.

	if (!m_lstParticles.empty())
	{
		PreRender();

		g_pD3DDevice->SetTransform(D3DTS_WORLD, &m_matWorld);
		g_pD3DDevice->SetTexture(0, m_pTexture);
		g_pD3DDevice->SetFVF(ST_PC_VERTEX::FVF);
		g_pD3DDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(ST_PC_VERTEX));

		//��ġ������ ������ �ϳ��� �׸���.
		//�������� ũ�Ⱑ ���ؽ������� ���� �����ϸ� �ٽ� ó������ �����Ѵ�.
		if (m_vbOffset >= m_vbSize)
			m_vbOffset = 0;

		ST_PC_VERTEX* stPCVertex;		// PCVertex ����ü �迭

		//�������� 0�̸� �������(D3DLOCK_DISCARD) ó������ �ٽ� ����.
		//�ƴϸ� �������� �״�εΰ� �ڿ� �����̱� �Ѵ�.(D3DLOCK_NOOVERWRITE)
		m_pVertexBuffer->Lock(m_vbOffset * sizeof(ST_PC_VERTEX), m_vbBatchSize * sizeof(ST_PC_VERTEX),
			(void**)&stPCVertex, m_vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

		DWORD numParticleInBatch = 0;

		for (auto iter = m_lstParticles.begin(); iter != m_lstParticles.end(); ++iter)
		{
			if (iter->isAlive)
			{
				//���ؽ� ���۷� ����
				stPCVertex->p = iter->pos;
				stPCVertex->c = (D3DCOLOR)iter->color;
				stPCVertex++;	//���ؽ� ������ ���� ��ġ�� �̵�

				numParticleInBatch++; // ���� ���� ����

				if (numParticleInBatch == m_vbBatchSize)	// �� �ܰ�(batch, ���)�� �׸� ���� �ٵ�� ���ٸ�
				{
					m_pVertexBuffer->Unlock();

					// �� �ܰ迡 ���ΰ��� �׸���.
					g_pD3DDevice->DrawPrimitive(D3DPT_POINTLIST, m_vbOffset, m_vbBatchSize);

					// �׸��� ���� ���� �ܰ踦 ä���
					m_vbOffset += m_vbBatchSize;		// ���� �ܰ�� �̵�
					if (m_vbOffset >= m_vbSize)			// �������� ũ�Ⱑ ���ؽ� ���� ���� �����ϸ�
						m_vbOffset = 0;					// ó������ �ٽ� ����

					m_pVertexBuffer->Lock(m_vbOffset * sizeof(ST_PC_VERTEX), m_vbBatchSize * sizeof(ST_PC_VERTEX),
						(void**)&stPCVertex, m_vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

					numParticleInBatch = 0;	// �ٽ� ó������ ī����
				}
			}
		}
		m_pVertexBuffer->Unlock(); // ������ ��� ����

		// ��ġ ������ ��ŭ�� �׸��� �������� ���� ¥������ ���� ��� �׸���.
		if (numParticleInBatch)
		{
			g_pD3DDevice->DrawPrimitive(D3DPT_POINTLIST, m_vbOffset, numParticleInBatch);
		}

		// ���� �������� ���� ������ ����
		m_vbOffset += m_vbBatchSize;

		PostRender();
	}
}

void cParticleSystem::PostRender()
{
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, true);
	g_pD3DDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, false);
	g_pD3DDevice->SetRenderState(D3DRS_POINTSCALEENABLE, false);
	g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	g_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);		// Z����(���� ����)�� ���ؼ� �׸��� ������ �������� �ɼ��� ����. : �������� ǥ���� �� �ӵ� ��� ȿ��


	g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	g_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

void cParticleSystem::Destroy()
{
	m_pTexture = NULL;
	SAFE_RELEASE(m_pVertexBuffer);
	m_lstParticles.clear();
}

void cParticleSystem::AddParticle()
{
	ParticleAttribute attribute;
	ResetParticle(attribute);
	m_lstParticles.push_back(attribute);
}

void cParticleSystem::Reset()
{
	for (auto iter = m_lstParticles.begin(); iter != m_lstParticles.end(); ++iter)
	{
		ResetParticle(*iter);	// ��ƼŬ ������ �°� ���� �ʱ�ȭ
	}
}

bool cParticleSystem::IsEmpty()
{
	return m_lstParticles.empty();
}

bool cParticleSystem::IsDead()
{
	for (auto iter = m_lstParticles.begin(); iter != m_lstParticles.end(); ++iter)
	{
		// ��ƼŬ�� �Ѱ��� ��� �ִٸ�
		if (iter->isAlive) return false;
	}
	// �Ѱ��� ��� ���� �ʴٸ�
	return true;
}
