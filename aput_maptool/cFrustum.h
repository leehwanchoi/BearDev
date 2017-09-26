#pragma once
class cFrustum : public cObject
{
private:
	vector<D3DXVECTOR3>		m_vecOriProjVertex;
	vector<D3DXPLANE>		m_vecPlane;

public:
	cFrustum();
	~cFrustum();

	void Setup();
	void Update();
	bool IsInFrustum(ST_SPHERE* pSphere);
};

