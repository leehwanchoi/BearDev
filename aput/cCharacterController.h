#pragma once
#include "iMap.h"

#define POWER_JUMP			12.0f	//���� ��
#define POWER_GRAVITY		20.0f	//�߷�
#define MAX_SPEED_PER_SEC	1000.0f	//������ �ִ� ���� �ӵ�
#define SPEED_WALK			3.0f	//�ȴ� �ӵ�
#define SPEED_RUN			10.0f	//�޸��� �ӵ�
#define MAX_HP				100		//�ִ� HP

class cSnowBall;
class cHPBar;
class cSnowBallUI;
class cParticle_Firework;

class cCharacterController : public cObject
{
private:
	//ĳ���� �̵����� ������
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

	//���콺 ���� ���� ����
	bool				m_isRButtonDown;
	POINT				m_ptPrevMouse;

	float				m_fMouseSensitivity;	//����

	//ĳ���� ���� ���� ����
	DWORD				m_dwState;
	ST_HP				m_stHp;
	cHPBar*				m_pHPBar;
	cSnowBallUI*		m_pSnowBallUI;

	float				m_fTrackPeriod;		// �� Ʈ���� �÷��� �ð� 
	float				m_fStackDeltaTime;	// �ִϸ��̼� �÷��� �� �ð� ���

	//���� ���� ����
	float				m_fHeight;
	float				m_fJumpPower;
	float				m_fGravity;
	float				m_fFallSpeed;
	bool				m_isJump;			//ĳ���Ͱ� ���������� Ȯ��
	bool				m_isOnTheGround;	//ĳ���Ͱ� ���� ����ִ��� Ȯ��

	//���� ���� ����
	bool				m_isAttack;
	bool				m_isThrow;
	bool				m_isHit;
	int					m_nAttackPower;		//���ݷ� ����

	//������ ���� ����
	int					m_nSnowBallNum;		//���������� ����
	int					m_nBigSnowBallNum;	//ū������ ����
	vector<cSnowBall*>	m_vecSnowBall;		//������ ��ü ����

	//Ư�� ���� ���� ����
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
	//ĳ���� ���¸� ������ bool ������
	m_isDeath		: m_fHp�� 0�� �Ǹ� true -> ��� ���� �Ұ�
	m_isHit			: �ִϸ��̼� 1�� ����Ǵ� ���� ����
	m_isAttack		: �ִϸ��̼� 1�� ����Ǵ� ���� ����
	m_isJump		: �ٴڿ� �浹�ϱ� ������ ����
	m_isMoving		: m_isMoving�� m_isOnTheGround�� ������ bool���� true�� �ƴ� ��� true 
	m_isYes, m_isNo, m_isBuild, m_isPickObj	 : �ִϸ��̼� 1�� ����Ǵ� ���� ���� 
	m_isSit, m_isSleep : �ٸ� ������ ���ϱ� ������ true
	m_isCollisionTornado : ����̵� ������������ ���߿� �� ����
	m_isFallByTornado : ���߿��� ���� �����Ҷ�����
	m_isSlideSlope : ������� �̲������� ����
	m_isOnTheGround : ĳ������ ���� ���� ����ִ� ��� true 
	*/
	//����Ʈ ����
	int					m_nCntSoundEffectDestroySnow;

	//Ư��ȿ�� ����
	cParticle_Firework*	m_pP_Firework;

	//����� ���� ���� ����
	bool				m_isOnKey2;
	LPD3DXFONT			m_pFont;
	LPRECT				m_pRect;

	//ĳ���� �浹 ���� ����
	SYNTHESIZE(bool, m_isCollision, IsCollision);
	SYNTHESIZE(bool, m_isCollisionTornado, IsCollisionTornado);

public:
	cCharacterController();
	~cCharacterController();

	void Setup(D3DXVECTOR3* pos);
	void Update(float fRotY);	
	void Render();

	void WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	//Get�Լ�
	D3DXVECTOR3* GetPosition() { return &m_vPosition; }
	D3DXVECTOR3* GetFirstPos() { return &m_vFirstPosition; }
	D3DXMATRIXA16* GetWorldTM() { return &m_matWorld; }
	float GetGroundHeight(){ return m_fGroundHeight; }
	bool GetIsMoving(){ return m_isMoving; }	//�Ⱦ���
	float GetMoveSpeed(){ return m_fMoveSpeed; }
	D3DXVECTOR3* GetvDir(){ return &m_vDir; }
	DWORD GetCharacterDirState(){ return m_dwDirState; }
	DWORD GetCharacterState(){ return m_dwState; }
	vector<cSnowBall*>& GetVecSnowBall(){ return m_vecSnowBall; }		//��ü������ ���� Get�Լ�
	int GetBigSnowBallNum(){ return m_nBigSnowBallNum; }
	bool GetIsAttack(){ return m_isAttack; }
	bool GetIsFallByTornado(){ return m_isFallByTornado; }
	float GetHp(){ return m_stHp.m_fHp; }
	float GetHpBar();
	bool GetHpBarIsSetHp();

	//Set�Լ�
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

	//ĳ���� ���� ���� �Լ�: �ִϸ��̼� ����
	void ManageCharacterState();
	void Revival(D3DXVECTOR3* pos = NULL);	//��Ȱ �Լ�

	//���� ���� �Լ�
	void Jump();
	void AirUpdate(D3DXVECTOR3* pos);
	void Collision(D3DXVECTOR3* pos);
	void Fall(D3DXVECTOR3* pos);

	//���� ���� �Լ�
	void Attack();
	void Throw();
	void Hit();
	void DestroySnowBall(int index);

	//Ư�� ���� ���� �Լ�
	void Yes();
	void No();
	void Build();
	void PickObj();
	void Sit();
	void Sleep();
	void CollisionTornado(D3DXVECTOR3* pos);
	void SlideSlopeVillage(D3DXVECTOR3* vPos);
	void SlideSlopeStage(D3DXVECTOR3* vPos, float fDepth);

	//���� ���� �Լ�
	bool IsDeath();
	void Death();

	//���� �߰� �Լ�
	void AddEffectSound();

	//����� ���� �Լ� �߰�
	void DebugRender_Character();
};