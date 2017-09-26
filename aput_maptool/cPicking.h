#pragma once
class cPicking : public cObject
{
private:
	vector<ST_SPHERE>		m_vecSphere;
	LPD3DXMESH				m_pSphere;

	vector<ST_PC_VERTEX>	m_vecPCVertex;
	LPD3DXMESH				m_pBigSphere;
	SYNTHESIZE_PASS_BY_REF(D3DXVECTOR3, m_vPickedPosition, PickedPosition);

	SYNTHESIZE_PASS_BY_REF(vector<D3DXVECTOR3>, m_vecVertex, Vertex);
	SYNTHESIZE_PASS_BY_REF(vector<DWORD>, m_vecIndex, Index);
	
public:
	cPicking();
	~cPicking();

	void Setup();
	void Render();
	void WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

