#pragma once
#include "iMap.h"

#define POWER_JUMP			12.0f	//점프 힘
#define POWER_GRAVITY		20.0f	//중력
#define MAX_SPEED_PER_SEC	1000.0f	//점프시 최대 낙하 속도
#define SPEED_WALK			3.0f	//걷는 속도
#define SPEED_RUN			8.0f	//달리는 속도
#define SPEED_CHARGE		15.0f	//돌진 속도
#define MAX_HP				100		//최대 HP
#define DISTANCE_RECOGNIZE	22.0f	//인지범위 거리	
#define DISTANCE_ATTACK		5.0f	//공격범위 거리
#define DISTANCE_CHARGE		10.0f	//돌진범위 거리

class cAStar;	//A* 추가
class cCube;
class cMonsterHPBar;

class cMonsterController : public cObject
{
private:
	//몬스터 이동관련 변수들
	D3DXVECTOR3			m_vPosition;
	D3DXVECTOR3			m_vFirstPosition;
	D3DXVECTOR3			m_vCharacterPosition;

	bool				m_isMoving;
	bool				m_isStart;
	bool				m_isDiePlay;
	float				m_fMoveSpeed;
	float				m_fRotY;

	D3DXVECTOR3			m_vDir;
	D3DXVECTOR3			m_vDest; //목적지벡터
	D3DXMATRIXA16		m_matRotY;
	D3DXMATRIXA16		m_matWorld;

	iMap*				m_pMap;

	//A*관련 변수
	cAStar*				m_pAStar;
	bool				m_isNeededAStar;
	bool				m_isAStarStart;		//A*이용 캐릭터 쫓아감
	bool				m_isAStarGoHome;	//A*이용 제자리로 돌아감
	bool				m_isDebugMode;		//A* 연구용 디버깅 변수

	vector<D3DXVECTOR3>	m_vecTriGrid; 		// 맵 픽킹 삼각형 간소화
	vector<D3DXVECTOR3> m_vecVertex;
	vector<DWORD>		m_vecIndex;

	vector<D3DXVECTOR3> m_vecAStarDest;
	vector<D3DXVECTOR3>	m_vecObstcles;

	//몬스터 인식범위 관련 변수
	LPD3DXMESH			m_pSphereRangeMeshRec;
	LPD3DXMESH			m_pSphereRangeMeshAtk;
	LPD3DXMESH			m_pSphereRangeMeshCharge;

	//몬스터 상태 관리 변수
	DWORD				m_dwState;
	DWORD				m_dwCharacterState;
	ST_HP				m_stHp;
	cMonsterHPBar*		m_pHPBar;

	float				m_fTrackPeriod;		// 한 트랙의 플레이 시간 
	float				m_fStackDeltaTime;	// 애니메이션 플레이 후 시간 경과
	bool				m_isResetStackDeltaTime;

	//점프 관련 변수
	float				m_fHeight;
	float				m_fJumpPower;
	float				m_fGravity;
	float				m_fFallSpeed;
	bool				m_isJump;			//몬스터 점프중인지 확인
	bool				m_isOnTheGround;	//몬스터가 땅에 닿아있는지 확인

	//전투 관련 변수
	bool				m_isAttack;
	bool				m_isCharge;
	bool				m_isReady;
	bool				m_isHit;
	bool				m_isHitBySnowBall;
	int					m_nAttackPower;		//공격력 변수
	int					m_nRandomAttack;	//공격 랜덤 선택 변수

	//특정 동작 관련 변수
	bool				m_isSit;
	bool				m_isSleep;
	bool				m_isEmotion;	//포효
	bool				m_isWakeUp;		//일어나서 포효
	bool				m_isForget;		//두리번
	bool				m_isForgetOn;

	/*
	//몬스터 상태를 구분할 bool 변수들
	m_isDeath		: m_fHp가 0이 되면 true -> 모든 동작 불가
	m_isHit			: 애니메이션 1번 재생되는 동안 유지
	m_isAttack		: 애니메이션 1번 재생되는 동안 유지
	m_isJump		: 바닥에 충돌하기 전까지 유지
	m_isMoving		: m_isMoving과 m_isOnTheGround를 제외한 bool값이 true가 아닌 경우 true
	m_isSit, m_isSleep : 다른 동작을 취하기 전까지 true
	m_isReady :
	m_isWakeUp :
	m_isForget :
	m_isEmotion : 
	m_isOnTheGround : 몬스터의 발이 땅에 닿아있는 경우 true

	m_isAStarStart
	m_isAStarGoHome
	*/

	//디버깅 렌더 관련 변수
	bool				m_isOnKey3;
	LPD3DXFONT			m_pFont;
	LPRECT				m_pRect;

	//몬스터 충돌 관련 함수
	SYNTHESIZE(bool, m_isCollision, IsCollision);

public:
	cMonsterController();
	~cMonsterController();

	void Setup(D3DXVECTOR3* pos = NULL);
	void Update(D3DXVECTOR3* pos, DWORD characterState);
	void Render();

	//몬스터 동작 관련 함수
	void SetDestinationAStar(D3DXVECTOR3* dest);
	bool IsInRecognizeRange(D3DXVECTOR3* pos);
	bool IsInAttackRange(D3DXVECTOR3* pos);
	bool IsInChargeRange(D3DXVECTOR3* pos);

	//Get함수
	D3DXVECTOR3* GetPosition() { return &m_vPosition; }
	D3DXVECTOR3* GetFirstPos() { return &m_vFirstPosition; }
	D3DXMATRIXA16* GetWorldTM() { return &m_matWorld; }
	bool GetIsMoving(){ return m_isMoving; }	//안쓰임
	bool GetIsDiePlay(){ return m_isDiePlay; }
	bool GetIsAttack(){ return m_isAttack; }
	bool GetIsCharge(){ return m_isCharge; }
	bool GetIsHit(){ return m_isHit; }
	DWORD GetCharacterState(){ return m_dwState; }
	float GetMaxHp(){ return m_stHp.m_fMaxHp; }
	float GetHp(){ return m_stHp.m_fHp; }
	bool GetIsDead(){ return m_stHp.m_isDeath; }

	//Set함수
	void SetMap(iMap* pMap);
	void SetAStar(cAStar* pAStar){ m_pAStar = pAStar; }
	void SetTrackPeriod(float period){ m_fTrackPeriod = period; }
	void SetIsHit(bool isHit){ m_isHit = isHit; }
	void SetIsHitBySnowBall(bool isHit){ m_isHitBySnowBall = isHit; }
	void SetIsCharge(bool isCharge){ m_isCharge; }
	void SetHp(float hp){ m_stHp.m_fHp = hp; }
	void SetPosition(D3DXVECTOR3* pos){ m_vPosition = *pos; }
	void SetDir(D3DXVECTOR3* dir){ m_vPosition = *dir; }
	void SetIsResetStackDeltaTime(bool isResetTime){ m_isResetStackDeltaTime = isResetTime; }
	void SetStackDeltaTime(float time){ m_fStackDeltaTime = time; }

	//몬스터 상태 관리 함수: 애니메이션 관리
	void ManageMonsterState();

	//전투 관련 함수
	void Attack1();
	void Attack2();
	void Attack3();
	void ChargeAttack();
	void Hit();
	void DamageOn();

	//특정 동작 관련 함수
	void Sit();
	void WakeUpEmotion();
	void Emotion();
	void ForgetAndGoHome();

	//죽음 판정 함수
	bool IsDeath();
	void Death();
	void Reset();

	//디버깅 렌더 함수 추가
	void DebugRender_Monster();
};

