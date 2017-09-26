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

	// ��ֹ� ���� (20��)
	for (int i = 0; i < 20; ++i)
	{
		// ���̴� 30���� �����ϰ� ����, ���� ũ�⸦ �������� �����.
		D3DXMatrixScaling(&matS, rand() % 10 + 3.0f, 30.0f, rand() % 10 + 3.0f);
		// ��ġ�� �� ������ 10 �����ŭ �������� �������� ���� �Ѵ�.
		D3DXMatrixTranslation(&matT, rand() % 237 + 10.0f, 0.0f, rand() % 237 + 10.0f);
		matW = matS * matT;

		cCube* pCube = new cCube;
		pCube->Setup(&matW, NULL, NULL, NULL);

		float y;	// ���̸ʿ� ���� ���� ������ ��ֹ��� ���̸� �����. (���� ����)
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

	// ����Ʈ�ʿ� ���� ��� ������ ����ȭ ���Ѽ� �����Ѵ�.
	// ����ȭ ��Ų ����� ������ ���� ���� �������� ���� ��ŷ ��ġ������ ������ Ŀ����.
	float nSimpleX = 32;
	float nSimpleZ = 32;
	for (int z = 0; z < nSimpleX; ++z)
	{
		for (int x = 0; x < nSimpleZ; ++x)
		{
			// ����ȭ ��Ű�� ����� ���� ��ŭ�� �������� �ε��� ��ȣ�� ã�Ƽ� ��� ������ �����Ѵ�.
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
	// �������� ���� ��ġ������ ����� �Ÿ��� ���Ѵ�.
	if (!m_isMoving && !m_vecAStarDest.empty())
	{
		m_vDest = m_vecAStarDest.back();
		m_vecAStarDest.pop_back();

		D3DXVECTOR3 vDir;
		m_vDest.y = m_vPosition.y;
		vDir = m_vDest - m_vPosition;
		dist = D3DXVec3Length(&vDir);			// �Ÿ�

		if (dist > 0)
		{
			D3DXVec3Normalize(&m_vDir, &vDir);	// ����
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
	// x, y, z ������ �̵� ��Ʈ������ �������ִ� �Լ�
	D3DXMatrixTranslation(&matT, m_vPosition.x, m_vPosition.y, m_vPosition.z);

	// ��ŷ�� ���� �������� ĳ���͸� ȸ�� ��Ų��.
	// ĳ������ �ٷκ��� ���⿡ ���� ���Ʈ������ �����.
	D3DXMatrixLookAtLH(&m_matRotY, &D3DXVECTOR3(0, 0, 0), &m_vDir, &D3DXVECTOR3(0, 1, 0));
	// ȸ������ ������ ��Ʈ������ Transpose �� ��Ű�� Inverse ��Ʈ������ �ȴ�.
	D3DXMatrixTranspose(&m_matRotY, &m_matRotY);

	// ���� ��Ʈ������ �� SRT ������ ���ؼ� �����.
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

			// �÷��̾��� ��ġ���� �������� ��� ���̸� �����ؼ�
			// �߰��� ��ֹ� ������ �Ǵ��Ѵ�.
			ray.m_vOrg = m_vPosition;
			ray.m_vDir = m_vDest - m_vPosition;

			// ������ ���� �� ���� ��ֹ��� �ִ��� ������ ���⿡ �ִٸ�
			// ���� �浹�� �Ͼ�� ������ �÷���� ��ֹ� ���� �Ÿ��� ����ؼ�
			// ��ֹ��� �÷��̾�� ������ ���̿� �ִ� ��ֹ����� �Ǻ��Ѵ�.
			float length = D3DXVec3Length(&ray.m_vDir);

			D3DXVec3Normalize(&ray.m_vDir, &ray.m_vDir);
			// ���� �� ���̸� ���� ��ֹ����� �浹 �˻�
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
				assert(m_pAStar && "����Ÿ ���� �̳��!");

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
