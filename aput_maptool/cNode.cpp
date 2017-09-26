#include "stdafx.h"
#include "cNode.h"


cNode::cNode(D3DXVECTOR3 vLoc)
	: F(0)
	, G(0)
	, H(0)
	, S(E_NONE)
	, Via(NULL)
{
	m_vLocation = vLoc;
	m_vecEdges.clear();
	m_vecLines.clear();
}


cNode::~cNode()
{
}

void cNode::AddEdge(cNode* pNode)
{
	ST_EdgeInfo* stEdge = new ST_EdgeInfo;
	stEdge->pNode = pNode;

	D3DXVECTOR3 v = this->m_vLocation - pNode->m_vLocation;
	stEdge->fEdgeCost = D3DXVec3Length(&v);

	m_vecEdges.push_back(stEdge);

	m_vecLines.push_back(this->m_vLocation);
	m_vecLines.push_back(pNode->m_vLocation);
}
