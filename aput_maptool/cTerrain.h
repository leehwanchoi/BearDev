#pragma once

//#define VERTEX_DIM (257)
#define VERTEX_DIM (128 + 1) //fast���� +1 //�⺻ 64 fast 64 + 1
#define TILE_N (VERTEX_DIM - 1)

class cXobjectLoader;

class cTerrain : public cObject
{
private:
	D3DXMATRIXA16			m_matWorld;

	//������ Vertex
	SYNTHESIZE_PASS_BY_REF(vector<D3DXVECTOR3>, m_vecVertex, Vertex);
	//���� �ϼ��� PNT
	SYNTHESIZE_PASS_BY_REF(vector<ST_PNT_VERTEX>, m_vecPNTVertex, PNTVertex);
	SYNTHESIZE_PASS_BY_REF(vector<DWORD>, m_vecIndex, Index);

	// �������� ��� ���Ŀ� ������ ������ �õ� �Ѵ�.
	float					m_fSizeX;
	float					m_fSizeZ;

	cRay					m_CameraRay;
	bool					m_isLButtonDown;	// ���콺 ���ۿ� �ʿ�
	bool					m_isPicked;			// ��ŷ Ȯ�ο�

	D3DXVECTOR3				m_vDest;
	DWORD					m_dwDest;
	LPD3DXMESH				m_pSphere;		// ��ŷ ���콺 ��ġ Ȯ�ο�

	//�ؽ���
	LPDIRECT3DTEXTURE9		m_pTexture;
	string					m_TextureName;

	//X File
	vector<cXobjectLoader*>	m_vecXLoader;
	int						m_XIndex;

	//�ε� Ȯ�ο� ����
	bool					m_isLoaded; //���̺� �ε��
	bool					m_isMod;	//X������Ʈ ����

	//�׸��� ����� ����
	int						m_nVertexNum;
	int						m_nTileNum;
	float					m_fVertexMaxHeight;
	float					m_fVertexMinHeight;
	float					m_fFlatteningHeight;
	float					m_fTerrainSpeed;
	bool					m_isFirst;

	//�׸��� ��ŷ Ȯ�ο� ����
	bool					m_isfastCheck;

	//����ȭ Ȯ�ο� ����(4/4�б�) (4���)
	int					m_fastSearch[9];
	vector<D3DXVECTOR3> m_fastVertex;

	//Ȯ�ο� ���� ���� ������ ���� ����(4/4�б⿡�� 4/4�б�� ����) (16���)
	int					m_fastSearchHalf01[9];
	vector<D3DXVECTOR3> m_fastVertexHalf01;

	int					m_fastSearchHalf02[9];
	vector<D3DXVECTOR3> m_fastVertexHalf02;

	int					m_fastSearchHalf03[9];
	vector<D3DXVECTOR3> m_fastVertexHalf03;

	int					m_fastSearchHalf04[9];
	vector<D3DXVECTOR3> m_fastVertexHalf04;
	
	//���� ã�� ��ġ �ε��� (16���)
	vector<DWORD>		m_vecfast01Index;
	vector<DWORD>		m_vecfast02Index;
	vector<DWORD>		m_vecfast03Index;
	vector<DWORD>		m_vecfast04Index;

	vector<DWORD>		m_vecfast05Index;
	vector<DWORD>		m_vecfast06Index;
	vector<DWORD>		m_vecfast07Index;
	vector<DWORD>		m_vecfast08Index;

	vector<DWORD>		m_vecfast09Index;
	vector<DWORD>		m_vecfast10Index;
	vector<DWORD>		m_vecfast11Index;
	vector<DWORD>		m_vecfast12Index;

	vector<DWORD>		m_vecfast13Index;
	vector<DWORD>		m_vecfast14Index;
	vector<DWORD>		m_vecfast15Index;
	vector<DWORD>		m_vecfast16Index;

	//����ȭ �Լ�
	void SliceQuarterSetup();
	void SliceQuarterCalculation();

	//���콺 ��ǥ ���� �Լ�
	void RealtimeMousePositionCalculation();

public:
	cTerrain();
	~cTerrain();

	void Setup();
	void Update();
	void Render();
	void WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	D3DXMATRIXA16 GetWorld() { return m_matWorld; };
	bool GetHeight(IN const float x, OUT float & y, IN const float z);
	void CreateTerrain(DWORD index);

	cRay GetCameraRay() { return m_CameraRay; }
	D3DXVECTOR3 GetVECTOR3Dest() { return m_vDest; }
	DWORD GetDWORDDest() { return m_dwDest; }

	bool GetIsPicked() { return m_isPicked; }
	void SetIsPicked(bool pick) { m_isPicked = pick; }

	void SetvecVertex(vector<D3DXVECTOR3>* vecVertex) { m_vecVertex = *vecVertex; }
	void SetvecIndex(vector<DWORD>* vecIndex) { m_vecIndex = *vecIndex; }

	void SetVertexNum(int nNum) { m_nVertexNum = nNum; }
	void SetTileNum(int nNum) { m_nTileNum = nNum; }
	int GetVertexNum() { return m_nVertexNum; }
	int GetTileNum() { return m_nTileNum; }
	void SetVertexMaxHeight(float height) { m_fVertexMaxHeight = height; }
	void SetVertexMinHeight(float height) { m_fVertexMinHeight = height; }
	void SetFlatteningHeight(float height) { m_fFlatteningHeight = height; }
	float GetVertexMaxHeight() { return m_fVertexMaxHeight; }
	float GetVertexMinHeight() { return m_fVertexMinHeight; }
	float GetFlatteningHeight() { return m_fFlatteningHeight; }

	//�ͷ��� ���� �ӵ� �� ������ �Լ�
	float GetTerrainSpeed() { return m_fTerrainSpeed; }
	void SetTerrainSpeed(float speed) { m_fTerrainSpeed = speed; }

	//�ؽ��� ����
	void SetTexture(LPDIRECT3DTEXTURE9 pTexture) { m_pTexture = pTexture; }
	LPDIRECT3DTEXTURE9* GetTexture() { return &m_pTexture; }
	void SetTextureName(char* str) { m_TextureName = str; }

	//���̺� �ε� �Լ� (������ ������)
	void Load();
	void Save();

	//���̺� �ε� �Լ� (�����ϱ� ���� ���� �����)
	void LoadAll();
	void SaveAll();

	//X���� ��� Ȯ�ο�
	void SetIsMod(bool mod) { m_isMod = mod; }
	bool GetIsMod() { return m_isMod; }

	//X����
	vector<cXobjectLoader*>& GetXLoader() { return m_vecXLoader; } //�������� ��� push & pop�� �Ҽ� �ִ�.(�ּҸ� �ָ� ������ ������, �������� �ƴϱ⶧���� ���� �ǵ�� ����.)
	int GetXIndex() { return m_XIndex; }
	void FindXIndex();

	//�׸��� ��ŷ Ȯ�ο� �Լ�
	bool GetIsfastCheck() { return m_isfastCheck; }
	void SetIsfastCheck(bool fc) { m_isfastCheck = fc; }
};

