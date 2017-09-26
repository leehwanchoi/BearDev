#pragma once
#include"cGameNode.h"

#include "iMap.h"

class cCamera;
class cCharacterController;
class cMonsterController;
class cNPCController;
class cHeightMap;
class cSkinnedMesh;

//Level 추가
class cSkyBox;
class cLoadMap;

//A* 알고리즘
class cAStar;

//프러스텀 컬링
class cFrustum;

class cMainScene : public cGameNode
{
private:
	cCamera*				m_pCamera;
	cCharacterController*	m_pController;			//캐릭터 컨트롤러
	cMonsterController*		m_pMonsterController;	//몬스터 컨트롤러
	cNPCController*			m_pNPCController;		//NPC 컨트롤러
	cNPCController*			m_pNPCController2;		//NPC2 컨트롤러
	cNPCController*			m_pNPCController3;		//NPC3 컨트롤러

	iMap*					m_pMap;
	cHeightMap*				m_pHeight;

	cSkinnedMesh*			m_pSkinnedCharacter;	//캐릭터
	cSkinnedMesh*			m_pSkinnedMonster;		//몬스터
	cSkinnedMesh*			m_pSkinnedNPC;			//NPC
	cSkinnedMesh*			m_pSkinnedNPC2;			//NPC2
	cSkinnedMesh*			m_pSkinnedNPC3;			//NPC3

	//vector<cSkinnedMesh*>	m_vecSkinnedNPC;		//NPC vector

	//x맵 추가
	cSkinnedMesh*			m_pXMap;
	bool					m_isShowXMap;

	//A* 알고리즘 추가
	cAStar*					m_pAStar;

	//프러스텀 컬링
	cFrustum*				m_pFrustum;


	//디버깅 렌더 관련 변수 추가
	bool					m_isOnKey1;
	bool					m_isOnKey3;
	bool					m_isObjMap;		//m_pMap이 Obj맵인지 확인


	/*===================================================================================*/
	/*                                 New class, This Area                              */
	/*===================================================================================*/
	//Level
	cSkyBox*				m_pSkyBox;
	cLoadMap*				m_pLoadMap;

public:
	cMainScene();
	virtual ~cMainScene();

	virtual HRESULT init() override;
	virtual void Release() override;
	virtual void update() override;
	virtual void render() override;

	//디버깅 렌더 함수 추가
	void DebugRender();

	//캐릭터&몬스터 충돌처리 함수
	void Collision();

	virtual void WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
};

