#pragma once
class cAlphaBlendTest : public cObject
{
private:
	D3DXMATRIXA16			m_matWorld;
	vector<ST_PC_VERTEX>	m_vecPCVertex;
	vector<ST_PT_VERTEX>	m_vecPTVertex;
	vector<ST_PNT_VERTEX>	m_vecPNTVertex;

	void SetPCPlane(vector<D3DXVECTOR3>& vecV, D3DXVECTOR3 s, D3DXVECTOR3 p, D3DCOLOR c);
	void SetPTPlane(vector<D3DXVECTOR3>& vecV, D3DXVECTOR3 s, D3DXVECTOR3 p);
	void SetPNTPlane(vector<D3DXVECTOR3>& vecV, D3DXVECTOR3 s, D3DXVECTOR3 p);

public:
	cAlphaBlendTest();
	~cAlphaBlendTest();

	void Setup();
	void Render();
};

