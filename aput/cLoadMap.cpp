#include "stdafx.h"
#include "cLoadMap.h"
#include "cXobjectLoader.h"

cLoadMap::cLoadMap()
	: m_fSizeX(1.0f)
	, m_fSizeZ(1.0f)
	, m_nVertexSize(0)
	, m_pTexture(NULL)
{
	D3DXMatrixIdentity(&m_matWorld); //Terrain(지형)의 매트리스 위치를 초기화 시키기 위한 작업
}


cLoadMap::~cLoadMap()
{
	for (int i = 0; i < m_vecXLoader.size(); i++) // 맵툴로 인해 생성된 모든 X파일 해제
		m_vecXLoader[i]->Destroy();
}

void cLoadMap::Load(char* szFilePath, D3DXMATRIXA16* pMat)
{
	assert(pMat && "Load는 사용할수 없습니다. RealLoad를 사용하세요 !!!");
	assert(pMat || "Load는 사용할수 없습니다. RealLoad를 사용하세요 !!!");
}


bool cLoadMap::GetHeight(IN const float x, OUT float & y, IN const float z)
{
	if (x < 0 || x / m_fSizeX > (m_nVertexSize - 1) || z < 0 || z / m_fSizeZ > (m_nVertexSize - 1))
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
	int _0 = (nZ + 0) * m_nVertexSize + nX + 0;
	int _1 = (nZ + 1) * m_nVertexSize + nX + 0;
	int _2 = (nZ + 0) * m_nVertexSize + nX + 1;
	int _3 = (nZ + 1) * m_nVertexSize + nX + 1;

	// 1--3
	// |\ |
	// | \|
	// 0--2
	if (fDeltaX + fDeltaZ < 1.0f) // 아래쪽 삼각형 위에 위치하는 경우
	{
		// 1 - 0
		float zY = m_vecPNTVertex[_1].p.y - m_vecPNTVertex[_0].p.y;
		// 2 - 0
		float xY = m_vecPNTVertex[_2].p.y - m_vecPNTVertex[_0].p.y;
		// 0번 인덱스의 y값을 기준으로 캐릭터 위치 y값 계산
		y = m_vecPNTVertex[_0].p.y + zY * fDeltaZ + xY * fDeltaX;
	}
	else // 윗쪽 삼각형
	{
		// 2 - 3
		float zY = m_vecPNTVertex[_2].p.y - m_vecPNTVertex[_3].p.y;
		// 1 - 3
		float xY = m_vecPNTVertex[_1].p.y - m_vecPNTVertex[_3].p.y;
		// 3번 인덱스의 y값을 기준으로 캐릭터 위치 y값 계산
		// 델타 값을 0번 인덱스 기준으로 계산을 했기 때문에
		// 3번 인덱스 기준으로 계산 할 때는 반전이 필요 하다.
		y = m_vecPNTVertex[_3].p.y + zY * (1.0f - fDeltaZ) + xY * (1.0f - fDeltaX);
	}

	return true;
}

void cLoadMap::Render()
{
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, false);  //Terrain(지형)은 라이트 미사용
	g_pD3DDevice->SetTransform(D3DTS_WORLD, &m_matWorld);
	g_pD3DDevice->SetTexture(0, m_pTexture);
	g_pD3DDevice->SetMaterial(&WHITE_MTRL);
	g_pD3DDevice->SetFVF(ST_PNT_VERTEX::FVF);
	g_pD3DDevice->DrawIndexedPrimitiveUP(
		D3DPT_TRIANGLELIST,				// 도형 그리기 타입
		0,								// 맨앞(시작) 버텍스의 인덱스
		m_vecPNTVertex.size(),			// 사용되는 버텍스의 수
		m_vecIndex.size() / 3,			// 도형의 수
		&m_vecIndex[0],					// 인덱스 정보를 저장한 배열
		D3DFMT_INDEX32,					// 인덱스 포맷 ( 16비트인지 32비트인지 )
		&m_vecPNTVertex[0],				// 버텍스 정보를 저장한 배열
		sizeof(ST_PNT_VERTEX));			// 버텍스의 사이즈

	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, true);  //맵툴로 생성된 X파일은 라이트 사용
	for (int i = 0; i < m_vecXLoader.size(); i++)
		m_vecXLoader[i]->Render();
}

