#include "stdafx.h"
#include "cCharacterController.h"
#include "cHeightMap.h"
#include "cAStar.h"
#include "cCube.h"


cCharacterController::cCharacterController()
	: m_vPosition(0, 0, 0)
	, m_fMoveSpeed(0.3f)
	, m_isMoving(false)
	, m_vDest(0, 0, 0)
	, m_vDir(0, 0, 1)
	, m_pAStar(NULL)
	, m_isNeededAStar(false)
	, m_isDebugMode(false)
{
	D3DXMatrixIdentity(&m_matRotY);
}


cCharacterController::~cCharacterController()
{
}

void cCharacterController::Setup()
{
	D3DXMATRIXA16 matS, matT, matW;

	// 장애물 생성 (20개)
	for (int i = 0; i < 20; ++i)
	{
		// 높이는 30으로 고정하고 가로, 세로 크기를 랜덤으로 만든다.
		D3DXMatrixScaling(&matS, rand() % 10 + 3.0f, 30.0f, rand() % 10 + 3.0f);
		// 위치는 맵 영역의 10 사이즈만큼 안쪽으로 랜덤으로 생성 한다.
		D3DXMatrixTranslation(&matT, rand() % 237 + 10.0f, 0.0f, rand() % 237 + 10.0f);
		matW = matS * matT;

		cCube* pCube = new cCube;
		pCube->Setup(&matW, NULL, NULL, NULL);

		float y;	// 높이맵에 따른 높이 값으로 장애물의 높이를 맞춘다. (중점 기준)
		m_pMap->GetHeight(pCube->GetPCVertex()[0].p.x, y, pCube->GetPCVertex()[0].p.z);

		for (int j = 0; j < pCube->GetPCVertex().size(); ++j)
		{
			pCube->GetPCVertex()[j].p.y += y;
		}

		for (int j = 0; j < pCube->GetPCVertex().size(); ++j)
		{
			m_vecObstcles.push_back(pCube->GetPCVertex()[j].p);
		}
		SAFE_RELEASE(pCube);
	}

	m_pAStar->SetObstacles(m_vecObstcles);

	// 하이트맵에 대한 평면 정보를 간소화 시켜서 저장한다.
	// 간소화 시킨 평면의 갯수가 작을 수록 빠르지만 실제 픽킹 위치에서의 오차가 커진다.
	float nSimpleX = 32;
	float nSimpleZ = 32;
	for (int z = 0; z < nSimpleX; ++z)
	{
		for (int x = 0; x < nSimpleZ; ++x)
		{
			// 간소화 시키는 평면의 갯수 만큼의 간격으로 인덱스 번호를 찾아서 평면 정보를 구성한다.
			m_vecTriGrid.push_back(m_vecVertex[(x + 0) / nSimpleX * TILE_N + (z + 0) / nSimpleZ * TILE_N * VERTEX_DIM]);
			m_vecTriGrid.push_back(m_vecVertex[(x + 0) / nSimpleX * TILE_N + (z + 1) / nSimpleZ * TILE_N * VERTEX_DIM]);
			m_vecTriGrid.push_back(m_vecVertex[(x + 1) / nSimpleX * TILE_N + (z + 0) / nSimpleZ * TILE_N * VERTEX_DIM]);

			m_vecTriGrid.push_back(m_vecVertex[(x + 1) / nSimpleX * TILE_N + (z + 0) / nSimpleZ * TILE_N * VERTEX_DIM]);
			m_vecTriGrid.push_back(m_vecVertex[(x + 0) / nSimpleX * TILE_N + (z + 1) / nSimpleZ * TILE_N * VERTEX_DIM]);
			m_vecTriGrid.push_back(m_vecVertex[(x + 1) / nSimpleX * TILE_N + (z + 1) / nSimpleZ * TILE_N * VERTEX_DIM]);
		}
	}
}

void cCharacterController::Update()
{
	static float dist = 0;
	// 목적지의 현재 위치에서의 방향과 거리를 구한다.
	if (!m_isMoving && !m_vecAStarDest.empty())
	{
		m_vDest = m_vecAStarDest.back();
		m_vecAStarDest.pop_back();

		D3DXVECTOR3 vDir;
		m_vDest.y = m_vPosition.y;
		vDir = m_vDest - m_vPosition;
		dist = D3DXVec3Length(&vDir);			// 거리

		if (dist > 0)
		{
			D3DXVec3Normalize(&m_vDir, &vDir);	// 방향
			m_isMoving = true;
		}
	}


	if (dist > 0 && dist > m_fMoveSpeed)
	{
		m_vPosition += m_vDir * m_fMoveSpeed;
		dist -= m_fMoveSpeed;
	}
	else
	{
		m_vPosition = m_vDest;
		m_isMoving = false;
	}

	if (m_pMap)
		m_pMap->GetHeight(m_vPosition.x, m_vPosition.y, m_vPosition.z);

	D3DXMATRIXA16 matT;
	// x, y, z 값으로 이동 매트릭스를 생성해주는 함수
	D3DXMatrixTranslation(&matT, m_vPosition.x, m_vPosition.y, m_vPosition.z);

	// 픽킹에 의한 방향으로 캐릭터를 회전 시킨다.
	// 캐릭터의 바로보는 방향에 대한 뷰매트릭스를 만든다.
	D3DXMatrixLookAtLH(&m_matRotY, &D3DXVECTOR3(0, 0, 0), &m_vDir, &D3DXVECTOR3(0, 1, 0));
	// 회전값만 가지는 매트릭스는 Transpose 를 시키면 Inverse 매트릭스가 된다.
	D3DXMatrixTranspose(&m_matRotY, &m_matRotY);

	// 월드 매트릭스는 꼭 SRT 순서로 곱해서 만든다.
	m_matWorld = m_matRotY * matT;
}

