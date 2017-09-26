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

	D3DXVECTOR3				m_vLocation;	// ����� ��ġ
	int						m_nIndex;		// ���� ����� �ε��� ��ȣ

	float					F;
	float					G;
	float					H;
	eNodeState				S;
	int						Via;

	vector<ST_EdgeInfo*>	m_vecEdges;
	vector<D3DXVECTOR3>		m_vecLines;

	void AddEdge(cNode* pNode);
};

