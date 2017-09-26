#include "stdafx.h"
#include "cHeightMap.h"
#include "cMtlTex.h"


cHeightMap::cHeightMap()
	: m_pMesh(NULL)
	, m_pMtlTex(NULL)
	, m_fSizeX(1.0f)
	, m_fSizeZ(1.0f)
{
	D3DXMatrixIdentity(&m_matWorld);
}


cHeightMap::~cHeightMap()
{
	SAFE_RELEASE(m_pMesh);
}

void cHeightMap::Load(char* szFilePath, D3DXMATRIXA16* pMat)
{
	vector<ST_PNT_VERTEX> vecPNTVertex;
	vecPNTVertex.resize(VERTEX_DIM * VERTEX_DIM);

	m_vecVertex.resize(VERTEX_DIM * VERTEX_DIM);

	FILE* fp = NULL;
	fopen_s(&fp, szFilePath, "rb");

	for (int z = 0; z < VERTEX_DIM; ++z)
	{
		for (int x = 0; x < VERTEX_DIM; ++x)
		{
			// 1중배열 방식의 인덱스 번호 셋팅
			int nIndex = z * VERTEX_DIM + x;
			float y = (unsigned char)fgetc(fp) / 10.0f;

			// z, x 정보를 사용하여 인덱스 번호를 구성 하였기 때문에
			// 정점 정보는 x, z 를 그대로 담고 y 값은 raw 파일에서 읽은 값을 담는다.
			vecPNTVertex[nIndex].p = D3DXVECTOR3(x, y, z);
			vecPNTVertex[nIndex].n = D3DXVECTOR3(0, 1, 0);
			// 텍스쳐의 좌표는 0 ~ 1 사이 값으로 계산해서 담는다.
			vecPNTVertex[nIndex].t = D3DXVECTOR2(x / (float)TILE_N, z / (float)TILE_N);

			// 맵의 위치 및 크기 변경이 있는 경우 처음 좌표 설정 단계에서 좌표 값을 변경하여 담는다.
			if (pMat)
				D3DXVec3TransformCoord(&vecPNTVertex[nIndex].p, &vecPNTVertex[nIndex].p, pMat);
			// 캐릭터의 위치에 따른 y 값 반환을 위해서 버텍스 정보만 따로 빼서 보관한다.
			m_vecVertex[nIndex] = vecPNTVertex[nIndex].p;
		}
	}

	fclose(fp);

	// 맵이 변경 된 사이즈에 따라서 캐릭터의 위치를 계산해야 하기 때문에
	// 변경 된 사이즈 정보를 저장한다.
	m_fSizeX = pMat->_11;
	m_fSizeZ = pMat->_33;

	// 인덱스 번호를 구성한다.
	//vector<DWORD> vecIndex;
	//vecIndex.reserve(TILE_N * TILE_N * 3 * 2);

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

	// 각 정점의 제대로 된 노말값을 계산한다.
	for (int z = 1; z < VERTEX_DIM - 1; ++z)
	{
		for (int x = 1; x < VERTEX_DIM - 1; ++x)
		{
			// ---u---
			// |\ |\ |
			// | \| \|
			// l--n--r
			// |\ |\ |
			// | \| \|
			// ---d---
			int nIndex = z * VERTEX_DIM + x; // n 위치의 인덱스 번호

			int l = nIndex - 1;
			int r = nIndex + 1;
			int u = nIndex + VERTEX_DIM;
			int d = nIndex - VERTEX_DIM;

			D3DXVECTOR3 du = m_vecVertex[u] - m_vecVertex[d];
			D3DXVECTOR3 rl = m_vecVertex[r] - m_vecVertex[l];
			D3DXVECTOR3 n;
			D3DXVec3Cross(&n, &du, &rl);
			D3DXVec3Normalize(&n, &n);

			vecPNTVertex[nIndex].n = n;
		}
	}

	D3DXCreateMeshFVF(m_vecIndex.size() / 3, vecPNTVertex.size(),
		D3DXMESH_MANAGED | D3DXMESH_32BIT, ST_PNT_VERTEX::FVF, g_pD3DDevice, &m_pMesh);

	ST_PNT_VERTEX* pV = NULL;
	m_pMesh->LockVertexBuffer(0, (LPVOID*)&pV);
	memcpy(pV, &vecPNTVertex[0], vecPNTVertex.size() * sizeof(ST_PNT_VERTEX));
	m_pMesh->UnlockVertexBuffer();

	DWORD* pI = NULL;
	m_pMesh->LockIndexBuffer(0, (LPVOID*)&pI);
	memcpy(pI, &m_vecIndex[0], m_vecIndex.size() * sizeof(DWORD));
	m_pMesh->UnlockIndexBuffer();

	DWORD* pA = NULL;
	m_pMesh->LockAttributeBuffer(0, &pA);
	ZeroMemory(pA, (m_vecIndex.size() / 3) * sizeof(DWORD));
	m_pMesh->UnlockAttributeBuffer();

	vector<DWORD> vecAdjbuf(m_pMesh->GetNumFaces() * 3); // vecIndex.size();
	m_pMesh->GenerateAdjacency(0.0f, &vecAdjbuf[0]);

	m_pMesh->OptimizeInplace(D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE, &vecAdjbuf[0], NULL, NULL, NULL);

	D3DMATERIAL9 stMtl = WHITE_MTRL;
	g_pTextureManager->AddTexture("terrain", "HeightMapData/terrain.jpg", NULL, NULL);

	m_pMtlTex = new cMtlTex;
	m_pMtlTex->AutoRelease();
	m_pMtlTex->SetMtl(stMtl);
	m_pMtlTex->SetTexture(g_pTextureManager->GetTexture("terrain"));
}

