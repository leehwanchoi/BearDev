#pragma once
#include "iMap.h"

#define NODE_NUM 5 //25
#define NODE_INTERVAL 9

class cNode;
class cHeap;

class cAStar : public cObject
{
private:
	vector<cNode*>		m_vecNodes;			// 모든 노드에 대한 정보를 담을 벡터
	LPD3DXMESH			m_pNodeSphere;		// 테스트 용 렌더를 위한 구 메쉬
	cHeap*				m_heapOpenNodes;	// 오픈 노드의 인덱스 목록
	vector<D3DXVECTOR3>	m_vecObstcles;

public:
	cAStar();
	~cAStar();

	void Setup(iMap* pMap);		// 임의의 노드를 셋팅 한다. (맵정보를 사용하여 높이 설정)
	void DebugRender();			// 노드와 엣지를 그린다.

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
// G: Cost - 출발 지점에서 현재 노드까지 도달하는데 드는 비용
// H: Heuristic(예측값) - 현재 노드에서 목적지 노드까지 도달하는데 드는 비용
//						스퀘어루트 연산을 사용하지 않기 위해서 멘하탄 디스턴스로 계산한다.
// F: G + H - 최적의 순서로 갱신 노드를 선택하기 위한 비용
// V: Via - 현재 노드로 오는 최적 루트의 앞 노드 정보
// S: 노드의 상태 정보(NONE, OPEN, CLOSED)
//		NONE - 한번도 갱신이 이루어 지지 않은 노드
//		OPEN - 최소 한번 이상 갱신이 된 노드
//		CLOSED - 더 이상 갱신을 할 수 없는 노드(이미 최적으로 설정 됨)

// 1. 시작 지점에서 가장 가까운노드, 도착 지점에서 가장 가까운 노드를 찾는다.
// 2. 시작 지점을 오픈 시키고 정보들을 갱신한다.
// == 반복 ==
// 3.1 오픈 된 노드 중에서 F 값이 가장 노드를 찾아서 클로즈 시키고
// 3.2 그 노드의 엣지 노드들의 정보를 갱신한다.(정보가 갱신 된 노드는 오픈 노드가 된다.)
// 3.3 도착 노드가 클로즈 됐다면 반복 정지
// ==========
// 4. 도착 노드부터 Via 노드를 따라가면서 최단 경로를 만든다.