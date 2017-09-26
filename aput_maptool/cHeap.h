#pragma once

class cNode;

class cHeap
{
private:
	vector<cNode*> m_vecHeap;

public:
	cHeap();
	~cHeap();

	void Insert(cNode* pNode);		// 새 노드 추가
	void UpdateUpper(int nIndex);	// 새 노드 추가후 부모를 타고 올라가면서 업데이트

	cNode* DeleteRoot();			// 루트 노드를 반환 하여 사용
	void UpdateLower();				// 루트 노드를 사용하고 삭제 알고리즘을 통한 재정렬

	void Swap(int n1, int n2);
	void Clear();
};