void cLoadMap::SliceQuarterSetup(int VertexNum)
{
	//초기 빠른 검색 인덱스 설정
	m_fastSearch[0] = 0;
	m_fastSearch[1] = (VertexNum - 1) / 2;
	m_fastSearch[2] = (VertexNum - 1);
	m_fastSearch[3] = (VertexNum - 1) * VertexNum / 2;
	m_fastSearch[4] = ((VertexNum - 1) * VertexNum / 2) + ((VertexNum - 1) / 2);
	m_fastSearch[5] = ((VertexNum - 1) * VertexNum / 2) + (VertexNum - 1);
	m_fastSearch[6] = (VertexNum - 1) * VertexNum;
	m_fastSearch[7] = ((VertexNum * VertexNum) - 1) - ((VertexNum - 1) / 2);
	m_fastSearch[8] = VertexNum * VertexNum - 1;

	/*
	6 7 8
	3 4 5
	0 1 2
	*/

	// 크게 한번 검색하기 위해, 분기별로 나눔.(1~4분기)
	if (m_fastSearch[8] > 0)
	{
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[0]].p);//1~3
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[3]].p);
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[1]].p);
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[1]].p);//4~6 //1분기 (x < m_fastSearch[1]) (z < m_fastSearch[3])
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[3]].p);
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[4]].p);
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[1]].p);//7~9
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[4]].p);
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[2]].p);
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[2]].p);//10~12 //2분기
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[4]].p);
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[5]].p);
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[3]].p);//13~15
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[6]].p);
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[4]].p);
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[4]].p);//16~18 //3분기
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[6]].p);
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[7]].p);
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[4]].p);//19~21
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[7]].p);
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[5]].p);
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[5]].p);//22~24 //4분기
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[7]].p);
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[8]].p);
	}


	//빠른 검색을 위한 작업 (하프) 1분기 세분화
	m_fastSearchHalf01[0] = 0;
	m_fastSearchHalf01[1] = m_fastSearch[1] / 2;
	m_fastSearchHalf01[2] = m_fastSearch[2] / 2;
	m_fastSearchHalf01[3] = m_fastSearch[3] / 2;
	m_fastSearchHalf01[4] = m_fastSearch[4] / 2;
	m_fastSearchHalf01[5] = m_fastSearch[5] / 2;
	m_fastSearchHalf01[6] = m_fastSearch[6] / 2;
	m_fastSearchHalf01[7] = m_fastSearch[7] / 2;
	m_fastSearchHalf01[8] = m_fastSearch[8] / 2;

	// 분기별로 나눈것을 다시 나눔. (빠른 검색을 위해) 1분기 세분화
	if (m_fastSearchHalf01[8] > 0)
	{
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[0]].p);//1~3
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[3]].p);
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[1]].p);
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[1]].p);//4~6 //1분기 (x < m_fastSearchHalf[1]) (z < m_fastSearchHalf[3])
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[3]].p);
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[4]].p);
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[1]].p);//7~9
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[4]].p);
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[2]].p);
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[2]].p);//10~12 //2분기
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[4]].p);
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[5]].p);
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[3]].p);//13~15
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[6]].p);
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[4]].p);
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[4]].p);//16~18 //3분기
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[6]].p);
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[7]].p);
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[4]].p);//19~21
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[7]].p);
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[5]].p);
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[5]].p);//22~24 //4분기
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[7]].p);
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[8]].p);
	}

	//빠른 검색을 위한 작업 (하프) 2분기 세분화
	m_fastSearchHalf02[0] = m_fastSearchHalf01[0] + m_fastSearchHalf01[2];
	m_fastSearchHalf02[1] = m_fastSearchHalf01[1] + m_fastSearchHalf01[2];
	m_fastSearchHalf02[2] = m_fastSearchHalf01[2] + m_fastSearchHalf01[2];
	m_fastSearchHalf02[3] = m_fastSearchHalf01[3] + m_fastSearchHalf01[2];
	m_fastSearchHalf02[4] = m_fastSearchHalf01[4] + m_fastSearchHalf01[2];
	m_fastSearchHalf02[5] = m_fastSearchHalf01[5] + m_fastSearchHalf01[2];
	m_fastSearchHalf02[6] = m_fastSearchHalf01[6] + m_fastSearchHalf01[2];
	m_fastSearchHalf02[7] = m_fastSearchHalf01[7] + m_fastSearchHalf01[2];
	m_fastSearchHalf02[8] = m_fastSearchHalf01[8] + m_fastSearchHalf01[2];

	// 분기별로 나눈것을 다시 나눔. (빠른 검색을 위해) 2분기 세분화
	if (m_fastSearchHalf02[8] > 0)
	{
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[0]].p);//1~3
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[3]].p);
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[1]].p);
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[1]].p);//4~6 //1분기 (x < m_fastSearchHalf[1]) (z < m_fastSearchHalf[3])
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[3]].p);
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[4]].p);
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[1]].p);//7~9
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[4]].p);
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[2]].p);
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[2]].p);//10~12 //2분기
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[4]].p);
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[5]].p);
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[3]].p);//13~15
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[6]].p);
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[4]].p);
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[4]].p);//16~18 //3분기
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[6]].p);
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[7]].p);
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[4]].p);//19~21
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[7]].p);
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[5]].p);
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[5]].p);//22~24 //4분기
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[7]].p);
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[8]].p);
	}

	//빠른 검색을 위한 작업 (하프) 3분기 세분화
	m_fastSearchHalf03[0] = m_fastSearchHalf01[0] + m_fastSearchHalf01[6];
	m_fastSearchHalf03[1] = m_fastSearchHalf01[1] + m_fastSearchHalf01[6];
	m_fastSearchHalf03[2] = m_fastSearchHalf01[2] + m_fastSearchHalf01[6];
	m_fastSearchHalf03[3] = m_fastSearchHalf01[3] + m_fastSearchHalf01[6];
	m_fastSearchHalf03[4] = m_fastSearchHalf01[4] + m_fastSearchHalf01[6];
	m_fastSearchHalf03[5] = m_fastSearchHalf01[5] + m_fastSearchHalf01[6];
	m_fastSearchHalf03[6] = m_fastSearchHalf01[6] + m_fastSearchHalf01[6];
	m_fastSearchHalf03[7] = m_fastSearchHalf01[7] + m_fastSearchHalf01[6];
	m_fastSearchHalf03[8] = m_fastSearchHalf01[8] + m_fastSearchHalf01[6];

	// 분기별로 나눈것을 다시 나눔. (빠른 검색을 위해) 3분기 세분화
	if (m_fastSearchHalf03[8] > 0)
	{
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[0]].p);//1~3
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[3]].p);
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[1]].p);
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[1]].p);//4~6 //1분기 (x < m_fastSearchHalf[1]) (z < m_fastSearchHalf[3])
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[3]].p);
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[4]].p);
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[1]].p);//7~9
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[4]].p);
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[2]].p);
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[2]].p);//10~12 //2분기
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[4]].p);
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[5]].p);
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[3]].p);//13~15
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[6]].p);
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[4]].p);
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[4]].p);//16~18 //3분기
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[6]].p);
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[7]].p);
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[4]].p);//19~21
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[7]].p);
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[5]].p);
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[5]].p);//22~24 //4분기
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[7]].p);
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[8]].p);
	}

	//빠른 검색을 위한 작업 (하프) 4분기 세분화
	m_fastSearchHalf04[0] = m_fastSearchHalf03[0] + m_fastSearchHalf01[2];
	m_fastSearchHalf04[1] = m_fastSearchHalf03[1] + m_fastSearchHalf01[2];
	m_fastSearchHalf04[2] = m_fastSearchHalf03[2] + m_fastSearchHalf01[2];
	m_fastSearchHalf04[3] = m_fastSearchHalf03[3] + m_fastSearchHalf01[2];
	m_fastSearchHalf04[4] = m_fastSearchHalf03[4] + m_fastSearchHalf01[2];
	m_fastSearchHalf04[5] = m_fastSearchHalf03[5] + m_fastSearchHalf01[2];
	m_fastSearchHalf04[6] = m_fastSearchHalf03[6] + m_fastSearchHalf01[2];
	m_fastSearchHalf04[7] = m_fastSearchHalf03[7] + m_fastSearchHalf01[2];
	m_fastSearchHalf04[8] = m_fastSearchHalf03[8] + m_fastSearchHalf01[2];

	// 분기별로 나눈것을 다시 나눔. (빠른 검색을 위해) 4분기 세분화
	if (m_fastSearchHalf04[8] > 0)
	{
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[0]].p);//1~3
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[3]].p);
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[1]].p);
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[1]].p);//4~6 //1분기 (x < m_fastSearchHalf[1]) (z < m_fastSearchHalf[3])
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[3]].p);
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[4]].p);
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[1]].p);//7~9
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[4]].p);
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[2]].p);
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[2]].p);//10~12 //2분기
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[4]].p);
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[5]].p);
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[3]].p);//13~15
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[6]].p);
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[4]].p);
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[4]].p);//16~18 //3분기
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[6]].p);
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[7]].p);
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[4]].p);//19~21
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[7]].p);
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[5]].p);
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[5]].p);//22~24 //4분기
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[7]].p);
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[8]].p);
	}


	// 빠른 검색을 위해 등분별 인덱스 저장
	// 1등분
	for (int z = m_fastSearchHalf01[0]; z < m_fastSearchHalf01[1]; ++z)
	{
		for (int x = m_fastSearchHalf01[0]; x < m_fastSearchHalf01[1]; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * VertexNum + x + 0;
			int _1 = (z + 1) * VertexNum + x + 0;
			int _2 = (z + 0) * VertexNum + x + 1;
			int _3 = (z + 1) * VertexNum + x + 1;

			m_vecfast01Index.push_back(_0);
			m_vecfast01Index.push_back(_1);
			m_vecfast01Index.push_back(_2);
			m_vecfast01Index.push_back(_2);
			m_vecfast01Index.push_back(_1);
			m_vecfast01Index.push_back(_3);
		}
	}

	// 2등분
	for (int z = m_fastSearchHalf01[0]; z < m_fastSearchHalf01[1]; ++z)
	{
		for (int x = m_fastSearchHalf01[1]; x < m_fastSearchHalf01[2]; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * VertexNum + x + 0;
			int _1 = (z + 1) * VertexNum + x + 0;
			int _2 = (z + 0) * VertexNum + x + 1;
			int _3 = (z + 1) * VertexNum + x + 1;

			m_vecfast02Index.push_back(_0);
			m_vecfast02Index.push_back(_1);
			m_vecfast02Index.push_back(_2);
			m_vecfast02Index.push_back(_2);
			m_vecfast02Index.push_back(_1);
			m_vecfast02Index.push_back(_3);
		}
	}

	// 3등분
	for (int z = m_fastSearchHalf01[1]; z < m_fastSearchHalf01[2]; ++z)
	{
		for (int x = m_fastSearchHalf01[0]; x < m_fastSearchHalf01[1]; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * VertexNum + x + 0;
			int _1 = (z + 1) * VertexNum + x + 0;
			int _2 = (z + 0) * VertexNum + x + 1;
			int _3 = (z + 1) * VertexNum + x + 1;

			m_vecfast03Index.push_back(_0);
			m_vecfast03Index.push_back(_1);
			m_vecfast03Index.push_back(_2);
			m_vecfast03Index.push_back(_2);
			m_vecfast03Index.push_back(_1);
			m_vecfast03Index.push_back(_3);
		}
	}

	// 4등분
	for (int z = m_fastSearchHalf01[1]; z < m_fastSearchHalf01[2]; ++z)
	{
		for (int x = m_fastSearchHalf01[1]; x < m_fastSearchHalf01[2]; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * VertexNum + x + 0;
			int _1 = (z + 1) * VertexNum + x + 0;
			int _2 = (z + 0) * VertexNum + x + 1;
			int _3 = (z + 1) * VertexNum + x + 1;

			m_vecfast04Index.push_back(_0);
			m_vecfast04Index.push_back(_1);
			m_vecfast04Index.push_back(_2);
			m_vecfast04Index.push_back(_2);
			m_vecfast04Index.push_back(_1);
			m_vecfast04Index.push_back(_3);
		}
	}

	// 5등분
	for (int z = m_fastSearchHalf01[0]; z < m_fastSearchHalf01[1]; ++z)
	{
		for (int x = m_fastSearchHalf02[0]; x < m_fastSearchHalf02[1]; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * VertexNum + x + 0;
			int _1 = (z + 1) * VertexNum + x + 0;
			int _2 = (z + 0) * VertexNum + x + 1;
			int _3 = (z + 1) * VertexNum + x + 1;

			m_vecfast05Index.push_back(_0);
			m_vecfast05Index.push_back(_1);
			m_vecfast05Index.push_back(_2);
			m_vecfast05Index.push_back(_2);
			m_vecfast05Index.push_back(_1);
			m_vecfast05Index.push_back(_3);
		}
	}

	// 6등분
	for (int z = m_fastSearchHalf01[0]; z < m_fastSearchHalf01[1]; ++z)
	{
		for (int x = m_fastSearchHalf02[1]; x < m_fastSearchHalf02[2]; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * VertexNum + x + 0;
			int _1 = (z + 1) * VertexNum + x + 0;
			int _2 = (z + 0) * VertexNum + x + 1;
			int _3 = (z + 1) * VertexNum + x + 1;

			m_vecfast06Index.push_back(_0);
			m_vecfast06Index.push_back(_1);
			m_vecfast06Index.push_back(_2);
			m_vecfast06Index.push_back(_2);
			m_vecfast06Index.push_back(_1);
			m_vecfast06Index.push_back(_3);
		}
	}

	// 7등분
	for (int z = m_fastSearchHalf01[1]; z < m_fastSearchHalf01[2]; ++z)
	{
		for (int x = m_fastSearchHalf02[0]; x < m_fastSearchHalf02[1]; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * VertexNum + x + 0;
			int _1 = (z + 1) * VertexNum + x + 0;
			int _2 = (z + 0) * VertexNum + x + 1;
			int _3 = (z + 1) * VertexNum + x + 1;

			m_vecfast07Index.push_back(_0);
			m_vecfast07Index.push_back(_1);
			m_vecfast07Index.push_back(_2);
			m_vecfast07Index.push_back(_2);
			m_vecfast07Index.push_back(_1);
			m_vecfast07Index.push_back(_3);
		}
	}

	// 8등분
	for (int z = m_fastSearchHalf01[1]; z < m_fastSearchHalf01[2]; ++z)
	{
		for (int x = m_fastSearchHalf02[1]; x < m_fastSearchHalf02[2]; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * VertexNum + x + 0;
			int _1 = (z + 1) * VertexNum + x + 0;
			int _2 = (z + 0) * VertexNum + x + 1;
			int _3 = (z + 1) * VertexNum + x + 1;

			m_vecfast08Index.push_back(_0);
			m_vecfast08Index.push_back(_1);
			m_vecfast08Index.push_back(_2);
			m_vecfast08Index.push_back(_2);
			m_vecfast08Index.push_back(_1);
			m_vecfast08Index.push_back(_3);
		}
	}

	// 9등분
	for (int z = m_fastSearchHalf02[0]; z < m_fastSearchHalf02[1]; ++z)
	{
		for (int x = m_fastSearchHalf01[0]; x < m_fastSearchHalf01[1]; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * VertexNum + x + 0;
			int _1 = (z + 1) * VertexNum + x + 0;
			int _2 = (z + 0) * VertexNum + x + 1;
			int _3 = (z + 1) * VertexNum + x + 1;

			m_vecfast09Index.push_back(_0);
			m_vecfast09Index.push_back(_1);
			m_vecfast09Index.push_back(_2);
			m_vecfast09Index.push_back(_2);
			m_vecfast09Index.push_back(_1);
			m_vecfast09Index.push_back(_3);
		}
	}

	// 10등분
	for (int z = m_fastSearchHalf02[0]; z < m_fastSearchHalf02[1]; ++z)
	{
		for (int x = m_fastSearchHalf01[1]; x < m_fastSearchHalf01[2]; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * VertexNum + x + 0;
			int _1 = (z + 1) * VertexNum + x + 0;
			int _2 = (z + 0) * VertexNum + x + 1;
			int _3 = (z + 1) * VertexNum + x + 1;

			m_vecfast10Index.push_back(_0);
			m_vecfast10Index.push_back(_1);
			m_vecfast10Index.push_back(_2);
			m_vecfast10Index.push_back(_2);
			m_vecfast10Index.push_back(_1);
			m_vecfast10Index.push_back(_3);
		}
	}

	// 11등분
	for (int z = m_fastSearchHalf02[1]; z < m_fastSearchHalf02[2]; ++z)
	{
		for (int x = m_fastSearchHalf01[0]; x < m_fastSearchHalf01[1]; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * VertexNum + x + 0;
			int _1 = (z + 1) * VertexNum + x + 0;
			int _2 = (z + 0) * VertexNum + x + 1;
			int _3 = (z + 1) * VertexNum + x + 1;

			m_vecfast11Index.push_back(_0);
			m_vecfast11Index.push_back(_1);
			m_vecfast11Index.push_back(_2);
			m_vecfast11Index.push_back(_2);
			m_vecfast11Index.push_back(_1);
			m_vecfast11Index.push_back(_3);
		}
	}

	// 12등분
	for (int z = m_fastSearchHalf02[1]; z < m_fastSearchHalf02[2]; ++z)
	{
		for (int x = m_fastSearchHalf01[1]; x < m_fastSearchHalf01[2]; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * VertexNum + x + 0;
			int _1 = (z + 1) * VertexNum + x + 0;
			int _2 = (z + 0) * VertexNum + x + 1;
			int _3 = (z + 1) * VertexNum + x + 1;

			m_vecfast12Index.push_back(_0);
			m_vecfast12Index.push_back(_1);
			m_vecfast12Index.push_back(_2);
			m_vecfast12Index.push_back(_2);
			m_vecfast12Index.push_back(_1);
			m_vecfast12Index.push_back(_3);
		}
	}

	// 13등분
	for (int z = m_fastSearchHalf02[0]; z < m_fastSearchHalf02[1]; ++z)
	{
		for (int x = m_fastSearchHalf02[0]; x < m_fastSearchHalf02[1]; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * VertexNum + x + 0;
			int _1 = (z + 1) * VertexNum + x + 0;
			int _2 = (z + 0) * VertexNum + x + 1;
			int _3 = (z + 1) * VertexNum + x + 1;

			m_vecfast13Index.push_back(_0);
			m_vecfast13Index.push_back(_1);
			m_vecfast13Index.push_back(_2);
			m_vecfast13Index.push_back(_2);
			m_vecfast13Index.push_back(_1);
			m_vecfast13Index.push_back(_3);
		}
	}

	// 14등분
	for (int z = m_fastSearchHalf02[0]; z < m_fastSearchHalf02[1]; ++z)
	{
		for (int x = m_fastSearchHalf02[1]; x < m_fastSearchHalf02[2]; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * VertexNum + x + 0;
			int _1 = (z + 1) * VertexNum + x + 0;
			int _2 = (z + 0) * VertexNum + x + 1;
			int _3 = (z + 1) * VertexNum + x + 1;

			m_vecfast14Index.push_back(_0);
			m_vecfast14Index.push_back(_1);
			m_vecfast14Index.push_back(_2);
			m_vecfast14Index.push_back(_2);
			m_vecfast14Index.push_back(_1);
			m_vecfast14Index.push_back(_3);
		}
	}

	// 15등분
	for (int z = m_fastSearchHalf02[1]; z < m_fastSearchHalf02[2]; ++z)
	{
		for (int x = m_fastSearchHalf02[0]; x < m_fastSearchHalf02[1]; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * VertexNum + x + 0;
			int _1 = (z + 1) * VertexNum + x + 0;
			int _2 = (z + 0) * VertexNum + x + 1;
			int _3 = (z + 1) * VertexNum + x + 1;

			m_vecfast15Index.push_back(_0);
			m_vecfast15Index.push_back(_1);
			m_vecfast15Index.push_back(_2);
			m_vecfast15Index.push_back(_2);
			m_vecfast15Index.push_back(_1);
			m_vecfast15Index.push_back(_3);
		}
	}

	// 16등분
	for (int z = m_fastSearchHalf02[1]; z < m_fastSearchHalf02[2]; ++z)
	{
		for (int x = m_fastSearchHalf02[1]; x < m_fastSearchHalf02[2]; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * VertexNum + x + 0;
			int _1 = (z + 1) * VertexNum + x + 0;
			int _2 = (z + 0) * VertexNum + x + 1;
			int _3 = (z + 1) * VertexNum + x + 1;

			m_vecfast16Index.push_back(_0);
			m_vecfast16Index.push_back(_1);
			m_vecfast16Index.push_back(_2);
			m_vecfast16Index.push_back(_2);
			m_vecfast16Index.push_back(_1);
			m_vecfast16Index.push_back(_3);
		}
	}
}

