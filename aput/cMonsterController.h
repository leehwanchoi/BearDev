#pragma once
#include "iMap.h"

#define POWER_JUMP			12.0f	//���� ��
#define POWER_GRAVITY		20.0f	//�߷�
#define MAX_SPEED_PER_SEC	1000.0f	//������ �ִ� ���� �ӵ�
#define SPEED_WALK			3.0f	//�ȴ� �ӵ�
#define SPEED_RUN			8.0f	//�޸��� �ӵ�
#define SPEED_CHARGE		15.0f	//���� �ӵ�
#define MAX_HP				100		//�ִ� HP
#define DISTANCE_RECOGNIZE	22.0f	//�������� �Ÿ�	
#define DISTANCE_ATTACK		5.0f	//���ݹ��� �Ÿ�
#define DISTANCE_CHARGE		10.0f	//�������� �Ÿ�

class cAStar;	//A* �߰�
class cCube;
class cMonsterHPBar;

class cMonsterController : public cObject
{
private:
	//���� �̵����� ������
	D3DXVECTOR3			m_vPosition;
	D3DXVECTOR3			m_vFirstPosition;
	D3DXVECTOR3			m_vCharacterPosition;

	bool				m_isMoving;
	bool				m_isStart;
	bool				m_isDiePlay;
	float				m_fMoveSpeed;
	float				m_fRotY;

	D3DXVECTOR3			m_vDir;
	D3DXVECTOR3			m_vDest; //����������
	D3DXMATRIXA16		m_matRotY;
	D3DXMATRIXA16		m_matWorld;

	iMap*				m_pMap;

	//A*���� ����
	cAStar*				m_pAStar;
	bool				m_isNeededAStar;
	bool				m_isAStarStart;		//A*�̿� ĳ���� �Ѿư�
	bool				m_isAStarGoHome;	//A*�̿� ���ڸ��� ���ư�
	bool				m_isDebugMode;		//A* ������ ����� ����

	vector<D3DXVECTOR3>	m_vecTriGrid; 		// �� ��ŷ �ﰢ�� ����ȭ
	vector<D3DXVECTOR3> m_vecVertex;
	vector<DWORD>		m_vecIndex;

	vector<D3DXVECTOR3> m_vecAStarDest;
	vector<D3DXVECTOR3>	m_vecObstcles;

	//���� �νĹ��� ���� ����
	LPD3DXMESH			m_pSphereRangeMeshRec;
	LPD3DXMESH			m_pSphereRangeMeshAtk;
	LPD3DXMESH			m_pSphereRangeMeshCharge;

	//���� ���� ���� ����
	DWORD				m_dwState;
	DWORD				m_dwCharacterState;
	ST_HP				m_stHp;
	cMonsterHPBar*		m_pHPBar;

	float				m_fTrackPeriod;		// �� Ʈ���� �÷��� �ð� 
	float				m_fStackDeltaTime;	// �ִϸ��̼� �÷��� �� �ð� ���
	bool				m_isResetStackDeltaTime;

	//���� ���� ����
	float				m_fHeight;
	float				m_fJumpPower;
	float				m_fGravity;
	float				m_fFallSpeed;
	bool				m_isJump;			//���� ���������� Ȯ��
	bool				m_isOnTheGround;	//���Ͱ� ���� ����ִ��� Ȯ��

	//���� ���� ����
	bool				m_isAttack;
	bool				m_isCharge;
	bool				m_isReady;
	bool				m_isHit;
	bool				m_isHitBySnowBall;
	int					m_nAttackPower;		//���ݷ� ����
	int					m_nRandomAttack;	//���� ���� ���� ����

	//Ư�� ���� ���� ����
	bool				m_isSit;
	bool				m_isSleep;
	bool				m_isEmotion;	//��ȿ
	bool				m_isWakeUp;		//�Ͼ�� ��ȿ
	bool				m_isForget;		//�θ���
	bool				m_isForgetOn;

	/*
	//���� ���¸� ������ bool ������
	m_isDeath		: m_fHp�� 0�� �Ǹ� true -> ��� ���� �Ұ�
	m_isHit			: �ִϸ��̼� 1�� ����Ǵ� ���� ����
	m_isAttack		: �ִϸ��̼� 1�� ����Ǵ� ���� ����
	m_isJump		: �ٴڿ� �浹�ϱ� ������ ����
	m_isMoving		: m_isMoving�� m_isOnTheGround�� ������ bool���� true�� �ƴ� ��� true
	m_isSit, m_isSleep : �ٸ� ������ ���ϱ� ������ true
	m_isReady :
	m_isWakeUp :
	m_isForget :
	m_isEmotion : 
	m_isOnTheGround : ������ ���� ���� ����ִ� ��� true

	m_isAStarStart
	m_isAStarGoHome
	*/

	//����� ���� ���� ����
	bool				m_isOnKey3;
	LPD3DXFONT			m_pFont;
	LPRECT				m_pRect;

	//���� �浹 ���� �Լ�
	SYNTHESIZE(bool, m_isCollision, IsCollision);

public:
	cMonsterController();
	~cMonsterController();

	void Setup(D3DXVECTOR3* pos = NULL);
	void Update(D3DXVECTOR3* pos, DWORD characterState);
	void Render();

	//���� ���� ���� �Լ�
	void SetDestinationAStar(D3DXVECTOR3* dest);
	bool IsInRecognizeRange(D3DXVECTOR3* pos);
	bool IsInAttackRange(D3DXVECTOR3* pos);
	bool IsInChargeRange(D3DXVECTOR3* pos);

	//Get�Լ�
	D3DXVECTOR3* GetPosition() { return &m_vPosition; }
	D3DXVECTOR3* GetFirstPos() { return &m_vFirstPosition; }
	D3DXMATRIXA16* GetWorldTM() { return &m_matWorld; }
	bool GetIsMoving(){ return m_isMoving; }	//�Ⱦ���
	bool GetIsDiePlay(){ return m_isDiePlay; }
	bool GetIsAttack(){ return m_isAttack; }
	bool GetIsCharge(){ return m_isCharge; }
	bool GetIsHit(){ return m_isHit; }
	DWORD GetCharacterState(){ return m_dwState; }
	float GetMaxHp(){ return m_stHp.m_fMaxHp; }
	float GetHp(){ return m_stHp.m_fHp; }
	bool GetIsDead(){ return m_stHp.m_isDeath; }

	//Set�Լ�
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

	//���� ���� ���� �Լ�: �ִϸ��̼� ����
	void ManageMonsterState();

	//���� ���� �Լ�
	void Attack1();
	void Attack2();
	void Attack3();
	void ChargeAttack();
	void Hit();
	void DamageOn();

	//Ư�� ���� ���� �Լ�
	void Sit();
	void WakeUpEmotion();
	void Emotion();
	void ForgetAndGoHome();

	//���� ���� �Լ�
	bool IsDeath();
	void Death();
	void Reset();

	//����� ���� �Լ� �߰�
	void DebugRender_Monster();
};

