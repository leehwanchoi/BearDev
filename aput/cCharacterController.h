#pragma once
#include "iMap.h"

#define POWER_JUMP			12.0f	//점프 힘
#define POWER_GRAVITY		20.0f	//중력
#define MAX_SPEED_PER_SEC	1000.0f	//점프시 최대 낙하 속도
#define SPEED_WALK			3.0f	//걷는 속도
#define SPEED_RUN			10.0f	//달리는 속도
#define MAX_HP				100		//최대 HP

class cSnowBall;
class cHPBar;
class cSnowBallUI;
class cParticle_Firework;

class cCharacterController : public cObject
{
private:
	//캐릭터 이동관련 변수들
	D3DXVECTOR3			m_vPosition;
	D3DXVECTOR3			m_vFirstPosition;
	float				m_fGroundHeight;

	bool				m_isMoving;
	float				m_fMoveSpeed;
	float				m_fRotY;

	DWORD				m_dwDirState;
	D3DXVECTOR3			m_vDir;
	D3DXMATRIXA16		m_matRotY;
	D3DXMATRIXA16		m_matWorld;

	iMap*				m_pMap;

	//마우스 조작 관련 변수
	bool				m_isRButtonDown;
	POINT				m_ptPrevMouse;

	float				m_fMouseSensitivity;	//감도

	//캐릭터 상태 관리 변수
	DWORD				m_dwState;
	ST_HP				m_stHp;
	cHPBar*				m_pHPBar;
	cSnowBallUI*		m_pSnowBallUI;

	float				m_fTrackPeriod;		// 한 트랙의 플레이 시간 
	float				m_fStackDeltaTime;	// 애니메이션 플레이 후 시간 경과

	//점프 관련 변수
	float				m_fHeight;
	float				m_fJumpPower;
	float				m_fGravity;
	float				m_fFallSpeed;
	bool				m_isJump;			//캐릭터가 점프중인지 확인
	bool				m_isOnTheGround;	//캐릭터가 땅에 닿아있는지 확인

	//전투 관련 변수
	bool				m_isAttack;
	bool				m_isThrow;
	bool				m_isHit;
	int					m_nAttackPower;		//공격력 변수

	//눈덩이 관련 변수
	int					m_nSnowBallNum;		//작은눈덩이 갯수
	int					m_nBigSnowBallNum;	//큰눈덩이 갯수
	vector<cSnowBall*>	m_vecSnowBall;		//눈덩이 전체 벡터

	//특정 동작 관련 변수
	bool				m_isSpecial;
	bool				m_isYes;
	bool				m_isNo;
	bool				m_isBuild;
	bool				m_isPickObj;
	bool				m_isSit;
	bool				m_isSleep;
	bool				m_isFallByTornado;
	bool				m_isSlideSlope;
	bool				m_isCanTalk;

	/*
	//캐릭터 상태를 구분할 bool 변수들
	m_isDeath		: m_fHp가 0이 되면 true -> 모든 동작 불가
	m_isHit			: 애니메이션 1번 재생되는 동안 유지
	m_isAttack		: 애니메이션 1번 재생되는 동안 유지
	m_isJump		: 바닥에 충돌하기 전까지 유지
	m_isMoving		: m_isMoving과 m_isOnTheGround를 제외한 bool값이 true가 아닌 경우 true 
	m_isYes, m_isNo, m_isBuild, m_isPickObj	 : 애니메이션 1번 재생되는 동안 유지 
	m_isSit, m_isSleep : 다른 동작을 취하기 전까지 true
	m_isCollisionTornado : 토네이도 만났을때부터 공중에 뜬 동안
	m_isFallByTornado : 공중에서 땅에 착지할때까지
	m_isSlideSlope : 경사진곳 미끄러지는 동안
	m_isOnTheGround : 캐릭터의 발이 땅에 닿아있는 경우 true 
	*/
	//이펙트 사운드
	int					m_nCntSoundEffectDestroySnow;