void cLoadMap::SliceQuarterCalculation(D3DXVECTOR3 & vDest)
{
	D3DXVECTOR3 vRayOrig(vDest.x, vDest.y + 10, vDest.z);
	D3DXVECTOR3 vRayDir(0, -1, 0);

	float minF = 100000;

	bool isBig = false;
	bool isSmall = false;

	int searchBig = -100000; //오류시 터지기 위해 -큰값 (분기 확인용)
	int searchSmall = -100000; //오류시 터지기 위해 -큰값 (세분화 분기 확인용)

							   //심플로 검색 (4분기로 검색)
	for (int i = 0; i < m_fastVertex.size(); i += 3)
	{
		float f;
		if (D3DXIntersectTri(&m_fastVertex[i + 0],
			&m_fastVertex[i + 1], &m_fastVertex[i + 2],
			&vRayOrig, &vRayDir, NULL, NULL, &f))
		{
			if (f < minF)
			{
				minF = f;
				isBig = true;
				searchBig = i + 0;

				break;
			}
		}
	}

	//초기화
	minF = 100000;
	//심플로 검색 (4분기에서 4분기로 검색) //16등분
	if (isBig && !isSmall)
	{
		if (searchBig < 6) // (4/1)분기
		{
			for (int i = 0; i < m_fastVertexHalf01.size(); i += 3)
			{
				float f;
				if (D3DXIntersectTri(&m_fastVertexHalf01[i + 0],
					&m_fastVertexHalf01[i + 1], &m_fastVertexHalf01[i + 2],
					&vRayOrig, &vRayDir, NULL, NULL, &f))
				{
					if (f < minF)
					{
						minF = f;
						isSmall = true;
						searchSmall = i + 0;

						break;
					}
				}
			}
		}
		else if (searchBig < 12) // (4/2)분기
		{
			for (int i = 0; i < m_fastVertexHalf02.size(); i += 3)
			{
				float f;
				if (D3DXIntersectTri(&m_fastVertexHalf02[i + 0],
					&m_fastVertexHalf02[i + 1], &m_fastVertexHalf02[i + 2],
					&vRayOrig, &vRayDir, NULL, NULL, &f))
				{
					if (f < minF)
					{
						minF = f;
						isSmall = true;
						searchSmall = i + 0;

						break;
					}
				}
			}
		}
		else if (searchBig < 18) // (4/3)분기
		{
			for (int i = 0; i < m_fastVertexHalf03.size(); i += 3)
			{
				float f;
				if (D3DXIntersectTri(&m_fastVertexHalf03[i + 0],
					&m_fastVertexHalf03[i + 1], &m_fastVertexHalf03[i + 2],
					&vRayOrig, &vRayDir, NULL, NULL, &f))
				{
					if (f < minF)
					{
						minF = f;
						isSmall = true;
						searchSmall = i + 0;

						break;
					}
				}
			}
		}
		else if (searchBig < 24) // (4/4)분기
		{
			for (int i = 0; i < m_fastVertexHalf04.size(); i += 3)
			{
				float f;
				if (D3DXIntersectTri(&m_fastVertexHalf04[i + 0],
					&m_fastVertexHalf04[i + 1], &m_fastVertexHalf04[i + 2],
					&vRayOrig, &vRayDir, NULL, NULL, &f))
				{
					if (f < minF)
					{
						minF = f;
						isSmall = true;
						searchSmall = i + 0;

						break;
					}
				}
			}
		}
	}

	//초기화
	minF = 100000;
	//16등분 중에서 검색
	if (isSmall)
	{
		if (searchBig < 6) // (4/1) -> (16/1)등분 ~ (16/4)등분
		{
			if (searchSmall < 6) // (16/1)등분
			{
				for (int i = 0; i < m_vecfast01Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast01Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast01Index[i + 1]].p, &m_vecPNTVertex[m_vecfast01Index[i + 2]].p,
						&vRayOrig, &vRayDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							vDest = vRayOrig + vRayDir * f;
							m_vecfast01Index[i + 0];

							break;
						}
					}
				}
			}
			else if (searchSmall < 12) // (16/2)등분
			{
				for (int i = 0; i < m_vecfast02Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast02Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast02Index[i + 1]].p, &m_vecPNTVertex[m_vecfast02Index[i + 2]].p,
						&vRayOrig, &vRayDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							vDest = vRayOrig + vRayDir * f;
							m_vecfast02Index[i + 0];

							break;
						}
					}
				}
			}
			else if (searchSmall < 18) // (16/3)등분
			{
				for (int i = 0; i < m_vecfast03Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast03Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast03Index[i + 1]].p, &m_vecPNTVertex[m_vecfast03Index[i + 2]].p,
						&vRayOrig, &vRayDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							vDest = vRayOrig + vRayDir * f;
							m_vecfast03Index[i + 0];

							break;
						}
					}
				}
			}
			else if (searchSmall < 24) // (16/4)등분
			{
				for (int i = 0; i < m_vecfast04Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast04Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast04Index[i + 1]].p, &m_vecPNTVertex[m_vecfast04Index[i + 2]].p,
						&vRayOrig, &vRayDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							vDest = vRayOrig + vRayDir * f;
							m_vecfast04Index[i + 0];

							break;
						}
					}
				}
			}
		}
		else if (searchBig < 12) // (4/2) -> (16/5)등분 ~ (16/8)등분
		{
			if (searchSmall < 6) // (16/5)등분
			{
				for (int i = 0; i < m_vecfast05Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast05Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast05Index[i + 1]].p, &m_vecPNTVertex[m_vecfast05Index[i + 2]].p,
						&vRayOrig, &vRayDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							vDest = vRayOrig + vRayDir * f;
							m_vecfast05Index[i + 0];

							break;
						}
					}
				}
			}
			else if (searchSmall < 12) // (16/6)등분
			{
				for (int i = 0; i < m_vecfast06Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast06Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast06Index[i + 1]].p, &m_vecPNTVertex[m_vecfast06Index[i + 2]].p,
						&vRayOrig, &vRayDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							vDest = vRayOrig + vRayDir * f;
							m_vecfast06Index[i + 0];

							break;
						}
					}
				}
			}
			else if (searchSmall < 18) // (16/7)등분
			{
				for (int i = 0; i < m_vecfast07Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast07Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast07Index[i + 1]].p, &m_vecPNTVertex[m_vecfast07Index[i + 2]].p,
						&vRayOrig, &vRayDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							vDest = vRayOrig + vRayDir * f;
							m_vecfast07Index[i + 0];

							break;
						}
					}
				}
			}
			else if (searchSmall < 24) // (16/8)등분
			{
				for (int i = 0; i < m_vecfast08Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast08Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast08Index[i + 1]].p, &m_vecPNTVertex[m_vecfast08Index[i + 2]].p,
						&vRayOrig, &vRayDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							vDest = vRayOrig + vRayDir * f;
							m_vecfast08Index[i + 0];

							break;
						}
					}
				}
			}
		}
		else if (searchBig < 18) // (4/3) -> (16/9)등분 ~ (16/12)등분
		{
			if (searchSmall < 6)
			{
				for (int i = 0; i < m_vecfast09Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast09Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast09Index[i + 1]].p, &m_vecPNTVertex[m_vecfast09Index[i + 2]].p,
						&vRayOrig, &vRayDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							vDest = vRayOrig + vRayDir * f;
							m_vecfast09Index[i + 0];

							break;
						}
					}
				}
			}
			else if (searchSmall < 12)
			{
				for (int i = 0; i < m_vecfast10Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast10Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast10Index[i + 1]].p, &m_vecPNTVertex[m_vecfast10Index[i + 2]].p,
						&vRayOrig, &vRayDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							vDest = vRayOrig + vRayDir * f;
							m_vecfast10Index[i + 0];

							break;
						}
					}
				}
			}
			else if (searchSmall < 18)
			{
				for (int i = 0; i < m_vecfast11Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast11Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast11Index[i + 1]].p, &m_vecPNTVertex[m_vecfast11Index[i + 2]].p,
						&vRayOrig, &vRayDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							vDest = vRayOrig + vRayDir * f;
							m_vecfast11Index[i + 0];

							break;
						}
					}
				}
			}
			else if (searchSmall < 24)
			{
				for (int i = 0; i < m_vecfast12Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast12Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast12Index[i + 1]].p, &m_vecPNTVertex[m_vecfast12Index[i + 2]].p,
						&vRayOrig, &vRayDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							vDest = vRayOrig + vRayDir * f;
							m_vecfast12Index[i + 0];

							break;
						}
					}
				}
			}
		}
		else if (searchBig < 24) // (4/4) -> (16/13)등분 ~ (16/16)등분
		{
			if (searchSmall < 6)
			{
				for (int i = 0; i < m_vecfast13Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast13Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast13Index[i + 1]].p, &m_vecPNTVertex[m_vecfast13Index[i + 2]].p,
						&vRayOrig, &vRayDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							vDest = vRayOrig + vRayDir * f;
							m_vecfast13Index[i + 0];

							break;
						}
					}
				}
			}
			else if (searchSmall < 12)
			{
				for (int i = 0; i < m_vecfast14Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast14Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast14Index[i + 1]].p, &m_vecPNTVertex[m_vecfast14Index[i + 2]].p,
						&vRayOrig, &vRayDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							vDest = vRayOrig + vRayDir * f;
							m_vecfast14Index[i + 0];

							break;
						}
					}
				}
			}
			else if (searchSmall < 18)
			{
				for (int i = 0; i < m_vecfast15Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast15Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast15Index[i + 1]].p, &m_vecPNTVertex[m_vecfast15Index[i + 2]].p,
						&vRayOrig, &vRayDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							vDest = vRayOrig + vRayDir * f;
							m_vecfast15Index[i + 0];

							break;
						}
					}
				}
			}
			else if (searchSmall < 24)
			{
				for (int i = 0; i < m_vecfast16Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast16Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast16Index[i + 1]].p, &m_vecPNTVertex[m_vecfast16Index[i + 2]].p,
						&vRayOrig, &vRayDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							vDest = vRayOrig + vRayDir * f;
							m_vecfast16Index[i + 0];

							break;
						}
					}
				}
			}
		}
	}
}

