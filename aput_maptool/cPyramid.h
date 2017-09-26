#pragma once
class cPyramid : public cObject
{
private:
	D3DXMATRIXA16			m_matWorld;
	vector<ST_PC_VERTEX>	m_vecPCVertex;

public:
	cPyramid();
	~cPyramid();

	void Setup(D3DCOLOR c, D3DXMATRIXA16& matWorld);
	void Render();
};