#pragma once
#include "iMap.h"

class cAStar;
class cCube;

class cCharacterController : public cObject
{
private:
	D3DXVECTOR3			m_vPosition;
	float				m_fMoveSpeed;
	bool				m_isMoving;
	D3DXMATRIXA16		m_matWorld;

	bool				m_isDebugMode;

	iMap*				m_pMap;			// 맵 정보 포인터
	// 맵 픽킹 삼각형 간소화
	vector<D3DXVECTOR3>	m_vecTriGrid;	// 간소화 시킨 평면 정점 정보를 담을 벡터

	// 맵 픽킹, 픽킹 지점으로의 이동 및 캐릭터 방향 설정 변수들
	vector<D3DXVECTOR3> m_vecVertex;
	vector<DWORD>		m_vecIndex;
	D3DXVECTOR3			m_vDest;
	D3DXVECTOR3			m_vDir;
	D3DXMATRIXA16		m_matRotY;

	cAStar*				m_pAStar;
	vector<D3DXVECTOR3> m_vecAStarDest;

	bool				m_isNeededAStar;
	vector<D3DXVECTOR3>	m_vecObstcles;

public:
	cCharacterController();
	~cCharacterController();

	void Setup();
	void Update();
	void Render();
	D3DXVECTOR3* GetPosition() { return &m_vPosition; }
	D3DXMATRIXA16* GetWorldTM() { return &m_matWorld; }

	void SetMap(iMap* pMap);
	void SetAStar(cAStar* pAStar) { m_pAStar = pAStar; }
	void WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};