/*============================================================================*/
//MainScene
void cLoadMap::RealLoad_MainScene()
{
	LoadAll_MainSceneTerrain();
	LoadAll_MainSceneX();
}

void cLoadMap::LoadAll_MainSceneTerrain()
{
	FILE* fp;

	//버텍스 갯수 로드
	fp = fopen("./save/MainScene/VertexNum", "r");
	char Temp[1024]; //담을 임시 저장소.
	fgets(Temp, sizeof(Temp), fp);
	int nVertexNum = atoi(Temp);
	fclose(fp);

	m_nVertexSize = nVertexNum;

	//인덱스 설정
	for (int z = 0; z < nVertexNum - 1; ++z)
	{
		for (int x = 0; x < nVertexNum - 1; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * nVertexNum + x + 0;
			int _1 = (z + 1) * nVertexNum + x + 0;
			int _2 = (z + 0) * nVertexNum + x + 1;
			int _3 = (z + 1) * nVertexNum + x + 1;

			m_vecIndex.push_back(_0);
			m_vecIndex.push_back(_1);
			m_vecIndex.push_back(_2);
			m_vecIndex.push_back(_2);
			m_vecIndex.push_back(_1);
			m_vecIndex.push_back(_3);
		}
	}

	//PNT 로드
	vector<D3DXVECTOR3> PNTtemp;
	PNTtemp.clear();
	fp = fopen("./save/MainScene/PNTVertex", "rb");
	while (!feof(fp))
	{
		D3DXVECTOR3 i;
		fread(&i, sizeof(D3DXVECTOR3), 1, fp);
		if (i != '\0')
			PNTtemp.push_back(i);
	}
	fclose(fp);

	int Discriminant = nVertexNum * nVertexNum;
	if (PNTtemp.size() > 0 && Discriminant == (PNTtemp.size() - 1)) //temp가 있고, 그리드 사이즈가 같으면
	{
		m_vecPNTVertex.clear();
		m_vecPNTVertex.resize(nVertexNum * nVertexNum * sizeof(ST_PNT_VERTEX));

		for (int z = 0; z < nVertexNum; ++z)
		{
			for (int x = 0; x < nVertexNum; ++x)
			{
				// 1중배열 방식의 인덱스 번호 셋팅
				int nIndex = z * nVertexNum + x;
				float y = 0.0f;

				D3DXVECTOR3 n;
				ComputeNormal(&n, &m_vecPNTVertex[nIndex + 0].p, &m_vecPNTVertex[nIndex + 1].p, &m_vecPNTVertex[nIndex + 2].p);

				// 캐릭터의 위치에 따른 y 값 반환을 위해서 버텍스 정보만 따로 빼서 보관한다.
				m_vecPNTVertex[nIndex].p = PNTtemp[nIndex];
				m_vecPNTVertex[nIndex].n = n;
				m_vecPNTVertex[nIndex].t = D3DXVECTOR2(x, z);
			}
		}
	}
	else if (PNTtemp.size() > 0 && Discriminant != (PNTtemp.size() - 1))
	{
		char str[300];
		wsprintf(str, "그리드 사이즈가 일치하지 않아 적용되지 않습니다.");
		MessageBox(g_hWnd, str, "파일 불러오기 실패", MB_OK);
	}

	//텍스처 로드
	string tName; //실제 보낼값
				  //char Temp[1024]; //임시로 담을용 //위에서 선언함.
	fp = fopen("./save/MainScene/TextureName", "r");
	//fread(&i, sizeof(string), 1, fp);
	fgets(Temp, sizeof(Temp), fp);
	tName = Temp; //char배열을 그냥 담으면 뒤에 널값들 때문에 string에 담아준다.(string은 알아서 계산해주기 때문에)
	fclose(fp);
	g_pTextureManager->AddTexture(tName, tName, &m_pTexture);
	m_pTexture = g_pTextureManager->GetTexture(tName);


	//최적화 초기 설정
	SliceQuarterSetup(nVertexNum);
}

