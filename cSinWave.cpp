#include "stdafx.h"
#include "cSinWave.h"


cSinWave::cSinWave()
	: m_fWaveHeight(0.25f)
{
	D3DXMatrixIdentity(&m_matW);
}


cSinWave::~cSinWave()
{
}

void cSinWave::Init()
{
	g_pTextureManager->AddTexture("wave", "Textures/wave.png", NULL);

	m_vecPNTVertex.resize(VERTEX_DIM * VERTEX_DIM * sizeof(ST_PNT_VERTEX));

	for (int z = 0; z < VERTEX_DIM; ++z)
	{
		for (int x = 0; x < VERTEX_DIM; ++x)
		{
			// 1중배열 방식의 인덱스 번호 셋팅
			int nIndex = z * VERTEX_DIM + x;
			float y = 0.0f;

			D3DXVECTOR3 n;
			ComputeNormal(&n, &m_vecPNTVertex[nIndex + 0].p, &m_vecPNTVertex[nIndex + 1].p, &m_vecPNTVertex[nIndex + 2].p);

			m_vecPNTVertex[nIndex].p = D3DXVECTOR3(x, y, z);
			m_vecPNTVertex[nIndex].n = n;
			m_vecPNTVertex[nIndex].t = D3DXVECTOR2(x, z);
		}
	}

	for (int z = 0; z < TILE_N; ++z)
	{
		for (int x = 0; x < TILE_N; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * VERTEX_DIM + x + 0;
			int _1 = (z + 1) * VERTEX_DIM + x + 0;
			int _2 = (z + 0) * VERTEX_DIM + x + 1;
			int _3 = (z + 1) * VERTEX_DIM + x + 1;

			m_vecIndex.push_back(_0);
			m_vecIndex.push_back(_1);
			m_vecIndex.push_back(_2);
			m_vecIndex.push_back(_2);
			m_vecIndex.push_back(_1);
			m_vecIndex.push_back(_3);
		}
	}
}

void cSinWave::Update()
{
	for (int z = 0; z < VERTEX_DIM; ++z)
	{
		for (int x = 0; x < VERTEX_DIM; ++x)
		{
			int nIndex = z * VERTEX_DIM + x;
			float fTime = (float)(GetTickCount() % 2000);

			if (x % 2 == 0)
				m_vecPNTVertex[nIndex].p.y = m_fWaveHeight * sinf(2.0f * D3DX_PI * (fTime / 2000 -x / 5.0f));
			else
				m_vecPNTVertex[nIndex].p.y = m_fWaveHeight * -sinf(2.0f * D3DX_PI * (fTime / 2000 -x / 5.0f));
		}
	}
}

void cSinWave::Render()
{
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, false);

	g_pD3DDevice->SetTransform(D3DTS_WORLD, &m_matW);
	g_pD3DDevice->SetFVF(ST_PNT_VERTEX::FVF);
	g_pD3DDevice->SetMaterial(&WHITE_MTRL);
	g_pD3DDevice->SetTexture(0, g_pTextureManager->GetTexture("wave"));
	g_pD3DDevice->DrawIndexedPrimitiveUP(
		D3DPT_TRIANGLELIST,				// 도형 그리기 타입
		0,								// 맨앞(시작) 버텍스의 인덱스
		m_vecPNTVertex.size(),				// 사용되는 버텍스의 수
		m_vecIndex.size() / 3,			// 도형의 수
		&m_vecIndex[0],					// 인덱스 정보를 저장한 배열
		D3DFMT_INDEX32,					// 인덱스 포맷 ( 16비트인지 32비트인지 )
		&m_vecPNTVertex[0],				// 버텍스 정보를 저장한 배열
		sizeof(ST_PNT_VERTEX));			// 버텍스의 사이즈

	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, true);
}
