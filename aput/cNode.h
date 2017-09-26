#pragma once

class cNode;

struct ST_EdgeInfo
{
	cNode* pNode;
	float fEdgeCost;
};

enum eNodeState { E_NONE, E_OPEN, E_CLOSED, E_USING };

class cNode
{
public:
	cNode(D3DXVECTOR3 vLoc);
	~cNode();

	D3DXVECTOR3				m_vLocation;	// 노드의 위치
	int						m_nIndex;		// 현재 노드의 인덱스 번호

	float					F;
	float					G;
	float					H;
	eNodeState				S;
	int						Via;

	vector<ST_EdgeInfo*>	m_vecEdges;
	vector<D3DXVECTOR3>		m_vecLines;

	void AddEdge(cNode* pNode);
};

