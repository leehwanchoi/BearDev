#pragma once

class cCube : public cObject
{
private:
	D3DXMATRIXA16			m_matWorld;
	D3DXVECTOR3				m_vPosition;

	vector<ST_PC_VERTEX>	m_vecPCVertex;	// 칼라 값을 가지는 버텍스 정보

	vector<ST_PN_VERTEX>	m_vecPNVertex;	// 노말 값을 가지는 버텍스 정보
	D3DMATERIAL9			m_stMtrl;

	vector<ST_PT_VERTEX>	m_vecPTVertex;	// 텍스쳐 값을 가지는 버텍스 정보
	LPDIRECT3DTEXTURE9		m_pTexture;

	vector<ST_PNT_VERTEX>	m_vecPNTVertex;

	vector<cCube*>			m_vecChild;

	D3DXVECTOR3				m_vStdRotPosition;

	float					m_fRotX;
	float					m_fRotXSpeed;
	float					m_fRotY;
	float					m_fRotYSpeed;

	vector<D3DXVECTOR3>		m_vecVertex;
	vector<WORD>			m_vecIndex;
	IDirect3DVertexBuffer9*	m_vb;
	IDirect3DIndexBuffer9*	m_ib;

public:
	cCube();
	~cCube();

	void Setup(D3DXMATRIXA16* pMat, vector<D3DXVECTOR2>* vecTexture,
		LPDIRECT3DTEXTURE9 pTexture, const D3DMATERIAL9* stMtrl);
	void SetupPCVertex(vector<D3DXVECTOR3>& vecVertex);
	void SetupPNVertex(vector<D3DXVECTOR3>& vecVertex);
	void SetupPTVertex(vector<D3DXVECTOR3>& vecVertex, vector<D3DXVECTOR2>& vecTexture);
	void SetupPNTVertex(vector<D3DXVECTOR3>& vecVertex, vector<D3DXVECTOR2>& vecTexture);
	void SetupD3DVertex();

	void Update(bool isMoving, D3DXMATRIXA16* pParent = NULL);
	void Render();

	void AddChild(cCube* pCube);

	D3DXVECTOR3& GetPosition() { return m_vPosition; }
	void SetPosition(D3DXVECTOR3 pos) { m_vPosition = pos; }
	void SetStdRotPosition(D3DXVECTOR3 pos) { m_vStdRotPosition = pos; }
	void SetRotXSpeed(float f) { m_fRotXSpeed = f; }
	void SetRotYSpeed(float f) { m_fRotYSpeed = f; }

	void Collision(D3DXVECTOR3 target);

	float GetRotY() { return m_fRotY; }
	vector<ST_PC_VERTEX>& GetPCVertex() { return m_vecPCVertex; }
};