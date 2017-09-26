#pragma once

class cPickingCamera : public cObject
{
private:
	float			m_fDistance;
	D3DXVECTOR3		m_vEye;
	D3DXVECTOR3		m_vLookAt;
	D3DXVECTOR3		m_vUp;

	D3DXMATRIXA16	m_matView;
	D3DXMATRIXA16	m_matProj;

	float			m_fRotY;
	float			m_fRotX;

	bool			m_isRButtonDown;		// 마우스 조작에 필요
	POINT			m_ptPrevMouse;			// 마우스 조작에 필요

public:
	cPickingCamera();
	~cPickingCamera();

	void Setup();
	void Update(D3DXVECTOR3* pTarget = NULL);

	D3DXMATRIXA16 GetViewMatrix() { return m_matView; }
	D3DXMATRIXA16 GetProjMatrix() { return m_matProj; }
	D3DXMATRIXA16 GetViewProjMatrix() { return m_matView * m_matProj; }

	float GetRotX() { return m_fRotX; }
	float GetRotY() { return m_fRotY; }

	void WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