void cLoadMap::LoadAll_MainSceneX()
{
	FILE* fp;
	string fileName;
	//X 파일 로드

	//X 파일 총 갯수(전체 갯수 파악을 위한 것이기에, 한번만 해주면됨)
	fp = fopen("./save/MainScene/XTotalNum", "r");
	char Temp[1024]; //담을 임시 저장소.
	fgets(Temp, sizeof(Temp), fp);
	int nXTotalNum = atoi(Temp);
	fclose(fp);

	fp = fopen("./save/MainScene/XTotalSize", "r");
	fgets(Temp, sizeof(Temp), fp);
	int nXTotalSize = atoi(Temp);
	fclose(fp);

	if (nXTotalNum > 0) // 존재하는 경우에만
	{
		for (int i = 0; i < nXTotalNum; i++)
		{
			//충돌체 구분
			fileName = "./save/MainScene/XisSphere";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "r");
			fgets(Temp, sizeof(Temp), fp);
			bool bisSphere = atoi(Temp);
			fclose(fp);

			//파일 이름
			fileName = "./save/MainScene/XFileName";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "r");
			fgets(Temp, sizeof(Temp), fp);
			fclose(fp);

			cXobjectLoader* temp = new cXobjectLoader;
			temp->Load(Temp, bisSphere);
			m_vecXLoader.push_back(temp);

			//매쉬 메트리스
			fileName = "./save/MainScene/XMatW";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "rb");
			while (!feof(fp))
			{
				D3DXMATRIXA16 MatW;
				fread(&MatW, sizeof(D3DXMATRIXA16), 1, fp);

				if (MatW != '\0')
					m_vecXLoader[i]->SetMeshMatrix(MatW);
			}
			fclose(fp);

			//매쉬 충돌체 메트리스
			fileName = "./save/MainScene/XCollisionMatW";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "rb");
			while (!feof(fp))
			{
				D3DXMATRIXA16 CollisionMatW;
				fread(&CollisionMatW, sizeof(D3DXMATRIXA16), 1, fp);

				if (CollisionMatW != '\0')
					m_vecXLoader[i]->SetCollisionMatrix(CollisionMatW);
			}
			fclose(fp);
		}
	}
}

/*============================================================================*/
//Village
void cLoadMap::RealLoad_Village()
{
	LoadAll_VillageTerrain();
	LoadAll_VillageX();
}

void cLoadMap::LoadAll_VillageTerrain()
{
	FILE* fp;

	//버텍스 갯수 로드
	fp = fopen("./save/Village/VertexNum", "r");
	char Temp[1024]; //담을 임시 저장소.
	fgets(Temp, sizeof(Temp), fp);
	int nVertexNum = atoi(Temp);
	fclose(fp);

	m_nVertexSize = nVertexNum;

	//인덱스 설정
	for (int z = 0; z < nVertexNum - 1; ++z)
	{
		for (int x = 0; x < nVertexNum - 1; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * nVertexNum + x + 0;
			int _1 = (z + 1) * nVertexNum + x + 0;
			int _2 = (z + 0) * nVertexNum + x + 1;
			int _3 = (z + 1) * nVertexNum + x + 1;

			m_vecIndex.push_back(_0);
			m_vecIndex.push_back(_1);
			m_vecIndex.push_back(_2);
			m_vecIndex.push_back(_2);
			m_vecIndex.push_back(_1);
			m_vecIndex.push_back(_3);
		}
	}

	//PNT 로드
	vector<D3DXVECTOR3> PNTtemp;
	PNTtemp.clear();
	fp = fopen("./save/Village/PNTVertex", "rb");
	while (!feof(fp))
	{
		D3DXVECTOR3 i;
		fread(&i, sizeof(D3DXVECTOR3), 1, fp);
		if (i != '\0')
			PNTtemp.push_back(i);
	}
	fclose(fp);

	int Discriminant = nVertexNum * nVertexNum;
	if (PNTtemp.size() > 0 && Discriminant == (PNTtemp.size() - 1)) //temp가 있고, 그리드 사이즈가 같으면
	{
		m_vecPNTVertex.clear();
		m_vecPNTVertex.resize(nVertexNum * nVertexNum * sizeof(ST_PNT_VERTEX));

		for (int z = 0; z < nVertexNum; ++z)
		{
			for (int x = 0; x < nVertexNum; ++x)
			{
				// 1중배열 방식의 인덱스 번호 셋팅
				int nIndex = z * nVertexNum + x;
				float y = 0.0f;

				D3DXVECTOR3 n;
				ComputeNormal(&n, &m_vecPNTVertex[nIndex + 0].p, &m_vecPNTVertex[nIndex + 1].p, &m_vecPNTVertex[nIndex + 2].p);

				// 캐릭터의 위치에 따른 y 값 반환을 위해서 버텍스 정보만 따로 빼서 보관한다.
				m_vecPNTVertex[nIndex].p = PNTtemp[nIndex];
				m_vecPNTVertex[nIndex].n = n;
				m_vecPNTVertex[nIndex].t = D3DXVECTOR2(x, z);
			}
		}
	}
	else if (PNTtemp.size() > 0 && Discriminant != (PNTtemp.size() - 1))
	{
		char str[300];
		wsprintf(str, "그리드 사이즈가 일치하지 않아 적용되지 않습니다.");
		MessageBox(g_hWnd, str, "파일 불러오기 실패", MB_OK);
	}

	//텍스처 로드
	string tName; //실제 보낼값
				  //char Temp[1024]; //임시로 담을용 //위에서 선언함.
	fp = fopen("./save/Village/TextureName", "r");
	//fread(&i, sizeof(string), 1, fp);
	fgets(Temp, sizeof(Temp), fp);
	tName = Temp; //char배열을 그냥 담으면 뒤에 널값들 때문에 string에 담아준다.(string은 알아서 계산해주기 때문에)
	fclose(fp);
	g_pTextureManager->AddTexture(tName, tName, &m_pTexture);
	m_pTexture = g_pTextureManager->GetTexture(tName);


	//최적화 초기 설정
	SliceQuarterSetup(nVertexNum);
}