	//특수효과 변수
	cParticle_Firework*	m_pP_Firework;

	//디버깅 렌더 관련 변수
	bool				m_isOnKey2;
	LPD3DXFONT			m_pFont;
	LPRECT				m_pRect;

	//캐릭터 충돌 관련 변수
	SYNTHESIZE(bool, m_isCollision, IsCollision);
	SYNTHESIZE(bool, m_isCollisionTornado, IsCollisionTornado);

public:
	cCharacterController();
	~cCharacterController();

	void Setup(D3DXVECTOR3* pos);
	void Update(float fRotY);	
	void Render();

	void WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	//Get함수
	D3DXVECTOR3* GetPosition() { return &m_vPosition; }
	D3DXVECTOR3* GetFirstPos() { return &m_vFirstPosition; }
	D3DXMATRIXA16* GetWorldTM() { return &m_matWorld; }
	float GetGroundHeight(){ return m_fGroundHeight; }
	bool GetIsMoving(){ return m_isMoving; }	//안쓰임
	float GetMoveSpeed(){ return m_fMoveSpeed; }
	D3DXVECTOR3* GetvDir(){ return &m_vDir; }
	DWORD GetCharacterDirState(){ return m_dwDirState; }
	DWORD GetCharacterState(){ return m_dwState; }
	vector<cSnowBall*>& GetVecSnowBall(){ return m_vecSnowBall; }		//전체눈덩이 벡터 Get함수
	int GetBigSnowBallNum(){ return m_nBigSnowBallNum; }
	bool GetIsAttack(){ return m_isAttack; }
	bool GetIsFallByTornado(){ return m_isFallByTornado; }
	float GetHp(){ return m_stHp.m_fHp; }
	float GetHpBar();
	bool GetHpBarIsSetHp();

	//Set함수
	void SetMap(iMap* pMap){ m_pMap = pMap; };
	void SetTrackPeriod(float period){ m_fTrackPeriod = period; }
	void SetStackDeltaTime(float time){ m_fStackDeltaTime = time; }
	void SetCharacterState(DWORD state){ m_dwState = state; }
	void SetSmallSnowBallNum(int num){ m_nSnowBallNum = num; }
	void SetBigSnowBallNum(int num){ m_nBigSnowBallNum = num; }
	void SetIsHit(bool isHit){ m_isHit = isHit; }
	void SetIsColiisionTornado(bool isCollision){ m_isCollisionTornado = isCollision; }
	void SetIsSlideSlope(bool isSlid){ m_isSlideSlope = isSlid; }
	void SetIsCanTalk(bool isCanTalk){ m_isCanTalk = isCanTalk; }
	void SetHp(float hp){ m_stHp.m_fHp = hp; }
	void SetHpBar(float hp);
	void SetHpBarIsSetHp(bool isSetHp);

	//캐릭터 상태 관리 함수: 애니메이션 관리
	void ManageCharacterState();
	void Revival(D3DXVECTOR3* pos = NULL);	//부활 함수

	//점프 관련 함수
	void Jump();
	void AirUpdate(D3DXVECTOR3* pos);
	void Collision(D3DXVECTOR3* pos);
	void Fall(D3DXVECTOR3* pos);

	//전투 관련 함수
	void Attack();
	void Throw();
	void Hit();
	void DestroySnowBall(int index);

	//특정 동작 관련 함수
	void Yes();
	void No();
	void Build();
	void PickObj();
	void Sit();
	void Sleep();
	void CollisionTornado(D3DXVECTOR3* pos);
	void SlideSlopeVillage(D3DXVECTOR3* vPos);
	void SlideSlopeStage(D3DXVECTOR3* vPos, float fDepth);

	//죽음 판정 함수
	bool IsDeath();
	void Death();

	//사운드 추가 함수
	void AddEffectSound();

	//디버깅 렌더 함수 추가
	void DebugRender_Character();
};