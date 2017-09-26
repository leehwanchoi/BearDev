#pragma once

class cNode;

class cHeap
{
private:
	vector<cNode*> m_vecHeap;

public:
	cHeap();
	~cHeap();

	void Insert(cNode* pNode);		// �� ��� �߰�
	void UpdateUpper(int nIndex);	// �� ��� �߰��� �θ� Ÿ�� �ö󰡸鼭 ������Ʈ

	cNode* DeleteRoot();			// ��Ʈ ��带 ��ȯ �Ͽ� ���
	void UpdateLower();				// ��Ʈ ��带 ����ϰ� ���� �˰����� ���� ������

	void Swap(int n1, int n2);
	void Clear();
};