void cCharacterController::Render()
{
	D3DXMATRIXA16 mat;
	D3DXMatrixIdentity(&mat);

	//if (m_pCube)
	//	m_pCube->Render();

	g_pD3DDevice->SetTransform(D3DTS_WORLD, &mat);
	g_pD3DDevice->SetTexture(0, NULL);
	g_pD3DDevice->SetFVF(D3DFVF_XYZ);

	if (m_vecObstcles.size() > 0)
	{

		g_pD3DDevice->SetMaterial(&BLUE_MTRL);
		g_pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_vecObstcles.size() / 3,
			&m_vecObstcles[0], sizeof(D3DXVECTOR3));
	}

	if (GetAsyncKeyState(VK_F1) & 0x0001)
		m_isDebugMode = !m_isDebugMode;

	if (m_isDebugMode)
	{
		g_pD3DDevice->SetMaterial(&YELLOW_MTRL);
		g_pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_vecTriGrid.size() / 3,
			&m_vecTriGrid[0], sizeof(D3DXVECTOR3));
	}
}

void cCharacterController::SetMap(iMap* pMap)
{
	m_pMap = pMap;
	m_vecVertex = ((cHeightMap*)pMap)->GetVertex();
	m_vecIndex = ((cHeightMap*)pMap)->GetIndex();
}

void cCharacterController::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_LBUTTONDOWN:
		{
			cRay ray = cRay::RayAtWorldSpace(LOWORD(lParam), HIWORD(lParam));
			float minF = 100000;
			//for (int i = 0; i < m_vecIndex.size(); i += 3)
			//{
			//	float f;
			//	if (D3DXIntersectTri(&m_vecVertex[m_vecIndex[i + 0]],
			//		&m_vecVertex[m_vecIndex[i + 1]], &m_vecVertex[m_vecIndex[i + 2]],
			//		&ray.m_vOrg, &ray.m_vDir, NULL, NULL, &f))
			//	{
			//		if (f < minF)
			//		{
			//			minF = f;
			//			m_vDest = ray.m_vOrg + ray.m_vDir * f;
			//		}
			//	}
			//}

			for (int i = 0; i < m_vecTriGrid.size(); i += 3)
			{
				float f;
				if (D3DXIntersectTri(&m_vecTriGrid[i + 0],
					&m_vecTriGrid[i + 1], &m_vecTriGrid[i + 2],
					&ray.m_vOrg, &ray.m_vDir, NULL, NULL, &f))
				{
					if (f < minF)
					{
						minF = f;
						m_vDest = ray.m_vOrg + ray.m_vDir * f;
					}
				}
			}

			m_pAStar->ResetNodes();
			m_isNeededAStar = false;

			// 플레이어의 위치에서 목적지로 쏘는 레이를 생성해서
			// 중간에 장애물 유무를 판단한다.
			ray.m_vOrg = m_vPosition;
			ray.m_vDir = m_vDest - m_vPosition;

			// 목적지 보다 먼 곳에 장애물이 있더라도 레이의 방향에 있다면
			// 레이 충돌이 일어나기 때문에 플레어와 장애물 간의 거리를 계산해서
			// 장애물이 플레이어와 목적지 사이에 있는 장애물인지 판별한다.
			float length = D3DXVec3Length(&ray.m_vDir);

			D3DXVec3Normalize(&ray.m_vDir, &ray.m_vDir);
			// 생성 된 레이를 쏴서 장애물과의 충돌 검사
			for (int i = 0; i < m_vecObstcles.size(); i += 3)
			{
				float f;
				if (D3DXIntersectTri(&m_vecObstcles[i], &m_vecObstcles[i + 1],
					&m_vecObstcles[i + 2], &ray.m_vOrg, &ray.m_vDir, NULL, NULL, &f)
					&& f < length)
				{
					m_isNeededAStar = true;
					break;
				}
			}

			if (m_isNeededAStar)
			{
				assert(m_pAStar && "맛스타 없다 이놈아!");

				int nStartIndex = m_pAStar->FindClosestNode(m_vPosition);
				int nEndIndex = m_pAStar->FindClosestNode(m_vDest);

				m_isMoving = false;
				m_vecAStarDest.clear();
				m_vecAStarDest.push_back(m_vDest);
				m_pAStar->FindAStarPath(nStartIndex, nEndIndex, m_vecAStarDest);
				m_pAStar->MakeDirectPath(m_vPosition, m_vDest, m_vecAStarDest);
			}
			else
			{
				m_isMoving = false;
				m_vecAStarDest.clear();
				m_vecAStarDest.push_back(m_vDest);
			}
		}
		break;
	}
}
