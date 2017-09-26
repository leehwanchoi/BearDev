#pragma once
struct SKYVERTEX
{
	float x, y, z, u, v;
};
#define D3DFVF_SKYVERTEX (D3DFVF_XYZ | D3DFVF_TEX1 )
class cSkyBox :public cObject
{
private:
	LPDIRECT3DVERTEXBUFFER9		m_pVB;
	LPDIRECT3DTEXTURE9			m_pTex[6];
public:
	cSkyBox();
	~cSkyBox();

	void CreateSkyVertex();
	void init();
	//void update();
	void render();
	void SetupLight();
};

