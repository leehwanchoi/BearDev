#include "stdafx.h"
#include "cTerrain.h"
#include "cXobjectLoader.h"

/*
float rsq = x * x + z * z;
float fHeight = 3.0f * expf(-rsq / (2.0f * 3.0f));
vecPNTVertex[nIndex].p = D3DXVECTOR3(x, -5.0f + fHeight, z);
*/

/*
그리드 옵션
4
8
16
32
64
128
*/

cTerrain::cTerrain()
	: m_CameraRay()
	, m_isPicked(false)
	, m_isLButtonDown(false)
	, m_vDest(0, 0, 0)
	, m_pSphere(NULL)
	, m_isFirst(true)
	, m_nVertexNum(128 + 1)
	, m_nTileNum(m_nVertexNum - 1)
	, m_isLoaded(NULL)
	, m_fVertexMaxHeight(5.0f)
	, m_fVertexMinHeight(0.0f)
	, m_fFlatteningHeight(1.0f)
	, m_pTexture(NULL)
	, m_isMod(false)
	, m_XIndex(0)
	, m_isfastCheck(false)
	, m_fTerrainSpeed(5.0f)
{
	D3DXMatrixIdentity(&m_matWorld);
}


cTerrain::~cTerrain()
{
	SAFE_RELEASE(m_pSphere);

	for (int i = 0; i < m_vecXLoader.size(); i++)
		m_vecXLoader[i]->Destroy();
}

void cTerrain::Setup()
{
	if (m_isFirst)
		D3DXCreateSphere(g_pD3DDevice, 0.1f, 10, 10, &m_pSphere, NULL);

	m_TextureName = "./MapToolTexture/snowTexture.jpg";
	g_pTextureManager->AddTexture(m_TextureName, m_TextureName, &m_pTexture);

	//그리드를 다시 설정하는 경우를 위해 모든 벡터 클리어
	D3DXMatrixIdentity(&m_matWorld);
	m_vDest = D3DXVECTOR3(0, 0, 0);
	m_vecVertex.clear();
	m_vecIndex.clear();
	m_fastVertex.clear();
	m_fastVertexHalf01.clear();
	m_fastVertexHalf02.clear();
	m_fastVertexHalf03.clear();
	m_fastVertexHalf04.clear();
	m_vecfast01Index.clear();
	m_vecfast02Index.clear();
	m_vecfast03Index.clear();
	m_vecfast04Index.clear();
	m_vecfast05Index.clear();
	m_vecfast06Index.clear();
	m_vecfast07Index.clear();
	m_vecfast08Index.clear();
	m_vecfast09Index.clear();
	m_vecfast10Index.clear();
	m_vecfast11Index.clear();
	m_vecfast12Index.clear();
	m_vecfast13Index.clear();
	m_vecfast14Index.clear();
	m_vecfast15Index.clear();
	m_vecfast16Index.clear();

	m_vecVertex.resize(m_nVertexNum * m_nVertexNum);
	m_vecPNTVertex.clear();
	m_vecPNTVertex.resize(m_nVertexNum * m_nVertexNum * sizeof(ST_PNT_VERTEX));

	for (int z = 0; z < m_nVertexNum; ++z)
	{
		for (int x = 0; x < m_nVertexNum; ++x)
		{
			// 1중배열 방식의 인덱스 번호 셋팅
			int nIndex = z * m_nVertexNum + x;
			float y = 0.0f;

			D3DXVECTOR3 n;
			ComputeNormal(&n, &m_vecPNTVertex[nIndex + 0].p, &m_vecPNTVertex[nIndex + 1].p, &m_vecPNTVertex[nIndex + 2].p);

			// 캐릭터의 위치에 따른 y 값 반환을 위해서 버텍스 정보만 따로 빼서 보관한다.
			m_vecVertex[nIndex] = D3DXVECTOR3(x, y, z);

			m_vecPNTVertex[nIndex].p = D3DXVECTOR3(x, y, z);
			m_vecPNTVertex[nIndex].n = n;
			m_vecPNTVertex[nIndex].t = D3DXVECTOR2(x, z);

			if (m_matWorld)
				D3DXVec3TransformCoord(&m_vecVertex[nIndex], &m_vecVertex[nIndex], &m_matWorld);
		}
	}

	// 맵이 변경 된 사이즈에 따라서 캐릭터의 위치를 계산해야 하기 때문에
	// 변경 된 사이즈 정보를 저장한다.
	m_fSizeX = m_matWorld._11;
	m_fSizeZ = m_matWorld._33;

	// 인덱스 번호를 구성한다.
	//vector<DWORD> vecIndex;
	//vecIndex.reserve(m_nTileNum * m_nTileNum * 3 * 2);

	for (int z = 0; z < m_nTileNum; ++z)
	{
		for (int x = 0; x < m_nTileNum; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * m_nVertexNum + x + 0;
			int _1 = (z + 1) * m_nVertexNum + x + 0;
			int _2 = (z + 0) * m_nVertexNum + x + 1;
			int _3 = (z + 1) * m_nVertexNum + x + 1;

			m_vecIndex.push_back(_0);
			m_vecIndex.push_back(_1);
			m_vecIndex.push_back(_2);
			m_vecIndex.push_back(_2);
			m_vecIndex.push_back(_1);
			m_vecIndex.push_back(_3);
		}
	}

	//빠른 검색을 위한 작업
	SliceQuarterSetup();

	//최초 실행후 false로 놓고, 변경 안되게 막는다. (구의 동적할당의 재실행 방지)
	m_isFirst = false;

	//X File
}

