#pragma once

class cCube;

class cCubeMan : public cObject
{
private:
	cCube*				m_pRoot;
	cCube*				m_pHead;
	LPDIRECT3DTEXTURE9	m_pTexture;

public:
	cCubeMan();
	~cCubeMan();

	void Setup();
	void SetTextureVertex(vector<D3DXVECTOR2>& vecTexture, RECT rcCoord);
	void Update(bool isMoving, D3DXMATRIXA16* mat = NULL);
	void Render();

	float GetHeadRotation();
};

