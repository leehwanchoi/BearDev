#include "stdafx.h"
#include "cAStar.h"
#include "cNode.h"
#include "cHeap.h"


cAStar::cAStar()
	: m_pNodeSphere(NULL)
{
}


cAStar::~cAStar()
{
	SAFE_RELEASE(m_pNodeSphere);
}

void cAStar::Setup(iMap* pMap)
{
	D3DXCreateSphere(g_pD3DDevice, 1.0f, 10, 10, &m_pNodeSphere, NULL);

	vector<D3DXVECTOR3> vecPos;

	for (int z = 0; z < NODE_NUM; ++z)
	{
		for (int x = 0; x < NODE_NUM; ++x)
		{
			vecPos.push_back(D3DXVECTOR3(10 + x * NODE_INTERVAL, 0, 10 + z * NODE_INTERVAL));
		}
	}

	for (int i = 0; i < vecPos.size(); ++i)
	{
		if (pMap->GetHeight(vecPos[i].x, vecPos[i].y, vecPos[i].z))
		{
			cNode* pNode = new cNode(vecPos[i]);
			pNode->m_nIndex = i;
			m_vecNodes.push_back(pNode);
		}
	}

	for (int i = 0; i < vecPos.size(); ++i)
	{
		// 노드가 정방향(가로갯수 == 세로갯수)으로 만들어진 경우
		// 스퀘어루트(전체) == 한 열 또는 행의 갯수 이다.
		int nSqrt = sqrtf(vecPos.size());
		if (i % nSqrt != nSqrt - 1)	// 가장 오른쪽 노드가 아닌 경우(x축)
		{
			m_vecNodes[i]->AddEdge(m_vecNodes[i + 1]);
			m_vecNodes[i + 1]->AddEdge(m_vecNodes[i]);
		}

		if (i < vecPos.size() - nSqrt)	// 가장 윗쪽 노드가 아닌 경우(z축)
		{
			m_vecNodes[i]->AddEdge(m_vecNodes[i + nSqrt]);
			m_vecNodes[i + nSqrt]->AddEdge(m_vecNodes[i]);
		}
	}

	m_heapOpenNodes = new cHeap;
}

void cAStar::DebugRender()
{
	D3DXMATRIXA16 matW;

	g_pD3DDevice->SetTexture(0, NULL);

	for each (auto p in m_vecNodes)
	{
		switch (p->S)
		{
		case E_NONE:
			g_pD3DDevice->SetMaterial(&WHITE_MTRL);
			break;
		case E_OPEN:
			g_pD3DDevice->SetMaterial(&GREEN_MTRL);
			break;
		case E_CLOSED:
			g_pD3DDevice->SetMaterial(&RED_MTRL);
			break;
		case E_USING:
			g_pD3DDevice->SetMaterial(&YELLOW_MTRL);
			break;
		}

		D3DXMatrixTranslation(&matW, p->m_vLocation.x,
			p->m_vLocation.y, p->m_vLocation.z);

		g_pD3DDevice->SetTransform(D3DTS_WORLD, &matW);
		m_pNodeSphere->DrawSubset(0);

		D3DXMatrixIdentity(&matW);
		matW._42 += 1.0f;
		g_pD3DDevice->SetTransform(D3DTS_WORLD, &matW);
		g_pD3DDevice->DrawPrimitiveUP(D3DPT_LINELIST, p->m_vecLines.size() / 2,
			&p->m_vecLines[0], sizeof(D3DXVECTOR3));
	}
}

void cAStar::ResetNodes()
{
	for (int i = 0; i < m_vecNodes.size(); ++i)
		m_vecNodes[i]->S = E_NONE;

	m_heapOpenNodes->Clear();
}

int cAStar::FindClosestNode(const D3DXVECTOR3& pos)
{
	float fDist = 100000;
	int nIndex = -1;

	for (int i = 0; i < m_vecNodes.size(); ++i)
	{
		D3DXVECTOR3 v = pos - m_vecNodes[i]->m_vLocation;
		float length = D3DXVec3Length(&v);
		if (fDist > length && IsAbleToUseNode(i, pos))
		{
			fDist = length;
			nIndex = i;
		}
	}

	return nIndex;
}