void cTerrain::Update()
{
	// 맵이 변경 된 사이즈에 따라서 캐릭터의 위치를 계산해야 하기 때문에
	// 변경 된 사이즈 정보를 저장한다.
	m_fSizeX = m_matWorld._11;
	m_fSizeZ = m_matWorld._33;

	//X파일 모드가 아닐때만 터레인모드
	if (!m_isMod)
		CreateTerrain(m_dwDest);

	//XLoader인덱스 변경하는 함수
	FindXIndex();
}

void cTerrain::Render()
{
	g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	//X File
	for (int i = 0; i < m_vecXLoader.size(); i++)
		m_vecXLoader[i]->Render();

	//X 파일 선택된 인덱스의 충돌체 그리기(선택된것만 빨간색으로)
	if (m_vecXLoader.size() > 0)
		m_vecXLoader[m_XIndex]->SelectedCollisionRender();

	D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity(&matWorld);

	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, true);
	//마우스 위치 표시할 구 그리기.
	g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	g_pD3DDevice->SetTexture(0, NULL);
	D3DXMatrixTranslation(&matWorld, m_vDest.x, m_vDest.y, m_vDest.z);
	g_pD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);
	g_pD3DDevice->SetMaterial(&RED_MTRL);
	m_pSphere->DrawSubset(0);
	g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	//월드매트리스 설정
	g_pD3DDevice->SetTransform(D3DTS_WORLD, &m_matWorld);
	
	//메터리얼 텍스쳐 설정
	if (!g_pKeyManager->isToggleKey(VK_CAPITAL))
		g_pD3DDevice->SetTexture(0, m_pTexture);

	//그리드 텍스쳐 그릴때는 라이트사용안함.
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, false);
	g_pD3DDevice->SetFVF(ST_PNT_VERTEX::FVF);

	//트라이앵글로 한번 그림
	//g_pD3DDevice->DrawIndexedPrimitiveUP(
	//	D3DPT_TRIANGLELIST,				// 도형 그리기 타입
	//	0,								// 맨앞(시작) 버텍스의 인덱스
	//	m_vecVertex.size(),				// 사용되는 버텍스의 수
	//	m_vecIndex.size() / 3,			// 도형의 수
	//	&m_vecIndex[0],					// 인덱스 정보를 저장한 배열
	//	D3DFMT_INDEX32,					// 인덱스 포맷 ( 16비트인지 32비트인지 )
	//	&m_vecVertex[0],				// 버텍스 정보를 저장한 배열
	//	sizeof(D3DXVECTOR3));			// 버텍스의 사이즈

	g_pD3DDevice->SetMaterial(&WHITE_MTRL);
	//최종본 트라이앵글로 그림
	g_pD3DDevice->DrawIndexedPrimitiveUP(
		D3DPT_TRIANGLELIST,				// 도형 그리기 타입
		0,								// 맨앞(시작) 버텍스의 인덱스
		m_vecPNTVertex.size(),				// 사용되는 버텍스의 수
		m_vecIndex.size() / 3,			// 도형의 수
		&m_vecIndex[0],					// 인덱스 정보를 저장한 배열
		D3DFMT_INDEX32,					// 인덱스 포맷 ( 16비트인지 32비트인지 )
		&m_vecPNTVertex[0],				// 버텍스 정보를 저장한 배열
		sizeof(ST_PNT_VERTEX));			// 버텍스의 사이즈

	//조명 끔
	if (g_pKeyManager->isToggleKey(VK_NUMLOCK))
		g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, true);
	else
		g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, false);

	//와이어로 그리기
	g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	//컬링 제거
	g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	g_pD3DDevice->SetMaterial(&GRAY_MTRL);

	//와이어로 한번 그림
	//g_pD3DDevice->DrawIndexedPrimitiveUP(
	//	D3DPT_TRIANGLELIST,				// 도형 그리기 타입
	//	0,								// 맨앞(시작) 버텍스의 인덱스
	//	m_vecVertex.size(),				// 사용되는 버텍스의 수
	//	m_vecIndex.size() / 3,			// 도형의 수
	//	&m_vecIndex[0],					// 인덱스 정보를 저장한 배열
	//	D3DFMT_INDEX32,					// 인덱스 포맷 ( 16비트인지 32비트인지 )
	//	&m_vecVertex[0],				// 버텍스 정보를 저장한 배열
	//	sizeof(D3DXVECTOR3));			// 버텍스의 사이즈

	//최종본 와이어로 그림
	g_pD3DDevice->DrawIndexedPrimitiveUP(
		D3DPT_TRIANGLELIST,				// 도형 그리기 타입
		0,								// 맨앞(시작) 버텍스의 인덱스
		m_vecPNTVertex.size(),				// 사용되는 버텍스의 수
		m_vecIndex.size() / 3,			// 도형의 수
		&m_vecIndex[0],					// 인덱스 정보를 저장한 배열
		D3DFMT_INDEX32,					// 인덱스 포맷 ( 16비트인지 32비트인지 )
		&m_vecPNTVertex[0],				// 버텍스 정보를 저장한 배열
		sizeof(ST_PNT_VERTEX));			// 버텍스의 사이즈

	//그리드 테스트용
	//g_pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_fastVertex.size() / 3, m_fastVertex[0], sizeof(D3DXVECTOR3));
	//g_pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_fastVertexHalf01.size() / 3, m_fastVertexHalf01[0], sizeof(D3DXVECTOR3));
	//g_pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_fastVertexHalf02.size() / 3, m_fastVertexHalf02[0], sizeof(D3DXVECTOR3));
	//g_pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_fastVertexHalf03.size() / 3, m_fastVertexHalf03[0], sizeof(D3DXVECTOR3));
	//g_pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_fastVertexHalf04.size() / 3, m_fastVertexHalf04[0], sizeof(D3DXVECTOR3));

	//조명 켬
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, false);
	//와이어->원래대로
	g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	//컬링 복구
	g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void cTerrain::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_MOUSEMOVE:
	{
		m_CameraRay = cRay::RayAtWorldSpace(LOWORD(lParam), HIWORD(lParam));
		//클릭했을시 좌표 갱신 및 터레인 작동
		SliceQuarterCalculation();

		//미 클릭시 마우스 좌표 갱신 함수
		if (!m_isLButtonDown)
			RealtimeMousePositionCalculation();

		if (m_isLButtonDown)
		{

			////원본으로 검색 (최적화 전)
			//for (int i = 0; i < m_vecIndex.size(); i += 3)
			//{
			//	float f;
			//	if (D3DXIntersectTri(&m_vecVertex[m_vecIndex[i + 0]],
			//		&m_vecVertex[m_vecIndex[i + 1]], &m_vecVertex[m_vecIndex[i + 2]],
			//		&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
			//	{
			//		if (f < minF)
			//		{
			//			minF = f;
			//			m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
			//			m_dwDest = m_vecIndex[i + 0];
			//			m_isPicked = true;
			//			break;
			//		}
			//	}
			//}
		}
	}
	break;
	case WM_LBUTTONDOWN:
	{
		m_isLButtonDown = true;

		//m_CameraRay = cRay::RayAtWorldSpace(LOWORD(lParam), HIWORD(lParam));
		//float minF = 100000;

		//for (int i = 0; i < m_vecIndex.size(); i += 3)
		//{
		//	float f;
		//	if (D3DXIntersectTri(&m_vecVertex[m_vecIndex[i + 0]],
		//		&m_vecVertex[m_vecIndex[i + 1]], &m_vecVertex[m_vecIndex[i + 2]],
		//		&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
		//	{
		//		if (f < minF)
		//		{
		//			minF = f;
		//			m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
		//			m_dwDest = m_vecIndex[i + 0];
		//			m_isPicked = true;
		//		}
		//	}
		//}
	}
	break;
	case WM_LBUTTONUP:
		m_isLButtonDown = false;
		m_isPicked = false;
		break;
	}
}

