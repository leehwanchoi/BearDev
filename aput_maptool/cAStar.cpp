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
		// ��尡 ������(���ΰ��� == ���ΰ���)���� ������� ���
		// �������Ʈ(��ü) == �� �� �Ǵ� ���� ���� �̴�.
		int nSqrt = sqrtf(vecPos.size());
		if (i % nSqrt != nSqrt - 1)	// ���� ������ ��尡 �ƴ� ���(x��)
		{
			m_vecNodes[i]->AddEdge(m_vecNodes[i + 1]);
			m_vecNodes[i + 1]->AddEdge(m_vecNodes[i]);
		}

		if (i < vecPos.size() - nSqrt)	// ���� ���� ��尡 �ƴ� ���(z��)
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
	// ���� ��带 ���� ��Ű�� ��忡 ���� �������� ���� �ϰ� �����Ѵ�.
	m_vecNodes[nStart]->S = E_OPEN;
	m_vecNodes[nStart]->G = G;
	m_vecNodes[nStart]->H = H;
	m_vecNodes[nStart]->F = G + H;
	m_vecNodes[nStart]->Via = nStart;
	m_heapOpenNodes->Insert(m_vecNodes[nStart]);

	while (m_vecNodes[nEnd]->S != E_CLOSED)
	{
		int nCurrIndex = GetMinFNode();	// ���� ����߿� F ���� ���� ���� ��带 ã�´�.
		Extend(nCurrIndex, nEnd);		// ã�� ����� �ֺ� ������ �����Ѵ�.
		m_vecNodes[nCurrIndex]->S = E_CLOSED;	// �ֺ� ����� ������ ������ �ش� ���� �ݴ´�.
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
	/* ========== �� ���� �ڷᱸ�� ��� ���� ��� ==========
	// ���� ��� �߿��� F ���� ���� ���� ����� �ε��� ��ȣ�� ���� �� ����
	int nMinFIndex = m_vecOpenNodes[0];	// 0���� F���� ���� �۴ٰ� �����ϰ� ����
	// ���� ���� F �� ���� ����
	float fMinF = m_vecNodes[m_vecOpenNodes[0]]->F;	// 0���� F�� ����
	// ���� �Ǿ� ���� ��忡�� ���� �� ���³���� �ε��� ��ȣ(Ŭ��¡ �� ��)
	int nClosingIndex = 0;	// 0������ �����ϰ� ����

	for (int i = 1; i < m_vecOpenNodes.size(); ++i)
	{
		// ���³���� i ��°�� �ε�����ȣ ��尡 F���� �� ���� ���
		if (fMinF > m_vecNodes[m_vecOpenNodes[i]]->F)
		{
			// �ּ�F ���� �ε��� ��ȣ���� ����
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

		// Ȯ�� ����� ������ ���µ� ��� ��� + Ȯ�� ��� ��忡�� ���������� ���µ� ��� ���
		float G = m_vecNodes[nCenter]->G + vecEdges[i]->fEdgeCost;

		// ���� �Ϸ��� ��尡 ���� ������ ���
		if (m_vecNodes[nIndex]->S == E_OPEN &&
			m_vecNodes[nIndex]->F > G + m_vecNodes[nIndex]->H)
		{
			m_vecNodes[nIndex]->G = G;
			m_vecNodes[nIndex]->F = G + m_vecNodes[nIndex]->H;
			m_vecNodes[nIndex]->Via = nCenter;
		}
		else // �����Ϸ��� ��尡 ù ������ ���
		{
			// F ���� ����ư ���Ͻ��� ���Ѵ�.
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
	int nCutNodeNum = 0;		// ���� �� ��� ����
	cRay ray;

	// ��� �������� �ٷ� �� �� �ִ� ��带 ã�´�. (������ ������ �������� �˻�)
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
				// �浹�� ���� ���
				bCollision = true;
				break;
			}
		}

		if (!bCollision) // �浹�� ���� ��� �� �� ��ų ��带 ����ϰ� ������ ������.
		{
			nCutNodeNum = vecAStarDest.size() - i - 1;
			break;
		}
	}

	// ������ ��带 �����.
	for (int i = 0; i < nCutNodeNum; ++i)
		vecAStarDest.pop_back();

	// ���� �������� �ٷ� �� �� �ִ� ��带 ã�´�. (ó�� ������ �˻�)
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
				// �浹�� ���� ���
				bCollision = true;
				break;
			}
		}

		if (!bCollision) // �浹�� ���� ��� �� �� ��ų ��带 ����ϰ� ������ ������.
		{
			nCutNodeNum = vecAStarDest.size() - i - 2;
			break;
		}
	}

	// ������ ��带 �����.
	for (int i = 0; i < nCutNodeNum; ++i)
		vecAStarDest.erase(vecAStarDest.begin() + 1);
}