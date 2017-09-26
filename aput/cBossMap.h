#pragma once
#include"cGameNode.h"
#include "iMap.h"

class cCamera;
class cCharacterController;
class cMonsterController;
class cSkinnedMesh;
class cSnowBall;
class cFrustum;
class cAStar;

class cSkyBox;
class cXobjectLoader;
class cLoadMap;

class cLoadingScene;
class cSinWave;
class cWaveWall;

class cParticle_Snow;
class cParticle_Walk;
class cSemicircleFX;
class cTeleporter;
class cTornado;

class cShowMenu;
class cShowClear;
class cShowYouDie;
class cShowStageNum;
class cAchievementBox;

class cBossMap : public cGameNode
{
private:
	cCamera*				m_pCamera;
	cCharacterController*	m_pController;
	cMonsterController*		m_pMonsterController;	//몬스터 컨트롤러
	cSkinnedMesh*			m_pSkinnedCharacter;	//캐릭터
	cSkinnedMesh*			m_pSkinnedMonster;		//몬스터

	/*===================================================================================*/
	/*                                 New class, This Area                              */
	/*===================================================================================*/
	iMap*					m_pMap;
	cSkyBox*				m_pSkyBox;

	//로드용
	cLoadMap*				m_pLoadMap;

	//로딩씬 + 씬변경
	cLoadingScene*			m_pLoadingScene;

	//파도
	cSinWave*				m_pSinWave;
	cWaveWall*				m_pWaveWall;

	//안개효과
	ID3DXEffect*			m_FogEffect;
	D3DXHANDLE				m_FogTechHandle;

	//FX
	cParticle_Snow*			m_pP_Snow;
	cParticle_Walk*			m_pP_Walk;
	vector<cSemicircleFX*>	m_vecSemicircleFX;
	cTeleporter*			m_pTeleporter;
	vector<cTornado*>		m_vecTornadoBarrier;
	vector<cTornado*>		m_vecTornado;

	//메뉴
	cShowMenu*				m_pShowMenu;
	cShowClear*				m_pShowClear;
	cShowYouDie*			m_pShowYouDie;
	cShowStageNum*			m_pShowStageNum;
	vector<cAchievementBox*>m_vecAchieveBox;
	/*===================================================================================*/
	
	//A* 알고리즘 추가
	cAStar*					m_pAStar;

	//프러스텀 컬링
	cFrustum*				m_pFrustum;

	//눈덩이 벡터
	vector<cSnowBall*>		m_vecSnowBall;

	//포탈
	BoundingBox				m_boundingBoxPortal;
	bool					m_isOpenPortal;
	int						m_nEffectSoundPorta;
	LPD3DXMESH				m_BoxMesh = 0;

	//충돌용 바운딩 박스
	vector<BoundingBox>		m_vecBoundingBox;

	//충돌용 바운딩 구체
	vector<ST_BUTTON>		m_vecBoundingSphereButton;

	//디버깅 렌더 관련 변수
	bool					m_isOnKey1;
	LPD3DXFONT				m_pFont;
	LPRECT					m_pRect;

public:
	cBossMap();
	~cBossMap();

	virtual HRESULT init() override;
	virtual void Release() override;
	virtual void update() override;
	virtual void render() override;

	virtual void WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;

	//토네이도 관련 함수
	void SetTornado();						//토네이도 셋팅 함수

	//포탈 관련 함수
	void SetPortal();						//포탈 셋팅 함수
	bool IsOpenPortal();					//포탈 열렸는지 확인 함수
	void RenderOpenPortal();				//포탈 출력 함수

	//충돌관련 함수
	void SetMapObjBoundingBox();			//맵 오브젝트 충돌판정 바운딩박스 셋팅 함수
	void SetButtonBoundingSphere();			//버튼 충돌판정 바운딩구체 셋팅 함수

	void UpdateSnowBallBoundingPosition();	//눈뭉치 충돌판정 바운딩구체 포지션 업데이트 함수
	void UpdateTornado();					//토네이도 상태 업데이트 함수

	void CollisionCharacter_MapObj();		//캐릭터 X 맵 오브젝트 충돌처리 함수
	void CollisionCharacter_Button();		//캐릭터 X 버튼 충돌처리 함수
	void CollisionCharacter_Tornado();		//캐릭터 X 토네이도 충돌처리 함수
	void CollisionCharacter_Monster();		//캐릭터 X 몬스터 충돌처리 함수
	void CollisionMonster_Character();		//몬스터 X 캐릭터 충돌처리 함수
	void CollisionMonster_MapObj();			//몬스터 X 맵 오브젝트 충돌처리 함수
	void CollisionSnowBall_MapObj();		//눈뭉치 X 맵 오브젝트 충돌처리 함수
	void CollisionSnowBall_Button();		//눈뭉치 X 버튼 충돌처리 함수
	void CollisionSnowBall_Monster();		//눈뭉치 X 몬스터 충돌처리 함수
	void CollisionSnowBall_SnowBall();		//눈뭉치 X 눈뭉치 충돌처리 함수

	//디버깅 렌더 함수
	void DebugRender();
	void DebugRender_Portal();				//포탈 디버깅함수
};

