#pragma once
#include "cUIButton.h"

class cUIObject;
class cXobjectLoader;

enum { E_BUTTON1 = 0, E_BUTTON2, E_TEXTVIEW, E_FolderFileTEXTVIEW};

class cMapToolInUI : public cObject, public iUIButtonDelegate
{
private:
	LPD3DXSPRITE			m_pSprite;
	D3DXIMAGE_INFO			m_stImageInfo;
	cUIObject*				m_pUIRoot;
	
	//X 파일
	vector<D3DXMATRIXA16>	m_vecMatW;
	vector<LPD3DXMESH>		m_vecMesh;
	vector<D3DMATERIAL9>	m_vecMaterial;
	vector<DWORD>			m_vecMtlNum;
	vector<string>			m_vecTextureName;
	
	//X 파일 검색용
	vector<string>			m_vecXName;
	int						m_XIndex;

	//버튼 확인용
	bool					m_IsLClick;
	bool					m_IsRClick;

public:
	cMapToolInUI();
	~cMapToolInUI();

	void Setup();
	void Load(char* lpstrFile);
	void Update(D3DXVECTOR3* pos = NULL);
	void Render();

	// 인터페이스 함수 구현
	virtual void OnClick(cUIButton* pSender) override;

	bool ComputeBoundingBox(ID3DXMesh* mesh, BoundingBox* box);

	//X파일 검색용
	vector<string>& GetvecXName() { return m_vecXName; }
	int GetXIndex() { return m_XIndex; }
	void SetXIndex(int index) { m_XIndex = index; }
	//X파일 추가
	void AddXFile(char* lpstrFile);

	//버튼 확인용
	bool GetIsLClick() { return m_IsLClick; }
	void SetIsLClick(bool lc) { m_IsLClick = lc; }
	bool GetIsRClick() { return m_IsRClick; }
	void SetIsRClick(bool rc) { m_IsRClick = rc; }
};

