#pragma once
#include "cGameNode.h"

class cPickingCamera;
class cToolCameraController;
class cTerrain;
class cChildTool;
class cMapToolInUI;
class cXobjectLoader;
class cSkyBox;

class cMapTool : public cGameNode
{
private:
	cPickingCamera*			m_pPickingCamera;
	cToolCameraController*	m_pToolCameraController;
	cTerrain*				m_pTerrain;
	cChildTool*				m_pChildTool;
	cMapToolInUI*			m_pMapToolInUI;
	cSkyBox*				m_pSkyBox;

	float					m_pXLoadTimeCount; //X���� ����� ī��Ʈ
	string					m_lpstrFile;	   //X���� �ε� �����̸� ���� ����

	float					m_fRotY;

public:
	cMapTool();
	~cMapTool();

	virtual HRESULT Init() override;
	virtual void Update() override;
	virtual void Render() override;
	virtual void Release() override;
	virtual void WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;

	//���̺� �ε� (�ٸ� �͵��� cTerrain�� ����.)
	void LoadAll();
	void SaveAll();
};

