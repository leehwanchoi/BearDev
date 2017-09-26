#include "stdafx.h"
#include "cHeap.h"
#include "cNode.h"


cHeap::cHeap()
{
}


cHeap::~cHeap()
{
}

void cHeap::Insert(cNode* pNode)
{
	m_vecHeap.push_back(pNode);
	UpdateUpper(m_vecHeap.size() - 1);
}

void cHeap::UpdateUpper(int nIndex)
{
	int nCurrIndex = nIndex;
	int nParent = (nCurrIndex - 1) / 2;

	while (nCurrIndex != 0)
	{
		if (m_vecHeap[nParent]->F < m_vecHeap[nCurrIndex]->F)
			break;

		Swap(nCurrIndex, nParent);
		nCurrIndex = nParent;
		nParent = (nCurrIndex - 1) / 2;
	}
}

cNode * cHeap::DeleteRoot()
{
	cNode* pRoot = m_vecHeap.front();
	if (m_vecHeap.size() > 1)
	{
		Swap(0, m_vecHeap.size() - 1);
		m_vecHeap.pop_back();
		UpdateLower();
	}

	return pRoot;
}

void cHeap::UpdateLower()
{
	int nCurrIndex = 0;
	int nLChild = nCurrIndex * 2 + 1;
	int nRChild = nCurrIndex * 2 + 2;
	int nMinNodeIndex = nCurrIndex;

	while (true)
	{
		// 왼쪽 자식이 존재 할 경우 현재의 가장 작은 F 값과 비교해서
		// 더 작은 값을 가장 작은 값으로 설정한다.
		if (nLChild < (int)m_vecHeap.size() - 1 &&
			m_vecHeap[nLChild]->F < m_vecHeap[nMinNodeIndex]->F)
		{
			nMinNodeIndex = nLChild;
		}

		// 오른쪽 자식이 존재 할 경우
		if (nRChild < (int)m_vecHeap.size() - 1 &&
			m_vecHeap[nRChild]->F < m_vecHeap[nMinNodeIndex]->F)
		{
			nMinNodeIndex = nRChild;
		}

		if (nMinNodeIndex == nCurrIndex)
			break;

		Swap(nCurrIndex, nMinNodeIndex);
		nCurrIndex = nMinNodeIndex;
		nLChild = nCurrIndex * 2 + 1;
		nRChild = nCurrIndex * 2 + 2;
	}
}

void cHeap::Swap(int n1, int n2)
{
	cNode* pTemp = m_vecHeap[n1];
	m_vecHeap[n1] = m_vecHeap[n2];
	m_vecHeap[n2] = pTemp;
}

void cHeap::Clear()
{
	m_vecHeap.clear();
}