bool cTerrain::GetHeight(IN const float x, OUT float & y, IN const float z)
{
	// 맵을 벗어난 경우 (사이즈가 변경 된 경우 변경 된 사이즈로 좌표를 나눠서 계산한다.)
	if (x < 0 || x / m_fSizeX > m_nTileNum || z < 0 || z / m_fSizeZ > m_nTileNum)
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
	int _0 = (nZ + 0) * m_nVertexNum + nX + 0;
	int _1 = (nZ + 1) * m_nVertexNum + nX + 0;
	int _2 = (nZ + 0) * m_nVertexNum + nX + 1;
	int _3 = (nZ + 1) * m_nVertexNum + nX + 1;

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

void cTerrain::CreateTerrain(DWORD index)
{
	if (m_isPicked)
	{
		//m_vecVertex[i].y -> PNT[i].p.y로 변경
		if (!g_pKeyManager->isStayKeyDown(VK_LSHIFT))
			m_vecPNTVertex[index + 0].p.y += m_fTerrainSpeed * g_pTimeManager->getElapsedTime();
		else if (g_pKeyManager->isStayKeyDown(VK_LSHIFT))
			m_vecPNTVertex[index + 0].p.y -= m_fTerrainSpeed * g_pTimeManager->getElapsedTime();

		if (m_vecPNTVertex[index + 0].p.y < m_fVertexMinHeight)
			m_vecPNTVertex[index + 0].p.y = m_fVertexMinHeight;

		if (m_vecPNTVertex[index + 0].p.y > m_fVertexMaxHeight)
			m_vecPNTVertex[index + 0].p.y = m_fVertexMaxHeight;

		if (g_pKeyManager->isStayKeyDown(VK_CONTROL))
			m_vecPNTVertex[index + 0].p.y = m_fFlatteningHeight;

		/*
		if (index + 1 + m_nVertexNum)
		{
			if (!g_pKeyManager->isStayKeyDown(VK_LSHIFT))
			{
				m_vecVertex[index + 0].y += +5.0f * g_pTimeManager->getElapsedTime();
				m_vecVertex[index + 1].y += +5.0f * g_pTimeManager->getElapsedTime();
				m_vecVertex[index + 0 + m_nVertexNum].y += +5.0f * g_pTimeManager->getElapsedTime();
				m_vecVertex[index + 1 + m_nVertexNum].y += +5.0f * g_pTimeManager->getElapsedTime();
			}
			else if (g_pKeyManager->isStayKeyDown(VK_LSHIFT))
			{
				m_vecVertex[index + 0].y -= +5.0f * g_pTimeManager->getElapsedTime();
				m_vecVertex[index + 1].y -= +5.0f * g_pTimeManager->getElapsedTime();
				m_vecVertex[index + 0 + m_nVertexNum].y -= +5.0f * g_pTimeManager->getElapsedTime();
				m_vecVertex[index + 1 + m_nVertexNum].y -= +5.0f * g_pTimeManager->getElapsedTime();
			}


			//여기부터 최초 최대 값 처리
			if (m_vecVertex[index + 0].y < 0.0f)
				m_vecVertex[index + 0].y = 0.0f;

			if (m_vecVertex[index + 0].y > 2.0f)
				m_vecVertex[index + 0].y = 2.0f;

			if (m_vecVertex[index + 1].y < 0.0f)
				m_vecVertex[index + 1].y = 0.0f;

			if (m_vecVertex[index + 1].y > 2.0f)
				m_vecVertex[index + 1].y = 2.0f;

			if (m_vecVertex[index + 0 + m_nVertexNum].y < 0.0f)
				m_vecVertex[index + 0 + m_nVertexNum].y = 0.0f;

			if (m_vecVertex[index + 0 + m_nVertexNum].y > 2.0f)
				m_vecVertex[index + 0 + m_nVertexNum].y = 2.0f;

			if (m_vecVertex[index + 1 + m_nVertexNum].y < 0.0f)
				m_vecVertex[index + 1 + m_nVertexNum].y = 0.0f;

			if (m_vecVertex[index + 1 + m_nVertexNum].y > 2.0f)
				m_vecVertex[index + 1 + m_nVertexNum].y = 2.0f;
		}
		*/
	}
}

void cTerrain::SliceQuarterSetup()
{
	//초기 빠른 검색 인덱스 설정
	m_fastSearch[0] = 0;
	m_fastSearch[1] = m_nTileNum / 2;
	m_fastSearch[2] = m_nTileNum;
	m_fastSearch[3] = m_nTileNum * m_nVertexNum / 2;
	m_fastSearch[4] = (m_nTileNum * m_nVertexNum / 2) + (m_nTileNum / 2);
	m_fastSearch[5] = (m_nTileNum * m_nVertexNum / 2) + m_nTileNum;
	m_fastSearch[6] = m_nTileNum * m_nVertexNum;
	m_fastSearch[7] = ((m_nVertexNum * m_nVertexNum) - 1) - (m_nTileNum / 2);
	m_fastSearch[8] = m_nVertexNum * m_nVertexNum - 1;

	/*
	6 7 8
	3 4 5
	0 1 2
	*/

	// 크게 한번 검색하기 위해, 분기별로 나눔.(1~4분기)
	if (m_fastSearch[8] > 0)
	{
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[0]]);//1~3
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[3]]);
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[1]]);
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[1]]);//4~6 //1분기 (x < m_fastSearch[1]) (z < m_fastSearch[3])
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[3]]);
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[4]]);
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[1]]);//7~9
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[4]]);
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[2]]);
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[2]]);//10~12 //2분기
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[4]]);
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[5]]);
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[3]]);//13~15
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[6]]);
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[4]]);
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[4]]);//16~18 //3분기
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[6]]);
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[7]]);
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[4]]);//19~21
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[7]]);
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[5]]);
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[5]]);//22~24 //4분기
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[7]]);
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[8]]);
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
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[0]]);//1~3
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[3]]);
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[1]]);
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[1]]);//4~6 //1분기 (x < m_fastSearchHalf[1]) (z < m_fastSearchHalf[3])
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[3]]);
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[4]]);
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[1]]);//7~9
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[4]]);
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[2]]);
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[2]]);//10~12 //2분기
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[4]]);
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[5]]);
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[3]]);//13~15
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[6]]);
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[4]]);
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[4]]);//16~18 //3분기
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[6]]);
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[7]]);
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[4]]);//19~21
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[7]]);
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[5]]);
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[5]]);//22~24 //4분기
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[7]]);
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[8]]);
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
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[0]]);//1~3
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[3]]);
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[1]]);
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[1]]);//4~6 //1분기 (x < m_fastSearchHalf[1]) (z < m_fastSearchHalf[3])
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[3]]);
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[4]]);
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[1]]);//7~9
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[4]]);
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[2]]);
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[2]]);//10~12 //2분기
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[4]]);
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[5]]);
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[3]]);//13~15
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[6]]);
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[4]]);
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[4]]);//16~18 //3분기
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[6]]);
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[7]]);
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[4]]);//19~21
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[7]]);
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[5]]);
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[5]]);//22~24 //4분기
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[7]]);
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[8]]);
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
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[0]]);//1~3
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[3]]);
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[1]]);
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[1]]);//4~6 //1분기 (x < m_fastSearchHalf[1]) (z < m_fastSearchHalf[3])
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[3]]);
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[4]]);
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[1]]);//7~9
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[4]]);
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[2]]);
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[2]]);//10~12 //2분기
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[4]]);
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[5]]);
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[3]]);//13~15
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[6]]);
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[4]]);
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[4]]);//16~18 //3분기
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[6]]);
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[7]]);
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[4]]);//19~21
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[7]]);
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[5]]);
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[5]]);//22~24 //4분기
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[7]]);
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[8]]);
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
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[0]]);//1~3
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[3]]);
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[1]]);
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[1]]);//4~6 //1분기 (x < m_fastSearchHalf[1]) (z < m_fastSearchHalf[3])
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[3]]);
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[4]]);
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[1]]);//7~9
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[4]]);
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[2]]);
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[2]]);//10~12 //2분기
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[4]]);
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[5]]);
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[3]]);//13~15
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[6]]);
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[4]]);
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[4]]);//16~18 //3분기
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[6]]);
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[7]]);
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[4]]);//19~21
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[7]]);
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[5]]);
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[5]]);//22~24 //4분기
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[7]]);
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[8]]);
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
			int _0 = (z + 0) * m_nVertexNum + x + 0;
			int _1 = (z + 1) * m_nVertexNum + x + 0;
			int _2 = (z + 0) * m_nVertexNum + x + 1;
			int _3 = (z + 1) * m_nVertexNum + x + 1;

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
			int _0 = (z + 0) * m_nVertexNum + x + 0;
			int _1 = (z + 1) * m_nVertexNum + x + 0;
			int _2 = (z + 0) * m_nVertexNum + x + 1;
			int _3 = (z + 1) * m_nVertexNum + x + 1;

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
			int _0 = (z + 0) * m_nVertexNum + x + 0;
			int _1 = (z + 1) * m_nVertexNum + x + 0;
			int _2 = (z + 0) * m_nVertexNum + x + 1;
			int _3 = (z + 1) * m_nVertexNum + x + 1;

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
			int _0 = (z + 0) * m_nVertexNum + x + 0;
			int _1 = (z + 1) * m_nVertexNum + x + 0;
			int _2 = (z + 0) * m_nVertexNum + x + 1;
			int _3 = (z + 1) * m_nVertexNum + x + 1;

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
			int _0 = (z + 0) * m_nVertexNum + x + 0;
			int _1 = (z + 1) * m_nVertexNum + x + 0;
			int _2 = (z + 0) * m_nVertexNum + x + 1;
			int _3 = (z + 1) * m_nVertexNum + x + 1;

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
			int _0 = (z + 0) * m_nVertexNum + x + 0;
			int _1 = (z + 1) * m_nVertexNum + x + 0;
			int _2 = (z + 0) * m_nVertexNum + x + 1;
			int _3 = (z + 1) * m_nVertexNum + x + 1;

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
			int _0 = (z + 0) * m_nVertexNum + x + 0;
			int _1 = (z + 1) * m_nVertexNum + x + 0;
			int _2 = (z + 0) * m_nVertexNum + x + 1;
			int _3 = (z + 1) * m_nVertexNum + x + 1;

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
			int _0 = (z + 0) * m_nVertexNum + x + 0;
			int _1 = (z + 1) * m_nVertexNum + x + 0;
			int _2 = (z + 0) * m_nVertexNum + x + 1;
			int _3 = (z + 1) * m_nVertexNum + x + 1;

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
			int _0 = (z + 0) * m_nVertexNum + x + 0;
			int _1 = (z + 1) * m_nVertexNum + x + 0;
			int _2 = (z + 0) * m_nVertexNum + x + 1;
			int _3 = (z + 1) * m_nVertexNum + x + 1;

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
			int _0 = (z + 0) * m_nVertexNum + x + 0;
			int _1 = (z + 1) * m_nVertexNum + x + 0;
			int _2 = (z + 0) * m_nVertexNum + x + 1;
			int _3 = (z + 1) * m_nVertexNum + x + 1;

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
			int _0 = (z + 0) * m_nVertexNum + x + 0;
			int _1 = (z + 1) * m_nVertexNum + x + 0;
			int _2 = (z + 0) * m_nVertexNum + x + 1;
			int _3 = (z + 1) * m_nVertexNum + x + 1;

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
			int _0 = (z + 0) * m_nVertexNum + x + 0;
			int _1 = (z + 1) * m_nVertexNum + x + 0;
			int _2 = (z + 0) * m_nVertexNum + x + 1;
			int _3 = (z + 1) * m_nVertexNum + x + 1;

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
			int _0 = (z + 0) * m_nVertexNum + x + 0;
			int _1 = (z + 1) * m_nVertexNum + x + 0;
			int _2 = (z + 0) * m_nVertexNum + x + 1;
			int _3 = (z + 1) * m_nVertexNum + x + 1;

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
			int _0 = (z + 0) * m_nVertexNum + x + 0;
			int _1 = (z + 1) * m_nVertexNum + x + 0;
			int _2 = (z + 0) * m_nVertexNum + x + 1;
			int _3 = (z + 1) * m_nVertexNum + x + 1;

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
			int _0 = (z + 0) * m_nVertexNum + x + 0;
			int _1 = (z + 1) * m_nVertexNum + x + 0;
			int _2 = (z + 0) * m_nVertexNum + x + 1;
			int _3 = (z + 1) * m_nVertexNum + x + 1;

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
			int _0 = (z + 0) * m_nVertexNum + x + 0;
			int _1 = (z + 1) * m_nVertexNum + x + 0;
			int _2 = (z + 0) * m_nVertexNum + x + 1;
			int _3 = (z + 1) * m_nVertexNum + x + 1;

			m_vecfast16Index.push_back(_0);
			m_vecfast16Index.push_back(_1);
			m_vecfast16Index.push_back(_2);
			m_vecfast16Index.push_back(_2);
			m_vecfast16Index.push_back(_1);
			m_vecfast16Index.push_back(_3);
		}
	}
}

