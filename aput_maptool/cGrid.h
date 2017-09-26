#pragma once

class cPyramid;
class cWall;

class cGrid : public cObject
{
private:
	D3DXMATRIXA16			m_matWorld;
	vector<ST_PC_VERTEX>	m_vecPCVertex;

	vector<cPyramid*>		m_vecPyramid;

	vector<cWall*>			m_vecWall;

public:
	cGrid();
	~cGrid();

	void Setup();
	void Render();
};

