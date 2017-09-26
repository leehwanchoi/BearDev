#pragma once

//#define VERTEX_DIM (257)
#define VERTEX_DIM (128 + 1) //fast쓸땐 +1 //기본 64 fast 64 + 1
#define TILE_N (VERTEX_DIM - 1)

class cXobjectLoader;

class cTerrain : public cObject
{
private:
	D3DXMATRIXA16			m_matWorld;

	//수정용 Vertex
	SYNTHESIZE_PASS_BY_REF(vector<D3DXVECTOR3>, m_vecVertex, Vertex);
	//최종 완성본 PNT
	SYNTHESIZE_PASS_BY_REF(vector<ST_PNT_VERTEX>, m_vecPNTVertex, PNTVertex);
	SYNTHESIZE_PASS_BY_REF(vector<DWORD>, m_vecIndex, Index);

	// 정상적인 출력 이후에 사이즈 변경을 시도 한다.
	float					m_fSizeX;
	float					m_fSizeZ;

	cRay					m_CameraRay;
	bool					m_isLButtonDown;	// 마우스 조작에 필요
	bool					m_isPicked;			// 픽킹 확인용

	D3DXVECTOR3				m_vDest;
	DWORD					m_dwDest;
	LPD3DXMESH				m_pSphere;		// 픽킹 마우스 위치 확인용

	//텍스쳐
	LPDIRECT3DTEXTURE9		m_pTexture;
	string					m_TextureName;

	//X File
	vector<cXobjectLoader*>	m_vecXLoader;
	int						m_XIndex;

	//로드 확인용 변수
	bool					m_isLoaded; //세이브 로드용
	bool					m_isMod;	//X오브젝트 모드용

	//그리드 변경용 변수
	int						m_nVertexNum;
	int						m_nTileNum;
	float					m_fVertexMaxHeight;
	float					m_fVertexMinHeight;
	float					m_fFlatteningHeight;
	float					m_fTerrainSpeed;
	bool					m_isFirst;

	//그리드 픽킹 확인용 변수
	bool					m_isfastCheck;

	//최적화 확인용 변수(4/4분기) (4등분)
	int					m_fastSearch[9];
	vector<D3DXVECTOR3> m_fastVertex;

	//확인용 변수 값을 나눠서 담을 변수(4/4분기에서 4/4분기로 나눔) (16등분)
	int					m_fastSearchHalf01[9];
	vector<D3DXVECTOR3> m_fastVertexHalf01;

	int					m_fastSearchHalf02[9];
	vector<D3DXVECTOR3> m_fastVertexHalf02;

	int					m_fastSearchHalf03[9];
	vector<D3DXVECTOR3> m_fastVertexHalf03;

	int					m_fastSearchHalf04[9];
	vector<D3DXVECTOR3> m_fastVertexHalf04;
	
	//실제 찾을 서치 인덱스 (16등분)
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

	//최적화 함수
	void SliceQuarterSetup();
	void SliceQuarterCalculation();

	//마우스 좌표 갱신 함수
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

	//터레인 생성 속도 값 얻어오는 함수
	float GetTerrainSpeed() { return m_fTerrainSpeed; }
	void SetTerrainSpeed(float speed) { m_fTerrainSpeed = speed; }

	//텍스쳐 설정
	void SetTexture(LPDIRECT3DTEXTURE9 pTexture) { m_pTexture = pTexture; }
	LPDIRECT3DTEXTURE9* GetTexture() { return &m_pTexture; }
	void SetTextureName(char* str) { m_TextureName = str; }

	//세이브 로드 함수 (기존에 쓰던것)
	void Load();
	void Save();

	//세이브 로드 함수 (전송하기 위해 새로 만든것)
	void LoadAll();
	void SaveAll();

	//X파일 모드 확인용
	void SetIsMod(bool mod) { m_isMod = mod; }
	bool GetIsMod() { return m_isMod; }

	//X파일
	vector<cXobjectLoader*>& GetXLoader() { return m_vecXLoader; } //실제값을 줘야 push & pop을 할수 있다.(주소를 주면 생성은 되지만, 실제값이 아니기때문에 값을 건들수 없다.)
	int GetXIndex() { return m_XIndex; }
	void FindXIndex();

	//그리드 픽킹 확인용 함수
	bool GetIsfastCheck() { return m_isfastCheck; }
	void SetIsfastCheck(bool fc) { m_isfastCheck = fc; }
};

