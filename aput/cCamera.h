#pragma once
#include "iMap.h"

#define ROTY_MIN_RANGEX 10
#define ROTY_MAX_RANGEX ((WINSIZEX -15) - 10)

class cCamera : public cObject
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

	//마우스 조작에 필요한 변수
	POINT			m_ptPrevMouse;

	bool			m_isAutoRotY;
	bool			m_isTalk;
	bool			m_isShowMenu;
	float			m_fMouseSensitivity;
	iMap*			m_pMap;

	//디버깅 관련 변수
	vector<ST_RHWC_VERTEX> m_vecVertex;

public:
	cCamera();
	~cCamera();

	void Setup();
	void Update(D3DXVECTOR3* pTarget = NULL);
	void UpdateControllByJoystick();
	void WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	//Get함수
	D3DXMATRIXA16 GetViewMatrix() { return m_matView; }
	D3DXMATRIXA16 GetProjMatrix() { return m_matProj; }
	D3DXMATRIXA16 GetViewProjMatrix() { return m_matView * m_matProj; }

	D3DXVECTOR3& GetEyePosition(){ return m_vEye; }
	D3DXVECTOR3& GetLookAtPosition(){ return m_vLookAt; }
	float GetfRotationY(){ return m_fRotY; }
	float GetfRotationX(){ return m_fRotX; }

	//Set함수
	void SetRotY(float fAngle){ m_fRotY = fAngle; }
	void SetRotX(float fAngle){ m_fRotX = fAngle; }
	void SetMap(iMap* pMap){ m_pMap = pMap; };
	void SetDistance(float dis){ m_fDistance = dis; }
	void SetIsTalk(bool istalk){ m_isTalk = istalk; }
	void SetIsShowMenu(bool isShowMenu){ m_isShowMenu = isShowMenu; }

	//디버깅함수
	void DebugRender();
};

