#pragma once
#include "iMap.h"

#define SPEED_WALK			1.0f	//걷는 속도
#define SPEED_RUN			10.0f	//달리는 속도
#define DISTANCE_RECOGNIZE	1.8f	//캐릭터의 NPC인지범위 거리		

class cTextBox;

class cNPCController : public cObject
{
private:
	//NPC 이동관련 변수들
	D3DXVECTOR3			m_vPosition;
	D3DXVECTOR3			m_vFirstPosition;
	D3DXVECTOR3			m_vCharacterPosition;

	bool				m_isMoving;
	float				m_fMoveSpeed;
	float				m_fRotY;
	float				m_fMoveDistance;

	D3DXVECTOR3			m_vDir;
	D3DXVECTOR3			m_vFirstDir;
	D3DXVECTOR3			m_vLastDir;
	D3DXVECTOR3			m_vLookFirstDir;
	D3DXVECTOR3			m_vLookDir;
	D3DXMATRIXA16		m_matRotY;
	D3DXMATRIXA16		m_matWorld;

	iMap*				m_pMap;

	//NPC 상태 관리 변수
	DWORD				m_dwState;
	DWORD				m_dwMoveMode;		//NPC 이동방식

	int					m_nRandomWalk;
	float				m_fTrackPeriod;		// 한 트랙의 플레이 시간 
	float				m_fStackDeltaTime;	// 애니메이션 플레이 후 시간 경과
	D3DXVECTOR3			m_vFromDir;			// 처음 바라보는 방향
	D3DXVECTOR3			m_vToDir;			// 변경되는 방향

	//특정 동작 관련 변수
	bool				m_isYes;
	bool				m_isNo;
	bool				m_isSit;
	bool				m_isSleep;

	//NPC 대화&퀘스트 관련 변수
	cTextBox*			m_pUITextBox;
	bool				m_canTalk;			//캐릭터 인지범위 안에 들어왔을때 true
	bool				m_isLookAt;			//대화시작시 NPC 각도조절
	bool				m_isEndTalk;		//대화끝났을시 NPC 각도조절
	bool				m_isSmoothTurn;		//부드럽게 회전
	bool				m_isShowTalkUI;		//대화 UI 렌더여부
	float				m_fActionTime;		//동작이 진행되는 시간
	float				m_fPassedTime;		//누적된 시간

	/*
	//NPC 상태를 구분할 bool 변수들
	m_isMoving		: m_isMoving과 m_isOnTheGround를 제외한 bool값이 true가 아닌 경우 true
	m_isYes, m_isNo	 : 애니메이션 1번 재생되는 동안 유지
	m_isSit, m_isSleep : 다른 동작을 취하기 전까지 true
	*/

	//디버깅 렌더 관련 변수
	bool				m_isOnKey4;
	LPD3DXFONT			m_pFont;
	LPRECT				m_pRect;
	LPD3DXMESH			m_pSphereRange;

	//NPC 대화 클리어 변수
	SYNTHESIZE(bool, m_isClear, IsClear);

	//NPC 충돌 관련 변수
	SYNTHESIZE(bool, m_isCollision, IsCollision);
public:
	cNPCController();
	~cNPCController();

	void Setup(D3DXVECTOR3* pos, float fRotY, DWORD mode);
	void Update(D3DXVECTOR3* pos);
	void Render();

	//Get함수
	D3DXVECTOR3* GetPosition() { return &m_vPosition; }
	D3DXMATRIXA16* GetMatRotY() { return &m_matRotY; }		//추가
	D3DXMATRIXA16* GetWorldTM() { return &m_matWorld; }
	DWORD GetCharacterState(){ return m_dwState; }
	bool GetIsMoving(){ return m_isMoving; }	//안쓰임
	bool GetIsLookAt(){ return m_isLookAt; }
	bool GetIsCanTalk(){ return m_canTalk; }

	//Set함수
	void SetMap(iMap* pMap){ m_pMap = pMap; };
	void SetTrackPeriod(float period){ m_fTrackPeriod = period; }
	void SetMoveDistance(float dis){ m_fMoveDistance = dis; }	//이동거리 설정 함수

	//NPC 이동방식 관련 함수
	void NotMove();
	void MoveOneway();
	void MoveSurround();

	//NPC가 캐릭터 인지범위 안에 있는지 확인하는 함수
	bool IsInRecognizeRange();

	//NPC 대화&퀘스트 관련 함수
	void SetTalkTextBox(string Name, string Dialouge, bool isDialouge); //대화UI 설정함수
	void LookAt(D3DXVECTOR3* vDir);	//NPC가 캐릭터 바라봄
	void SmoothTurn(OUT D3DXVECTOR3* vDir, D3DXVECTOR3* vFrom, D3DXVECTOR3* vTo);	//부드럽게 회전 보간
	void Talk();	//대화UI Update

	//NPC 상태 관리 함수: 애니메이션 관리
	void ManageNPCState();

	//특정 동작 관련 함수
	void Idle();
	void Yes();
	void No();
	void Sit();
	void Sleep();

	//디버깅 렌더 함수
	void DebugRender_NPC();
};