void cLoadMap::LoadAll_VillageX()
{
	FILE* fp;
	string fileName;
	//X 파일 로드

	//X 파일 총 갯수(전체 갯수 파악을 위한 것이기에, 한번만 해주면됨)
	fp = fopen("./save/Village/XTotalNum", "r");
	char Temp[1024]; //담을 임시 저장소.
	fgets(Temp, sizeof(Temp), fp);
	int nXTotalNum = atoi(Temp);
	fclose(fp);

	fp = fopen("./save/Village/XTotalSize", "r");
	fgets(Temp, sizeof(Temp), fp);
	int nXTotalSize = atoi(Temp);
	fclose(fp);

	if (nXTotalNum > 0) // 존재하는 경우에만
	{
		for (int i = 0; i < nXTotalNum; i++)
		{
			//충돌체 구분
			fileName = "./save/Village/XisSphere";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "r");
			fgets(Temp, sizeof(Temp), fp);
			bool bisSphere = atoi(Temp);
			fclose(fp);

			//파일 이름
			fileName = "./save/Village/XFileName";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "r");
			fgets(Temp, sizeof(Temp), fp);
			fclose(fp);

			cXobjectLoader* temp = new cXobjectLoader;
			temp->Load(Temp, bisSphere);
			m_vecXLoader.push_back(temp);

			//매쉬 메트리스
			fileName = "./save/Village/XMatW";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "rb");
			while (!feof(fp))
			{
				D3DXMATRIXA16 MatW;
				fread(&MatW, sizeof(D3DXMATRIXA16), 1, fp);

				if (MatW != '\0')
					m_vecXLoader[i]->SetMeshMatrix(MatW);
			}
			fclose(fp);

			//매쉬 충돌체 메트리스
			fileName = "./save/Village/XCollisionMatW";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "rb");
			while (!feof(fp))
			{
				D3DXMATRIXA16 CollisionMatW;
				fread(&CollisionMatW, sizeof(D3DXMATRIXA16), 1, fp);

				if (CollisionMatW != '\0')
					m_vecXLoader[i]->SetCollisionMatrix(CollisionMatW);
			}
			fclose(fp);
		}
	}
}

/*============================================================================*/
//Stage1
void cLoadMap::RealLoad_Stage1()
{
	LoadAll_Stage1Terrain();
	LoadAll_Stage1X();
}

void cLoadMap::LoadAll_Stage1Terrain()
{
	FILE* fp;

	//버텍스 갯수 로드
	fp = fopen("./save/Stage1/VertexNum", "r");
	char Temp[1024]; //담을 임시 저장소.
	fgets(Temp, sizeof(Temp), fp);
	int nVertexNum = atoi(Temp);
	fclose(fp);

	m_nVertexSize = nVertexNum;

	//인덱스 설정
	for (int z = 0; z < nVertexNum - 1; ++z)
	{
		for (int x = 0; x < nVertexNum - 1; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * nVertexNum + x + 0;
			int _1 = (z + 1) * nVertexNum + x + 0;
			int _2 = (z + 0) * nVertexNum + x + 1;
			int _3 = (z + 1) * nVertexNum + x + 1;

			m_vecIndex.push_back(_0);
			m_vecIndex.push_back(_1);
			m_vecIndex.push_back(_2);
			m_vecIndex.push_back(_2);
			m_vecIndex.push_back(_1);
			m_vecIndex.push_back(_3);
		}
	}

	//PNT 로드
	vector<D3DXVECTOR3> PNTtemp;
	PNTtemp.clear();
	fp = fopen("./save/Stage1/PNTVertex", "rb");
	while (!feof(fp))
	{
		D3DXVECTOR3 i;
		fread(&i, sizeof(D3DXVECTOR3), 1, fp);
		if (i != '\0')
			PNTtemp.push_back(i);
	}
	fclose(fp);

	int Discriminant = nVertexNum * nVertexNum;
	if (PNTtemp.size() > 0 && Discriminant == (PNTtemp.size() - 1)) //temp가 있고, 그리드 사이즈가 같으면
	{
		m_vecPNTVertex.clear();
		m_vecPNTVertex.resize(nVertexNum * nVertexNum * sizeof(ST_PNT_VERTEX));

		for (int z = 0; z < nVertexNum; ++z)
		{
			for (int x = 0; x < nVertexNum; ++x)
			{
				// 1중배열 방식의 인덱스 번호 셋팅
				int nIndex = z * nVertexNum + x;
				float y = 0.0f;

				D3DXVECTOR3 n;
				ComputeNormal(&n, &m_vecPNTVertex[nIndex + 0].p, &m_vecPNTVertex[nIndex + 1].p, &m_vecPNTVertex[nIndex + 2].p);

				// 캐릭터의 위치에 따른 y 값 반환을 위해서 버텍스 정보만 따로 빼서 보관한다.
				m_vecPNTVertex[nIndex].p = PNTtemp[nIndex];
				m_vecPNTVertex[nIndex].n = n;
				m_vecPNTVertex[nIndex].t = D3DXVECTOR2(x, z);
			}
		}
	}
	else if (PNTtemp.size() > 0 && Discriminant != (PNTtemp.size() - 1))
	{
		char str[300];
		wsprintf(str, "그리드 사이즈가 일치하지 않아 적용되지 않습니다.");
		MessageBox(g_hWnd, str, "파일 불러오기 실패", MB_OK);
	}

	//텍스처 로드
	string tName; //실제 보낼값
				  //char Temp[1024]; //임시로 담을용 //위에서 선언함.
	fp = fopen("./save/Stage1/TextureName", "r");
	//fread(&i, sizeof(string), 1, fp);
	fgets(Temp, sizeof(Temp), fp);
	tName = Temp; //char배열을 그냥 담으면 뒤에 널값들 때문에 string에 담아준다.(string은 알아서 계산해주기 때문에)
	fclose(fp);
	g_pTextureManager->AddTexture(tName, tName, &m_pTexture);
	m_pTexture = g_pTextureManager->GetTexture(tName);


	//최적화 초기 설정
	SliceQuarterSetup(nVertexNum);
}

void cLoadMap::LoadAll_Stage1X()
{
	FILE* fp;
	string fileName;
	//X 파일 로드

	//X 파일 총 갯수(전체 갯수 파악을 위한 것이기에, 한번만 해주면됨)
	fp = fopen("./save/Stage1/XTotalNum", "r");
	char Temp[1024]; //담을 임시 저장소.
	fgets(Temp, sizeof(Temp), fp);
	int nXTotalNum = atoi(Temp);
	fclose(fp);

	fp = fopen("./save/Stage1/XTotalSize", "r");
	fgets(Temp, sizeof(Temp), fp);
	int nXTotalSize = atoi(Temp);
	fclose(fp);

	if (nXTotalNum > 0) // 존재하는 경우에만
	{
		for (int i = 0; i < nXTotalNum; i++)
		{
			//충돌체 구분
			fileName = "./save/Stage1/XisSphere";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "r");
			fgets(Temp, sizeof(Temp), fp);
			bool bisSphere = atoi(Temp);
			fclose(fp);

			//파일 이름
			fileName = "./save/Stage1/XFileName";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "r");
			fgets(Temp, sizeof(Temp), fp);
			fclose(fp);

			cXobjectLoader* temp = new cXobjectLoader;
			temp->Load(Temp, bisSphere);
			m_vecXLoader.push_back(temp);

			//매쉬 메트리스
			fileName = "./save/Stage1/XMatW";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "rb");
			while (!feof(fp))
			{
				D3DXMATRIXA16 MatW;
				fread(&MatW, sizeof(D3DXMATRIXA16), 1, fp);

				if (MatW != '\0')
					m_vecXLoader[i]->SetMeshMatrix(MatW);
			}
			fclose(fp);

			//매쉬 충돌체 메트리스
			fileName = "./save/Stage1/XCollisionMatW";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "rb");
			while (!feof(fp))
			{
				D3DXMATRIXA16 CollisionMatW;
				fread(&CollisionMatW, sizeof(D3DXMATRIXA16), 1, fp);

				if (CollisionMatW != '\0')
					m_vecXLoader[i]->SetCollisionMatrix(CollisionMatW);
			}
			fclose(fp);
		}
	}
}

/*============================================================================*/
//Stage2
void cLoadMap::RealLoad_Stage2()
{
	LoadAll_Stage2Terrain();
	LoadAll_Stage2X();
}