void cAStar::FindAStarPath(IN int nStart, IN int nEnd, OUT vector<D3DXVECTOR3>& vecDest)
{
	float G = 0.0f;
	float H = GetManhattanDistance(nStart, nEnd);
	// 시작 노드를 오픈 시키고 노드에 대한 정보들을 갱신 하고 시작한다.
	m_vecNodes[nStart]->S = E_OPEN;
	m_vecNodes[nStart]->G = G;
	m_vecNodes[nStart]->H = H;
	m_vecNodes[nStart]->F = G + H;
	m_vecNodes[nStart]->Via = nStart;
	m_heapOpenNodes->Insert(m_vecNodes[nStart]);

	while (m_vecNodes[nEnd]->S != E_CLOSED)
	{
		int nCurrIndex = GetMinFNode();	// 오픈 노드중에 F 값이 가장 작은 노드를 찾는다.
		Extend(nCurrIndex, nEnd);		// 찾은 노드의 주변 노드들을 갱신한다.
		m_vecNodes[nCurrIndex]->S = E_CLOSED;	// 주변 노드의 갱신이 끝나면 해당 노드는 닫는다.
	}

	int nCurrIndex = nEnd;
	while (nCurrIndex != nStart)
	{
		m_vecNodes[nCurrIndex]->S = E_USING;
		vecDest.push_back(m_vecNodes[nCurrIndex]->m_vLocation);
		nCurrIndex = m_vecNodes[nCurrIndex]->Via;
	}
	m_vecNodes[nCurrIndex]->S = E_USING;
	vecDest.push_back(m_vecNodes[nCurrIndex]->m_vLocation);
}

float cAStar::GetManhattanDistance(IN int nCurrIndex, IN int nEnd)
{
	return fabs(m_vecNodes[nCurrIndex]->m_vLocation.x - m_vecNodes[nEnd]->m_vLocation.x)
		+ fabs(m_vecNodes[nCurrIndex]->m_vLocation.y - m_vecNodes[nEnd]->m_vLocation.y)
		+ fabs(m_vecNodes[nCurrIndex]->m_vLocation.z - m_vecNodes[nEnd]->m_vLocation.z);
}

int cAStar::GetMinFNode()
{
	/* ========== 힙 정렬 자료구조 사용 이전 방식 ==========
	// 오픈 노드 중에서 F 값이 가장 작은 노드의 인덱스 번호를 저장 할 변수
	int nMinFIndex = m_vecOpenNodes[0];	// 0번의 F값이 가장 작다고 가정하고 시작
	// 가장 작은 F 값 저장 변수
	float fMinF = m_vecNodes[m_vecOpenNodes[0]]->F;	// 0번의 F값 저장
	// 선택 되어 오픈 노드에서 제거 될 오픈노드의 인덱스 번호(클로징 될 놈)
	int nClosingIndex = 0;	// 0번으로 가정하고 시작

	for (int i = 1; i < m_vecOpenNodes.size(); ++i)
	{
		// 오픈노드의 i 번째의 인덱스번호 노드가 F값이 더 작을 경우
		if (fMinF > m_vecNodes[m_vecOpenNodes[i]]->F)
		{
			// 최소F 값과 인덱스 번호들을 갱신
			fMinF = m_vecNodes[m_vecOpenNodes[i]]->F;
			nMinFIndex = m_vecOpenNodes[i];
			nClosingIndex = i;
		}
	}

	m_vecOpenNodes.erase(m_vecOpenNodes.begin() + nClosingIndex);

	return nMinFIndex;
	============================================== */

	return m_heapOpenNodes->DeleteRoot()->m_nIndex;
}

void cAStar::Extend(IN int nCenter, IN int nEnd)
{
	vector<ST_EdgeInfo*> vecEdges = m_vecNodes[nCenter]->m_vecEdges;

	for (int i = 0; i < vecEdges.size(); ++i)
	{
		int nIndex = vecEdges[i]->pNode->m_nIndex;

		if (m_vecNodes[nIndex]->S == E_CLOSED) continue;
		if (!IsAbleToUseNode(nIndex, m_vecNodes[nCenter]->m_vLocation)) continue;

		// 확장 대상의 노드까지 오는데 드는 비용 + 확장 대상 노드에서 엣지노드까지 가는데 드는 비용
		float G = m_vecNodes[nCenter]->G + vecEdges[i]->fEdgeCost;

		// 갱신 하려는 노드가 오픈 상태일 경우
		if (m_vecNodes[nIndex]->S == E_OPEN &&
			m_vecNodes[nIndex]->F > G + m_vecNodes[nIndex]->H)
		{
			m_vecNodes[nIndex]->G = G;
			m_vecNodes[nIndex]->F = G + m_vecNodes[nIndex]->H;
			m_vecNodes[nIndex]->Via = nCenter;
		}
		else // 갱신하려는 노드가 첫 갱신일 경우
		{
			// F 값은 맨헤튼 디스턴스로 구한다.
			float H = GetManhattanDistance(nIndex, nEnd);

			m_vecNodes[nIndex]->G = G;
			m_vecNodes[nIndex]->H = H;
			m_vecNodes[nIndex]->F = G + H;
			m_vecNodes[nIndex]->Via = nCenter;
			m_vecNodes[nIndex]->S = E_OPEN;
			m_heapOpenNodes->Insert(m_vecNodes[nIndex]);
		}
	}
}

