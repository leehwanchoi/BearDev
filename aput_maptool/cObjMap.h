#pragma once
#include "iMap.h"

class cObjMap : public iMap
{
private:
	vector<D3DXVECTOR3> m_vecVertex;

public:
	cObjMap();
	~cObjMap();

	virtual void Load(char* szFilePath, D3DXMATRIXA16* pMat) override;
	virtual bool GetHeight(IN const float x, OUT float& y, IN const float z) override;

	void Render();
};

