#pragma once
#include "iMap.h"

#define NODE_NUM 5 //25
#define NODE_INTERVAL 9

class cNode;
class cHeap;

class cAStar : public cObject
{
private:
	vector<cNode*>		m_vecNodes;			// ��� ��忡 ���� ������ ���� ����
	LPD3DXMESH			m_pNodeSphere;		// �׽�Ʈ �� ������ ���� �� �޽�
	cHeap*				m_heapOpenNodes;	// ���� ����� �ε��� ���
	vector<D3DXVECTOR3>	m_vecObstcles;

public:
	cAStar();
	~cAStar();

	void Setup(iMap* pMap);		// ������ ��带 ���� �Ѵ�. (�������� ����Ͽ� ���� ����)
	void DebugRender();			// ���� ������ �׸���.

	void ResetNodes();
	int FindClosestNode(const D3DXVECTOR3& pos);
	void FindAStarPath(IN int nStart, IN int nEnd, OUT vector<D3DXVECTOR3>& vecDest);
	float GetManhattanDistance(IN int nCurrIndex, IN int nEnd);
	int GetMinFNode();
	void Extend(IN int nCenter, IN int nEnd);

	void SetObstacles(vector<D3DXVECTOR3>& vecObstcles) { m_vecObstcles = vecObstcles; }
	bool IsAbleToUseNode(int nIndex, D3DXVECTOR3 pos);
	void MakeDirectPath(IN D3DXVECTOR3 vStart, IN D3DXVECTOR3 vEnd, OUT vector<D3DXVECTOR3>& vecAStarDest);
};

// NodeInfo
// G: Cost - ��� �������� ���� ������ �����ϴµ� ��� ���
// H: Heuristic(������) - ���� ��忡�� ������ ������ �����ϴµ� ��� ���
//						�������Ʈ ������ ������� �ʱ� ���ؼ� ����ź ���Ͻ��� ����Ѵ�.
// F: G + H - ������ ������ ���� ��带 �����ϱ� ���� ���
// V: Via - ���� ���� ���� ���� ��Ʈ�� �� ��� ����
// S: ����� ���� ����(NONE, OPEN, CLOSED)
//		NONE - �ѹ��� ������ �̷�� ���� ���� ���
//		OPEN - �ּ� �ѹ� �̻� ������ �� ���
//		CLOSED - �� �̻� ������ �� �� ���� ���(�̹� �������� ���� ��)

// 1. ���� �������� ���� �������, ���� �������� ���� ����� ��带 ã�´�.
// 2. ���� ������ ���� ��Ű�� �������� �����Ѵ�.
// == �ݺ� ==
// 3.1 ���� �� ��� �߿��� F ���� ���� ��带 ã�Ƽ� Ŭ���� ��Ű��
// 3.2 �� ����� ���� ������ ������ �����Ѵ�.(������ ���� �� ���� ���� ��尡 �ȴ�.)
// 3.3 ���� ��尡 Ŭ���� �ƴٸ� �ݺ� ����
// ==========
// 4. ���� ������ Via ��带 ���󰡸鼭 �ִ� ��θ� �����.