void cLoadMap::LoadAll_Stage2Terrain()
{
	FILE* fp;

	//버텍스 갯수 로드
	fp = fopen("./save/Stage2/VertexNum", "r");
	char Temp[1024]; //담을 임시 저장소.
	fgets(Temp, sizeof(Temp), fp);
	int nVertexNum = atoi(Temp);
	fclose(fp);

	m_nVertexSize = nVertexNum;

	//인덱스 설정
	for (int z = 0; z < nVertexNum - 1; ++z)
	{
		for (int x = 0; x < nVertexNum - 1; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * nVertexNum + x + 0;
			int _1 = (z + 1) * nVertexNum + x + 0;
			int _2 = (z + 0) * nVertexNum + x + 1;
			int _3 = (z + 1) * nVertexNum + x + 1;

			m_vecIndex.push_back(_0);
			m_vecIndex.push_back(_1);
			m_vecIndex.push_back(_2);
			m_vecIndex.push_back(_2);
			m_vecIndex.push_back(_1);
			m_vecIndex.push_back(_3);
		}
	}

	//PNT 로드
	vector<D3DXVECTOR3> PNTtemp;
	PNTtemp.clear();
	fp = fopen("./save/Stage2/PNTVertex", "rb");
	while (!feof(fp))
	{
		D3DXVECTOR3 i;
		fread(&i, sizeof(D3DXVECTOR3), 1, fp);
		if (i != '\0')
			PNTtemp.push_back(i);
	}
	fclose(fp);

	int Discriminant = nVertexNum * nVertexNum;
	if (PNTtemp.size() > 0 && Discriminant == (PNTtemp.size() - 1)) //temp가 있고, 그리드 사이즈가 같으면
	{
		m_vecPNTVertex.clear();
		m_vecPNTVertex.resize(nVertexNum * nVertexNum * sizeof(ST_PNT_VERTEX));

		for (int z = 0; z < nVertexNum; ++z)
		{
			for (int x = 0; x < nVertexNum; ++x)
			{
				// 1중배열 방식의 인덱스 번호 셋팅
				int nIndex = z * nVertexNum + x;
				float y = 0.0f;

				D3DXVECTOR3 n;
				ComputeNormal(&n, &m_vecPNTVertex[nIndex + 0].p, &m_vecPNTVertex[nIndex + 1].p, &m_vecPNTVertex[nIndex + 2].p);

				// 캐릭터의 위치에 따른 y 값 반환을 위해서 버텍스 정보만 따로 빼서 보관한다.
				m_vecPNTVertex[nIndex].p = PNTtemp[nIndex];
				m_vecPNTVertex[nIndex].n = n;
				m_vecPNTVertex[nIndex].t = D3DXVECTOR2(x, z);
			}
		}
	}
	else if (PNTtemp.size() > 0 && Discriminant != (PNTtemp.size() - 1))
	{
		char str[300];
		wsprintf(str, "그리드 사이즈가 일치하지 않아 적용되지 않습니다.");
		MessageBox(g_hWnd, str, "파일 불러오기 실패", MB_OK);
	}

	//텍스처 로드
	string tName; //실제 보낼값
				  //char Temp[1024]; //임시로 담을용 //위에서 선언함.
	fp = fopen("./save/Stage2/TextureName", "r");
	//fread(&i, sizeof(string), 1, fp);
	fgets(Temp, sizeof(Temp), fp);
	tName = Temp; //char배열을 그냥 담으면 뒤에 널값들 때문에 string에 담아준다.(string은 알아서 계산해주기 때문에)
	fclose(fp);
	g_pTextureManager->AddTexture(tName, tName, &m_pTexture);
	m_pTexture = g_pTextureManager->GetTexture(tName);


	//최적화 초기 설정
	SliceQuarterSetup(nVertexNum);
}

void cLoadMap::LoadAll_Stage2X()
{
	FILE* fp;
	string fileName;
	//X 파일 로드

	//X 파일 총 갯수(전체 갯수 파악을 위한 것이기에, 한번만 해주면됨)
	fp = fopen("./save/Stage2/XTotalNum", "r");
	char Temp[1024]; //담을 임시 저장소.
	fgets(Temp, sizeof(Temp), fp);
	int nXTotalNum = atoi(Temp);
	fclose(fp);

	fp = fopen("./save/Stage2/XTotalSize", "r");
	fgets(Temp, sizeof(Temp), fp);
	int nXTotalSize = atoi(Temp);
	fclose(fp);

	if (nXTotalNum > 0) // 존재하는 경우에만
	{
		for (int i = 0; i < nXTotalNum; i++)
		{
			//충돌체 구분
			fileName = "./save/Stage2/XisSphere";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "r");
			fgets(Temp, sizeof(Temp), fp);
			bool bisSphere = atoi(Temp);
			fclose(fp);

			//파일 이름
			fileName = "./save/Stage2/XFileName";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "r");
			fgets(Temp, sizeof(Temp), fp);
			fclose(fp);

			cXobjectLoader* temp = new cXobjectLoader;
			temp->Load(Temp, bisSphere);
			m_vecXLoader.push_back(temp);

			//매쉬 메트리스
			fileName = "./save/Stage2/XMatW";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "rb");
			while (!feof(fp))
			{
				D3DXMATRIXA16 MatW;
				fread(&MatW, sizeof(D3DXMATRIXA16), 1, fp);

				if (MatW != '\0')
					m_vecXLoader[i]->SetMeshMatrix(MatW);
			}
			fclose(fp);

			//매쉬 충돌체 메트리스
			fileName = "./save/Stage2/XCollisionMatW";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "rb");
			while (!feof(fp))
			{
				D3DXMATRIXA16 CollisionMatW;
				fread(&CollisionMatW, sizeof(D3DXMATRIXA16), 1, fp);

				if (CollisionMatW != '\0')
					m_vecXLoader[i]->SetCollisionMatrix(CollisionMatW);
			}
			fclose(fp);
		}
	}
}

/*============================================================================*/
//Stage3
void cLoadMap::RealLoad_Stage3()
{
	LoadAll_Stage3Terrain();
	LoadAll_Stage3X();
}

void cLoadMap::LoadAll_Stage3Terrain()
{
	FILE* fp;

	//버텍스 갯수 로드
	fp = fopen("./save/Stage3/VertexNum", "r");
	char Temp[1024]; //담을 임시 저장소.
	fgets(Temp, sizeof(Temp), fp);
	int nVertexNum = atoi(Temp);
	fclose(fp);

	m_nVertexSize = nVertexNum;

	//인덱스 설정
	for (int z = 0; z < nVertexNum - 1; ++z)
	{
		for (int x = 0; x < nVertexNum - 1; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * nVertexNum + x + 0;
			int _1 = (z + 1) * nVertexNum + x + 0;
			int _2 = (z + 0) * nVertexNum + x + 1;
			int _3 = (z + 1) * nVertexNum + x + 1;

			m_vecIndex.push_back(_0);
			m_vecIndex.push_back(_1);
			m_vecIndex.push_back(_2);
			m_vecIndex.push_back(_2);
			m_vecIndex.push_back(_1);
			m_vecIndex.push_back(_3);
		}
	}

	//PNT 로드
	vector<D3DXVECTOR3> PNTtemp;
	PNTtemp.clear();
	fp = fopen("./save/Stage3/PNTVertex", "rb");
	while (!feof(fp))
	{
		D3DXVECTOR3 i;
		fread(&i, sizeof(D3DXVECTOR3), 1, fp);
		if (i != '\0')
			PNTtemp.push_back(i);
	}
	fclose(fp);

	int Discriminant = nVertexNum * nVertexNum;
	if (PNTtemp.size() > 0 && Discriminant == (PNTtemp.size() - 1)) //temp가 있고, 그리드 사이즈가 같으면
	{
		m_vecPNTVertex.clear();
		m_vecPNTVertex.resize(nVertexNum * nVertexNum * sizeof(ST_PNT_VERTEX));

		for (int z = 0; z < nVertexNum; ++z)
		{
			for (int x = 0; x < nVertexNum; ++x)
			{
				// 1중배열 방식의 인덱스 번호 셋팅
				int nIndex = z * nVertexNum + x;
				float y = 0.0f;

				D3DXVECTOR3 n;
				ComputeNormal(&n, &m_vecPNTVertex[nIndex + 0].p, &m_vecPNTVertex[nIndex + 1].p, &m_vecPNTVertex[nIndex + 2].p);

				// 캐릭터의 위치에 따른 y 값 반환을 위해서 버텍스 정보만 따로 빼서 보관한다.
				m_vecPNTVertex[nIndex].p = PNTtemp[nIndex];
				m_vecPNTVertex[nIndex].n = n;
				m_vecPNTVertex[nIndex].t = D3DXVECTOR2(x, z);
			}
		}
	}
	else if (PNTtemp.size() > 0 && Discriminant != (PNTtemp.size() - 1))
	{
		char str[300];
		wsprintf(str, "그리드 사이즈가 일치하지 않아 적용되지 않습니다.");
		MessageBox(g_hWnd, str, "파일 불러오기 실패", MB_OK);
	}

	//텍스처 로드
	string tName; //실제 보낼값
				  //char Temp[1024]; //임시로 담을용 //위에서 선언함.
	fp = fopen("./save/Stage3/TextureName", "r");
	//fread(&i, sizeof(string), 1, fp);
	fgets(Temp, sizeof(Temp), fp);
	tName = Temp; //char배열을 그냥 담으면 뒤에 널값들 때문에 string에 담아준다.(string은 알아서 계산해주기 때문에)
	fclose(fp);
	g_pTextureManager->AddTexture(tName, tName, &m_pTexture);
	m_pTexture = g_pTextureManager->GetTexture(tName);


	//최적화 초기 설정
	SliceQuarterSetup(nVertexNum);
}

