#pragma once
#include "iMap.h"

class cObjMap : public iMap
{
private:
	vector<D3DXVECTOR3>		m_vecVertex;
	//vector<DWORD>			m_vecIndex;

	//SYNTHESIZE_PASS_BY_REF(vector<D3DXVECTOR3>, m_vecVertex, Vertex);
	//SYNTHESIZE_PASS_BY_REF(vector<DWORD>, m_vecIndex, Index);	//추가

public:
	cObjMap();
	~cObjMap();

	virtual void Load(char* szFilePath, D3DXMATRIXA16* pMat) override;
	virtual bool GetHeight(IN const float x, OUT float& y, IN const float z) override;
	void Render();

	//Get 함수 추가
	//virtual vector<D3DXVECTOR3>& GetVertex(){ return m_vecVertex; }
	//virtual vector<DWORD>& GetIndex(){ return m_vecIndex; }

	//Set 함수 추가
	//virtual void SetVertex(vector<D3DXVECTOR3>& vecVertex){ m_vecVertex = vecVertex; }
	//virtual void SetIndex(vector<DWORD>& vecIndex){ m_vecIndex = vecIndex; }
};

