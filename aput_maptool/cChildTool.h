#pragma once
#include "cChildWindowUIButton.h"

class cChildWindowUIobject;

enum 
{
	//��ư
	E_childGridDownBUTTON = 0, E_childGridUpBUTTON,					//�׸��� ���� ��ư
	E_childGridMinHeightDownBUTTON, E_childGridMinHeightUpBUTTON,	//�׸��� ���� ���� ��ư
	E_childGridMaxHeightDownBUTTON, E_childGridMaxHeightUpBUTTON,	//�׸��� ���� ���� ��ư
	E_childGridFlatHeightDownBUTTON, E_childGridFlatHeightUpBUTTON, //�׸��� ��źȭ ���� ��ư
	E_childGridSpeedDownBUTTON, E_childGridSpeedUpBUTTON,			//�׸� �ӵ� ���� ��ư
	E_childXFileLoadBUTTON,									//X ���� �ε� ��ư
	E_childTextureLoadBUTTON,								//�ؽ��� �ε� ��ư
	E_childLoadBUTTON, E_childSaveBUTTON,					//�ε�, ���̺� ��ư
	E_childModBUTTON,										//��� ���� ��ư
	E_childActivateBUTTON,									//���� ��ư

	//�ؽ�Ʈ��
	E_childGridTEXTVIEW,									//�׸��� �ؽ�Ʈ��
	E_childGridMinHeightTEXTVIEW,							//�׸��� �ּ� ���� �ؽ�Ʈ��
	E_childGridMaxHeightTEXTVIEW,							//�׸��� �ִ� ���� �ؽ�Ʈ��
	E_childGridFlatHeightTEXTVIEW,							//�׸��� ��źȭ ���� �ؽ�Ʈ��
	E_childGridSpeedTEXTVIEW,								//�׸��� �ӵ� �ؽ�Ʈ��
	E_childXFileTEXTVIEW,									//X ���� �ؽ�Ʈ��
	E_childTextureTEXTVIEW									//�ؽ��� �ؽ�Ʈ��
};

class cChildTool : public cObject, public iChildWindowUIButtonDelegate
{
private:
	LPD3DXSPRITE				m_pSprite;
	cChildWindowUIobject*		m_pUIRoot;

	//�׸��� ����� ����
	string						m_stringVertexNum;
	int							m_nVertexNum;
	string						m_stringTileNum;
	int							m_nTileNum;
	string						m_stringVertexHeight;
	float						m_fVertexMaxHeight;
	float						m_fVertexMinHeight;
	float						m_fFlatteningHeight;
	float						m_fTerrainSpeed;

	//����, �ε�, ���̺�, �ؽ���, ��� Ȯ�ο� ����
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

	// �������̽� �Լ� ����
	virtual void OnClick(cChildWindowUIButton* pSender) override;

	//�׸��� ���� �Լ�
	void SelectGrid();

	//�׸��� ����� �Լ�
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

	//����, �ε�, ���̺� Ȯ�ο� �Լ�
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