void cLoadMap::LoadAll_Stage3X()
{
	FILE* fp;
	string fileName;
	//X 파일 로드

	//X 파일 총 갯수(전체 갯수 파악을 위한 것이기에, 한번만 해주면됨)
	fp = fopen("./save/Stage3/XTotalNum", "r");
	char Temp[1024]; //담을 임시 저장소.
	fgets(Temp, sizeof(Temp), fp);
	int nXTotalNum = atoi(Temp);
	fclose(fp);

	fp = fopen("./save/Stage3/XTotalSize", "r");
	fgets(Temp, sizeof(Temp), fp);
	int nXTotalSize = atoi(Temp);
	fclose(fp);

	if (nXTotalNum > 0) // 존재하는 경우에만
	{
		for (int i = 0; i < nXTotalNum; i++)
		{
			//충돌체 구분
			fileName = "./save/Stage3/XisSphere";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "r");
			fgets(Temp, sizeof(Temp), fp);
			bool bisSphere = atoi(Temp);
			fclose(fp);

			//파일 이름
			fileName = "./save/Stage3/XFileName";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "r");
			fgets(Temp, sizeof(Temp), fp);
			fclose(fp);

			cXobjectLoader* temp = new cXobjectLoader;
			temp->Load(Temp, bisSphere);
			m_vecXLoader.push_back(temp);

			//매쉬 메트리스
			fileName = "./save/Stage3/XMatW";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "rb");
			while (!feof(fp))
			{
				D3DXMATRIXA16 MatW;
				fread(&MatW, sizeof(D3DXMATRIXA16), 1, fp);

				if (MatW != '\0')
					m_vecXLoader[i]->SetMeshMatrix(MatW);
			}
			fclose(fp);

			//매쉬 충돌체 메트리스
			fileName = "./save/Stage3/XCollisionMatW";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "rb");
			while (!feof(fp))
			{
				D3DXMATRIXA16 CollisionMatW;
				fread(&CollisionMatW, sizeof(D3DXMATRIXA16), 1, fp);

				if (CollisionMatW != '\0')
					m_vecXLoader[i]->SetCollisionMatrix(CollisionMatW);
			}
			fclose(fp);
		}
	}
}

/*============================================================================*/
//BossMap
void cLoadMap::RealLoad_BossMap()
{
	LoadAll_BossMapTerrain();
	LoadAll_BossMapX();
}

void cLoadMap::LoadAll_BossMapTerrain()
{
	FILE* fp;

	//버텍스 갯수 로드
	fp = fopen("./save/BossMap/VertexNum", "r");
	char Temp[1024]; //담을 임시 저장소.
	fgets(Temp, sizeof(Temp), fp);
	int nVertexNum = atoi(Temp);
	fclose(fp);

	m_nVertexSize = nVertexNum;

	//인덱스 설정
	for (int z = 0; z < nVertexNum - 1; ++z)
	{
		for (int x = 0; x < nVertexNum - 1; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * nVertexNum + x + 0;
			int _1 = (z + 1) * nVertexNum + x + 0;
			int _2 = (z + 0) * nVertexNum + x + 1;
			int _3 = (z + 1) * nVertexNum + x + 1;

			m_vecIndex.push_back(_0);
			m_vecIndex.push_back(_1);
			m_vecIndex.push_back(_2);
			m_vecIndex.push_back(_2);
			m_vecIndex.push_back(_1);
			m_vecIndex.push_back(_3);
		}
	}

	//PNT 로드
	vector<D3DXVECTOR3> PNTtemp;
	PNTtemp.clear();
	fp = fopen("./save/BossMap/PNTVertex", "rb");
	while (!feof(fp))
	{
		D3DXVECTOR3 i;
		fread(&i, sizeof(D3DXVECTOR3), 1, fp);
		if (i != '\0')
			PNTtemp.push_back(i);
	}
	fclose(fp);

	int Discriminant = nVertexNum * nVertexNum;
	if (PNTtemp.size() > 0 && Discriminant == (PNTtemp.size() - 1)) //temp가 있고, 그리드 사이즈가 같으면
	{
		m_vecPNTVertex.clear();
		m_vecPNTVertex.resize(nVertexNum * nVertexNum * sizeof(ST_PNT_VERTEX));

		m_vecVertex.clear();	//추가
		m_vecVertex.resize(nVertexNum * nVertexNum);	//추가

		for (int z = 0; z < nVertexNum; ++z)
		{
			for (int x = 0; x < nVertexNum; ++x)
			{
				// 1중배열 방식의 인덱스 번호 셋팅
				int nIndex = z * nVertexNum + x;
				float y = 0.0f;

				D3DXVECTOR3 n;
				ComputeNormal(&n, &m_vecPNTVertex[nIndex + 0].p, &m_vecPNTVertex[nIndex + 1].p, &m_vecPNTVertex[nIndex + 2].p);

				// 캐릭터의 위치에 따른 y 값 반환을 위해서 버텍스 정보만 따로 빼서 보관한다.
				m_vecPNTVertex[nIndex].p = PNTtemp[nIndex];
				m_vecVertex[nIndex] = m_vecPNTVertex[nIndex].p;	//추가
				m_vecPNTVertex[nIndex].n = n;
				m_vecPNTVertex[nIndex].t = D3DXVECTOR2(x, z);
			}
		}
	}
	else if (PNTtemp.size() > 0 && Discriminant != (PNTtemp.size() - 1))
	{
		char str[300];
		wsprintf(str, "그리드 사이즈가 일치하지 않아 적용되지 않습니다.");
		MessageBox(g_hWnd, str, "파일 불러오기 실패", MB_OK);
	}

	//텍스처 로드
	string tName; //실제 보낼값
				  //char Temp[1024]; //임시로 담을용 //위에서 선언함.
	fp = fopen("./save/BossMap/TextureName", "r");
	//fread(&i, sizeof(string), 1, fp);
	fgets(Temp, sizeof(Temp), fp);
	tName = Temp; //char배열을 그냥 담으면 뒤에 널값들 때문에 string에 담아준다.(string은 알아서 계산해주기 때문에)
	fclose(fp);
	g_pTextureManager->AddTexture(tName, tName, &m_pTexture);
	m_pTexture = g_pTextureManager->GetTexture(tName);


	//최적화 초기 설정
	SliceQuarterSetup(nVertexNum);
}

void cLoadMap::LoadAll_BossMapX()
{
	FILE* fp;
	string fileName;
	//X 파일 로드

	//X 파일 총 갯수(전체 갯수 파악을 위한 것이기에, 한번만 해주면됨)
	fp = fopen("./save/BossMap/XTotalNum", "r");
	char Temp[1024]; //담을 임시 저장소.
	fgets(Temp, sizeof(Temp), fp);
	int nXTotalNum = atoi(Temp);
	fclose(fp);

	fp = fopen("./save/BossMap/XTotalSize", "r");
	fgets(Temp, sizeof(Temp), fp);
	int nXTotalSize = atoi(Temp);
	fclose(fp);

	if (nXTotalNum > 0) // 존재하는 경우에만
	{
		for (int i = 0; i < nXTotalNum; i++)
		{
			//충돌체 구분
			fileName = "./save/BossMap/XisSphere";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "r");
			fgets(Temp, sizeof(Temp), fp);
			bool bisSphere = atoi(Temp);
			fclose(fp);

			//파일 이름
			fileName = "./save/BossMap/XFileName";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "r");
			fgets(Temp, sizeof(Temp), fp);
			fclose(fp);

			cXobjectLoader* temp = new cXobjectLoader;
			temp->Load(Temp, bisSphere);
			m_vecXLoader.push_back(temp);

			//매쉬 메트리스
			fileName = "./save/BossMap/XMatW";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "rb");
			while (!feof(fp))
			{
				D3DXMATRIXA16 MatW;
				fread(&MatW, sizeof(D3DXMATRIXA16), 1, fp);

				if (MatW != '\0')
					m_vecXLoader[i]->SetMeshMatrix(MatW);
			}
			fclose(fp);

			//매쉬 충돌체 메트리스
			fileName = "./save/BossMap/XCollisionMatW";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "rb");
			while (!feof(fp))
			{
				D3DXMATRIXA16 CollisionMatW;
				fread(&CollisionMatW, sizeof(D3DXMATRIXA16), 1, fp);

				if (CollisionMatW != '\0')
					m_vecXLoader[i]->SetCollisionMatrix(CollisionMatW);
			}
			fclose(fp);
		}
	}
}