bool cAStar::IsAbleToUseNode(int nIndex, D3DXVECTOR3 pos)
{
	cRay ray;
	ray.m_vOrg = pos;
	ray.m_vDir = m_vecNodes[nIndex]->m_vLocation - pos;
	float length = D3DXVec3Length(&ray.m_vDir);
	D3DXVec3Normalize(&ray.m_vDir, &ray.m_vDir);

	for (int i = 0; i < m_vecObstcles.size(); i += 3)
	{
		float f;
		if (D3DXIntersectTri(&m_vecObstcles[i], &m_vecObstcles[i + 1],
			&m_vecObstcles[i + 2], &ray.m_vOrg, &ray.m_vDir, NULL, NULL, &f)
			&& f < length)
		{
			return false;
		}
	}

	return true;
}

void cAStar::MakeDirectPath(IN D3DXVECTOR3 vStart, IN D3DXVECTOR3 vEnd, OUT vector<D3DXVECTOR3>& vecAStarDest)
{
	int nCutNodeNum = 0;		// 삭제 할 노드 갯수
	cRay ray;

	// 출발 지점에서 바로 갈 수 있는 노드를 찾는다. (마지막 노드부터 역순으로 검색)
	ray.m_vOrg = vStart;
	for (int i = 0; i < vecAStarDest.size(); ++i)
	{
		ray.m_vDir = vecAStarDest[i] - ray.m_vOrg;
		float fLength = D3DXVec3Length(&ray.m_vDir);
		D3DXVec3Normalize(&ray.m_vDir, &ray.m_vDir);

		bool bCollision = false;
		for (int j = 0; j < m_vecObstcles.size(); j += 3)
		{
			float f;
			if (D3DXIntersectTri(&m_vecObstcles[j + 0], &m_vecObstcles[j + 1],
				&m_vecObstcles[j + 2], &ray.m_vOrg, &ray.m_vDir, NULL, NULL, &f) &&
				f < fLength)
			{
				// 충돌이 있을 경우
				bCollision = true;
				break;
			}
		}

		if (!bCollision) // 충돌이 없을 경우 컷 팅 시킬 노드를 계산하고 컷팅을 끝낸다.
		{
			nCutNodeNum = vecAStarDest.size() - i - 1;
			break;
		}
	}

	// 실제로 노드를 지운다.
	for (int i = 0; i < nCutNodeNum; ++i)
		vecAStarDest.pop_back();

	// 도착 지점에서 바로 갈 수 있는 노드를 찾는다. (처음 노드부터 검색)
	ray.m_vOrg = vEnd;
	for (int i = 0; i < vecAStarDest.size(); ++i)
	{
		ray.m_vDir = vecAStarDest[vecAStarDest.size() - i - 1] - ray.m_vOrg;
		float fLength = D3DXVec3Length(&ray.m_vDir);
		D3DXVec3Normalize(&ray.m_vDir, &ray.m_vDir);

		bool bCollision = false;
		for (int j = 0; j < m_vecObstcles.size(); j += 3)
		{
			float f;
			if (D3DXIntersectTri(&m_vecObstcles[j + 0], &m_vecObstcles[j + 1],
				&m_vecObstcles[j + 2], &ray.m_vOrg, &ray.m_vDir, NULL, NULL, &f) &&
				f < fLength)
			{
				// 충돌이 있을 경우
				bCollision = true;
				break;
			}
		}

		if (!bCollision) // 충돌이 없을 경우 컷 팅 시킬 노드를 계산하고 컷팅을 끝낸다.
		{
			nCutNodeNum = vecAStarDest.size() - i - 2;
			break;
		}
	}

	// 실제로 노드를 지운다.
	for (int i = 0; i < nCutNodeNum; ++i)
		vecAStarDest.erase(vecAStarDest.begin() + 1);
}