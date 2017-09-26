#pragma once
#include "cChildWindowUIButton.h"

class cChildWindowUIobject;

enum 
{
	//버튼
	E_childGridDownBUTTON = 0, E_childGridUpBUTTON,					//그리드 설정 버튼
	E_childGridMinHeightDownBUTTON, E_childGridMinHeightUpBUTTON,	//그리드 높이 설정 버튼
	E_childGridMaxHeightDownBUTTON, E_childGridMaxHeightUpBUTTON,	//그리드 높이 설정 버튼
	E_childGridFlatHeightDownBUTTON, E_childGridFlatHeightUpBUTTON, //그리드 평탄화 설정 버튼
	E_childGridSpeedDownBUTTON, E_childGridSpeedUpBUTTON,			//그리 속도 설정 버튼
	E_childXFileLoadBUTTON,									//X 파일 로드 버튼
	E_childTextureLoadBUTTON,								//텍스쳐 로드 버튼
	E_childLoadBUTTON, E_childSaveBUTTON,					//로드, 세이브 버튼
	E_childModBUTTON,										//모드 변경 버튼
	E_childActivateBUTTON,									//적용 버튼

	//텍스트뷰
	E_childGridTEXTVIEW,									//그리드 텍스트뷰
	E_childGridMinHeightTEXTVIEW,							//그리드 최소 높이 텍스트뷰
	E_childGridMaxHeightTEXTVIEW,							//그리드 최대 높이 텍스트뷰
	E_childGridFlatHeightTEXTVIEW,							//그리드 평탄화 높이 텍스트뷰
	E_childGridSpeedTEXTVIEW,								//그리드 속도 텍스트뷰
	E_childXFileTEXTVIEW,									//X 파일 텍스트뷰
	E_childTextureTEXTVIEW									//텍스쳐 텍스트뷰
};

class cChildTool : public cObject, public iChildWindowUIButtonDelegate
{
private:
	LPD3DXSPRITE				m_pSprite;
	cChildWindowUIobject*		m_pUIRoot;

	//그리드 변경용 변수
	string						m_stringVertexNum;
	int							m_nVertexNum;
	string						m_stringTileNum;
	int							m_nTileNum;
	string						m_stringVertexHeight;
	float						m_fVertexMaxHeight;
	float						m_fVertexMinHeight;
	float						m_fFlatteningHeight;
	float						m_fTerrainSpeed;

	//적용, 로드, 세이브, 텍스쳐, 모드 확인용 변수
	bool						m_isActivate;
	bool						m_isLoad;
	bool						m_isSave;
	bool						m_isXFile;
	bool						m_isTexture;
	bool						m_isMod;

public:
	cChildTool();
	~cChildTool();

	void Setup();
	void Update();
	void Render();
	void WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	// 인터페이스 함수 구현
	virtual void OnClick(cChildWindowUIButton* pSender) override;

	//그리드 조절 함수
	void SelectGrid();

	//그리드 변경용 함수
	void SetstringVertexNum(int nNum) { m_stringVertexNum = to_string(nNum); };
	void SetstringTileNum(int nNum) { m_stringTileNum = to_string(nNum); };
	void SetVertexNum(int nNum) { m_nVertexNum = nNum; }
	int GetVertexNum() { return m_nVertexNum; }
	void SetTileNum(int nNum) { m_nTileNum = nNum; }
	int GetTileNum() { return m_nTileNum; }
	void SetVertexMaxHeight(float height) { m_fVertexMaxHeight = height; }
	float GetVertexMaxHeight() { return m_fVertexMaxHeight; }
	void SetVertexMinHeight(float height) { m_fVertexMinHeight = height; }
	float GetVertexMinHeight() { return m_fVertexMinHeight; }
	void SetFlatteningHeight(float height) { m_fFlatteningHeight = height; }
	float GetFlatteningHeight() { return m_fFlatteningHeight; }
	void SetTerrainSpeed(float speed) { m_fTerrainSpeed = speed; }
	float GetTerrainSpeed() { return m_fTerrainSpeed; }

	//적용, 로드, 세이브 확인용 함수
	void SetIsActivate(bool act) { m_isActivate = act; }
	bool GetIsActivate() { return m_isActivate; }
	void SetIsLoad(bool load) { m_isLoad = load; }
	bool GetIsLoad() { return m_isLoad; }
	void SetIsSave(bool save) { m_isSave = save; }
	bool GetIsSave() { return m_isSave; }
	void SetIsXFile(bool xfile) { m_isXFile = xfile; }
	bool GetIsXFile() { return m_isXFile; }
	void SetIsTexture(bool texture) { m_isTexture = texture; }
	bool GetIsTexture() { return m_isTexture; }
	void SetIsMod(bool mod) { m_isMod = mod; }
	bool GetIsMod() { return m_isMod; }
};