bool cHeightMap::GetHeight(IN const float x, OUT float& y, IN const float z)
{
	// 맵을 벗어난 경우 (사이즈가 변경 된 경우 변경 된 사이즈로 좌표를 나눠서 계산한다.)
	if (x < 0 || x / m_fSizeX > TILE_N || z < 0 || z / m_fSizeZ > TILE_N)
		return false;

	// 1--3
	// |\ |
	// | \|
	// 0--2
	int nX = (int)(x / m_fSizeX);
	int nZ = (int)(z / m_fSizeZ);
	float fDeltaX = (x / m_fSizeX) - nX;
	float fDeltaZ = (z / m_fSizeZ) - nZ;

	// 캐릭터 위치 x, z 를 통해서 0 1 2 3 정점의 인덱스 번호를 계산한다.
	int _0 = (nZ + 0) * VERTEX_DIM + nX + 0;
	int _1 = (nZ + 1) * VERTEX_DIM + nX + 0;
	int _2 = (nZ + 0) * VERTEX_DIM + nX + 1;
	int _3 = (nZ + 1) * VERTEX_DIM + nX + 1;

	// 1--3
	// |\ |
	// | \|
	// 0--2
	if (fDeltaX + fDeltaZ < 1.0f) // 아래쪽 삼각형 위에 위치하는 경우
	{
		// 1 - 0
		float zY = m_vecVertex[_1].y - m_vecVertex[_0].y;
		// 2 - 0
		float xY = m_vecVertex[_2].y - m_vecVertex[_0].y;
		// 0번 인덱스의 y값을 기준으로 캐릭터 위치 y값 계산
		y = m_vecVertex[_0].y + zY * fDeltaZ + xY * fDeltaX;
	}
	else // 윗쪽 삼각형
	{
		// 2 - 3
		float zY = m_vecVertex[_2].y - m_vecVertex[_3].y;
		// 1 - 3
		float xY = m_vecVertex[_1].y - m_vecVertex[_3].y;
		// 3번 인덱스의 y값을 기준으로 캐릭터 위치 y값 계산
		// 델타 값을 0번 인덱스 기준으로 계산을 했기 때문에
		// 3번 인덱스 기준으로 계산 할 때는 반전이 필요 하다.
		y = m_vecVertex[_3].y + zY * (1.0f - fDeltaZ) + xY * (1.0f - fDeltaX);
	}

	return true;
}

void cHeightMap::Render()
{
	g_pD3DDevice->SetTransform(D3DTS_WORLD, &m_matWorld);
	g_pD3DDevice->SetMaterial(&m_pMtlTex->GetMtl());
	g_pD3DDevice->SetTexture(0, m_pMtlTex->GetTexture());
	m_pMesh->DrawSubset(0);
}