void cTerrain::SliceQuarterCalculation()
{
	float minF = 100000;

	bool isBig = false;
	bool isSmall = false;

	int searchBig = -100000; //오류시 터지기 위해 -큰값 (분기 확인용)
	int searchSmall = -100000; //오류시 터지기 위해 -큰값 (세분화 분기 확인용)

	//심플로 검색 (4분기로 검색)
	if (m_isLButtonDown)
	{
		for (int i = 0; i < m_fastVertex.size(); i += 3)
		{
			float f;
			if (D3DXIntersectTri(&m_fastVertex[i + 0],
				&m_fastVertex[i + 1], &m_fastVertex[i + 2],
				&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
			{
				if (f < minF)
				{
					minF = f;
					//m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
					//m_dwDest = i + 0;
					//m_isPicked = true;
					isBig = true;
					searchBig = i + 0;

					//그리드 픽킹됬는지 확인하기 위해 저장
					m_isfastCheck = true;

			
				}
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
					&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
				{
					if (f < minF)
					{
						minF = f;
						//m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
						//m_dwDest = i + 0;
						isSmall = true;
						searchSmall = i + 0;

					
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
					&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
				{
					if (f < minF)
					{
						minF = f;
						//m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
						//m_dwDest = i + 0;
						isSmall = true;
						searchSmall = i + 0;

				
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
					&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
				{
					if (f < minF)
					{
						minF = f;
						//m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
						//m_dwDest = i + 0;
						isSmall = true;
						searchSmall = i + 0;

				
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
					&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
				{
					if (f < minF)
					{
						minF = f;
						//m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
						//m_dwDest = i + 0;
						isSmall = true;
						searchSmall = i + 0;

				
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
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast01Index[i + 0];
							m_isPicked = true;

						
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
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast02Index[i + 0];
							m_isPicked = true;

				
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
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast03Index[i + 0];
							m_isPicked = true;

					
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
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast04Index[i + 0];
							m_isPicked = true;

					
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
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast05Index[i + 0];
							m_isPicked = true;

					
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
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast06Index[i + 0];
							m_isPicked = true;

					
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
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast07Index[i + 0];
							m_isPicked = true;

					
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
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast08Index[i + 0];
							m_isPicked = true;

				
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
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast09Index[i + 0];
							m_isPicked = true;

				
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
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast10Index[i + 0];
							m_isPicked = true;

			
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
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast11Index[i + 0];
							m_isPicked = true;

					
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
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast12Index[i + 0];
							m_isPicked = true;

						
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
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast13Index[i + 0];
							m_isPicked = true;

					
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
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast14Index[i + 0];
							m_isPicked = true;

					
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
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast15Index[i + 0];
							m_isPicked = true;

					
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
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast16Index[i + 0];
							m_isPicked = true;

						
						}
					}
				}
			}
		}
	}
}

void cTerrain::RealtimeMousePositionCalculation()
{
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
			&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
		{
			if (f < minF)
			{
				minF = f;
				isBig = true;
				searchBig = i + 0;

				
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
					&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
				{
					if (f < minF)
					{
						minF = f;
						isSmall = true;
						searchSmall = i + 0;

					
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
					&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
				{
					if (f < minF)
					{
						minF = f;
						isSmall = true;
						searchSmall = i + 0;

				
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
					&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
				{
					if (f < minF)
					{
						minF = f;
						isSmall = true;
						searchSmall = i + 0;

					
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
					&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
				{
					if (f < minF)
					{
						minF = f;
						isSmall = true;
						searchSmall = i + 0;

					
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
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast01Index[i + 0];

						
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
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast02Index[i + 0];

						
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
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast03Index[i + 0];

						
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
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast04Index[i + 0];

					
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
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast05Index[i + 0];

						
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
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast06Index[i + 0];

				
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
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast07Index[i + 0];

					
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
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast08Index[i + 0];

					
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
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast09Index[i + 0];

					
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
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast10Index[i + 0];

				
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
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast11Index[i + 0];

				
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
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast12Index[i + 0];

					
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
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast13Index[i + 0];

						
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
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast14Index[i + 0];

					
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
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast15Index[i + 0];

					
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
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast16Index[i + 0];

					
						}
					}
				}
			}
		}
	}
}

void cTerrain::Load()
{
	//로드(txtLoadD3DXVECTOR3()함수 만들기전)
	/*vector<string> temp;
	temp = g_pTXTDATAManager->txtLoadDialog(m_isLoaded);
	for (int i = 0; i < temp.size(); i++)
	{
	m_vecVertex[i].y = atof(temp[i].c_str());
	}*/

	vector<D3DXVECTOR3> temp;
	temp.clear();
	temp = g_pTXTDATAManager->txtLoadD3DXVECTOR3(m_isLoaded);

	int Discriminant = m_nVertexNum * m_nVertexNum;
	if (temp.size() > 0 && Discriminant == (temp.size() - 1)) //temp가 있으면
	{
		//수정용
		m_vecVertex.clear();
		m_vecVertex.resize(temp.size() - 1); //temp에서 -1해줘야함.
		//최종
		m_vecPNTVertex.clear();
		m_vecPNTVertex.resize(m_nVertexNum * m_nVertexNum * sizeof(ST_PNT_VERTEX));

		for (int z = 0; z < m_nVertexNum; ++z)
		{
			for (int x = 0; x < m_nVertexNum; ++x)
			{
				// 1중배열 방식의 인덱스 번호 셋팅
				int nIndex = z * m_nVertexNum + x;
				float y = 0.0f;

				D3DXVECTOR3 n;
				ComputeNormal(&n, &m_vecPNTVertex[nIndex + 0].p, &m_vecPNTVertex[nIndex + 1].p, &m_vecPNTVertex[nIndex + 2].p);

				// 캐릭터의 위치에 따른 y 값 반환을 위해서 버텍스 정보만 따로 빼서 보관한다.
				m_vecVertex[nIndex] = temp[nIndex];

				m_vecPNTVertex[nIndex].p = temp[nIndex];
				m_vecPNTVertex[nIndex].n = n;
				m_vecPNTVertex[nIndex].t = D3DXVECTOR2(x, z);

				if (m_matWorld)
					D3DXVec3TransformCoord(&m_vecVertex[nIndex], &m_vecVertex[nIndex], &m_matWorld);
			}
		}
	}
	else if (temp.size() > 0 && Discriminant != (temp.size() - 1))
	{
		char str[300];
		wsprintf(str, "그리드 사이즈가 일치하지 않아 적용되지 않습니다.");
		MessageBox(g_hWnd, str, "파일 불러오기 실패", MB_OK);
	}
}

void cTerrain::Save()
{
	//저장(txtSaveD3DXVECTOR3()함수 만들기전)
	/*//세이브를 위해 스트링변환과정
	vector<string> tempVertexY;
	for (int i = 0; i < m_vecVertex.size(); i++)
	{
	tempVertexY.push_back(to_string(m_vecVertex[i].y) + "\n");
	}
	g_pTXTDATAManager->txtSaveDialog(tempVertexY);*/

	vector<D3DXVECTOR3> temp;
	for (int i = 0; i < m_vecVertex.size(); i++)
		temp.push_back(m_vecPNTVertex[i].p);

	g_pTXTDATAManager->txtSaveD3DXVECTOR3(temp);

	char str[300];
	sprintf(str, "%d 그리드 사이즈로 적용되었습니다.", (int)sqrtf(m_vecVertex.size() - 1));
	MessageBox(g_hWnd, str, "파일 저장", MB_OK);
}

void cTerrain::FindXIndex()
{
	if (g_pKeyManager->isOnceKeyDown(VK_PRIOR))
	{
		m_XIndex++;
	}
	else if (g_pKeyManager->isOnceKeyDown(VK_NEXT))
	{
		m_XIndex--;
	}

	if (m_XIndex < 0)
		m_XIndex = m_vecXLoader.size() - 1;
	else if (m_XIndex > m_vecXLoader.size() - 1)
		m_XIndex = 0;

	int num = m_XIndex;
	num = m_XIndex;
}





////////////////////////////////////////////////////////////////////////////////////////////////////
//너무 길어져서 구분하기 위해서 주석처리함.
//이곳부터는 모든 데이터를 세이브 하고 로드 하는 함수
////////////////////////////////////////////////////////////////////////////////////////////////////

void cTerrain::LoadAll()
{
	FILE* fp;

	//버텍스 갯수 로드
	fp = fopen("./save/VertexNum", "r");
	char Temp[1024]; //담을 임시 저장소.
	fgets(Temp, sizeof(Temp), fp);
	int nVertexNum = atoi(Temp);
	fclose(fp);

	//PNT 로드
	vector<D3DXVECTOR3> PNTtemp;
	PNTtemp.clear();
	fp = fopen("./save/PNTVertex", "rb");
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

				if (m_matWorld)
					D3DXVec3TransformCoord(&m_vecVertex[nIndex], &m_vecVertex[nIndex], &m_matWorld);
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
	fp = fopen("./save/TextureName", "r");
	//fread(&i, sizeof(string), 1, fp);
	fgets(Temp, sizeof(Temp), fp);
	tName = Temp; //char배열을 그냥 담으면 뒤에 널값들 때문에 string에 담아준다.(string은 알아서 계산해주기 때문에)
	fclose(fp);
	g_pTextureManager->AddTexture(tName, tName, &m_pTexture);
	m_pTexture = g_pTextureManager->GetTexture(tName);
}

void cTerrain::SaveAll()
{
	//파일 포인터
	FILE *fp;

	//버텍스 갯수 저장
	fp = fopen("./save/VertexNum", "w");
	char VertexNum_ch[1024];
	itoa(m_nVertexNum, VertexNum_ch, 10);
	fputs(VertexNum_ch, fp);
	fclose(fp);

	//PNT 저장
	vector<D3DXVECTOR3> PNTtemp;
	for (int i = 0; i < m_vecVertex.size(); i++)
		PNTtemp.push_back(m_vecPNTVertex[i].p);
	fp = fopen("./save/PNTVertex", "wb");
	fwrite(&PNTtemp[0], PNTtemp.size() * sizeof(D3DXVECTOR3), 1, fp); //중요!! 사이즈 * sizeof(타입)
	fclose(fp);

	//텍스쳐 저장
	fp = fopen("./save/TextureName", "w");
	//fwrite(&m_TextureName, m_TextureName.size() * sizeof(string), 1, fp); //중요!! 사이즈 * sizeof(타입)
	fputs(m_TextureName.c_str(), fp);
	fclose(fp);

	//맨 마지막에 그리드가 몇개로 저장되었는지 확인용
	char Gridstr[300];
	sprintf(Gridstr, "%d 그리드 사이즈로 적용되었습니다.", (int)sqrtf(PNTtemp.size() - 1));
	MessageBox(g_hWnd, Gridstr, "파일 저장", MB_OK);
}
