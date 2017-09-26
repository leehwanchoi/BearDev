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
	g_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);		// Z버퍼(깊이 버퍼)에 의해서 그리는 순서가 정해지는 옵션을 끈다. : 반투명을 표현할 때 속도 향상 효과

	// 조명은 사용하지 않는다.
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, false);

	//포인트에 텍스쳐를 입힐 수 있게 해준다.
	g_pD3DDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, true);

	//포인트를 확대 축소 할 수 있게 해준다.
	g_pD3DDevice->SetRenderState(D3DRS_POINTSCALEENABLE, true);

	//포인트 사이즈 설정
	g_pD3DDevice->SetRenderState(D3DRS_POINTSIZE, FloatToDWORD(m_fSize));

	//포인트 최소 크기
	g_pD3DDevice->SetRenderState(D3DRS_POINTSIZE_MIN, FloatToDWORD(0.0f));

	//포인트 최대 크기
	g_pD3DDevice->SetRenderState(D3DRS_POINTSIZE_MAX, FloatToDWORD(75.0f));

	//포인트 스케일링 팩터값 설정
	g_pD3DDevice->SetRenderState(D3DRS_POINTSCALE_A, FloatToDWORD(1.0f));
	g_pD3DDevice->SetRenderState(D3DRS_POINTSCALE_B, FloatToDWORD(1.0f));
	g_pD3DDevice->SetRenderState(D3DRS_POINTSCALE_C, FloatToDWORD(1.0f));

	//텍스쳐 알파 사용
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	//알파 값 사용 설정
	g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	g_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

void cParticleSystem::Render()
{
	//전체 파티클을 한번에 그리는게 아니라 섹션 단위로 나눠서 그린다.

	if (!m_lstParticles.empty())
	{
		PreRender();

		g_pD3DDevice->SetTransform(D3DTS_WORLD, &m_matWorld);
		g_pD3DDevice->SetTexture(0, m_pTexture);
		g_pD3DDevice->SetFVF(ST_PC_VERTEX::FVF);
		g_pD3DDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(ST_PC_VERTEX));

		//배치사이즈 단위로 하나씩 그린다.
		//오프셋의 크기가 버텍스버퍼의 끝에 도달하면 다시 처음부터 시작한다.
		if (m_vbOffset >= m_vbSize)
			m_vbOffset = 0;

		ST_PC_VERTEX* stPCVertex;		// PCVertex 구조체 배열

		//오프셋이 0이면 다지우고(D3DLOCK_DISCARD) 처음부터 다시 쓴다.
		//아니면 이전것은 그대로두고 뒤에 덧붙이기 한다.(D3DLOCK_NOOVERWRITE)
		m_pVertexBuffer->Lock(m_vbOffset * sizeof(ST_PC_VERTEX), m_vbBatchSize * sizeof(ST_PC_VERTEX),
			(void**)&stPCVertex, m_vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

		DWORD numParticleInBatch = 0;

		for (auto iter = m_lstParticles.begin(); iter != m_lstParticles.end(); ++iter)
		{
			if (iter->isAlive)
			{
				//버텍스 버퍼로 복사
				stPCVertex->p = iter->pos;
				stPCVertex->c = (D3DCOLOR)iter->color;
				stPCVertex++;	//버텍스 버퍼의 다음 위치로 이동

				numParticleInBatch++; // 복사 갯수 증가

				if (numParticleInBatch == m_vbBatchSize)	// 한 단계(batch, 덩어리)에 그릴 양이 다들어 갔다면
				{
					m_pVertexBuffer->Unlock();

					// 한 단계에 모인것을 그린다.
					g_pD3DDevice->DrawPrimitive(D3DPT_POINTLIST, m_vbOffset, m_vbBatchSize);

					// 그리는 동안 다음 단계를 채우기
					m_vbOffset += m_vbBatchSize;		// 다음 단계로 이동
					if (m_vbOffset >= m_vbSize)			// 오프셋의 크기가 버텍스 버퍼 끝에 도달하면
						m_vbOffset = 0;					// 처음부터 다시 시작

					m_pVertexBuffer->Lock(m_vbOffset * sizeof(ST_PC_VERTEX), m_vbBatchSize * sizeof(ST_PC_VERTEX),
						(void**)&stPCVertex, m_vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

					numParticleInBatch = 0;	// 다시 처음부터 카운팅
				}
			}
		}
		m_pVertexBuffer->Unlock(); // 마지막 잠금 해제

		// 배치 사이즈 만큼씩 그리고 마지막에 남은 짜투리가 있을 경우 그린다.
		if (numParticleInBatch)
		{
			g_pD3DDevice->DrawPrimitive(D3DPT_POINTLIST, m_vbOffset, numParticleInBatch);
		}

		// 다음 프레임을 위해 오프셋 증가
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
	g_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);		// Z버퍼(깊이 버퍼)에 의해서 그리는 순서가 정해지는 옵션을 끈다. : 반투명을 표현할 때 속도 향상 효과


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
		ResetParticle(*iter);	// 파티클 종류에 맞게 전부 초기화
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
		// 파티클이 한개라도 살아 있다면
		if (iter->isAlive) return false;
	}
	// 한개도 살아 있지 않다면
	return true;
}
