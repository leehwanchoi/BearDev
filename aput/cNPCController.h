#pragma once
#include "iMap.h"

#define SPEED_WALK			1.0f	//�ȴ� �ӵ�
#define SPEED_RUN			10.0f	//�޸��� �ӵ�
#define DISTANCE_RECOGNIZE	1.8f	//ĳ������ NPC�������� �Ÿ�		

class cTextBox;

class cNPCController : public cObject
{
private:
	//NPC �̵����� ������
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

	//NPC ���� ���� ����
	DWORD				m_dwState;
	DWORD				m_dwMoveMode;		//NPC �̵����

	int					m_nRandomWalk;
	float				m_fTrackPeriod;		// �� Ʈ���� �÷��� �ð� 
	float				m_fStackDeltaTime;	// �ִϸ��̼� �÷��� �� �ð� ���
	D3DXVECTOR3			m_vFromDir;			// ó�� �ٶ󺸴� ����
	D3DXVECTOR3			m_vToDir;			// ����Ǵ� ����

	//Ư�� ���� ���� ����
	bool				m_isYes;
	bool				m_isNo;
	bool				m_isSit;
	bool				m_isSleep;

	//NPC ��ȭ&����Ʈ ���� ����
	cTextBox*			m_pUITextBox;
	bool				m_canTalk;			//ĳ���� �������� �ȿ� �������� true
	bool				m_isLookAt;			//��ȭ���۽� NPC ��������
	bool				m_isEndTalk;		//��ȭ�������� NPC ��������
	bool				m_isSmoothTurn;		//�ε巴�� ȸ��
	bool				m_isShowTalkUI;		//��ȭ UI ��������
	float				m_fActionTime;		//������ ����Ǵ� �ð�
	float				m_fPassedTime;		//������ �ð�

	/*
	//NPC ���¸� ������ bool ������
	m_isMoving		: m_isMoving�� m_isOnTheGround�� ������ bool���� true�� �ƴ� ��� true
	m_isYes, m_isNo	 : �ִϸ��̼� 1�� ����Ǵ� ���� ����
	m_isSit, m_isSleep : �ٸ� ������ ���ϱ� ������ true
	*/

	//����� ���� ���� ����
	bool				m_isOnKey4;
	LPD3DXFONT			m_pFont;
	LPRECT				m_pRect;
	LPD3DXMESH			m_pSphereRange;

	//NPC ��ȭ Ŭ���� ����
	SYNTHESIZE(bool, m_isClear, IsClear);

	//NPC �浹 ���� ����
	SYNTHESIZE(bool, m_isCollision, IsCollision);
public:
	cNPCController();
	~cNPCController();

	void Setup(D3DXVECTOR3* pos, float fRotY, DWORD mode);
	void Update(D3DXVECTOR3* pos);
	void Render();

	//Get�Լ�
	D3DXVECTOR3* GetPosition() { return &m_vPosition; }
	D3DXMATRIXA16* GetMatRotY() { return &m_matRotY; }		//�߰�
	D3DXMATRIXA16* GetWorldTM() { return &m_matWorld; }
	DWORD GetCharacterState(){ return m_dwState; }
	bool GetIsMoving(){ return m_isMoving; }	//�Ⱦ���
	bool GetIsLookAt(){ return m_isLookAt; }
	bool GetIsCanTalk(){ return m_canTalk; }

	//Set�Լ�
	void SetMap(iMap* pMap){ m_pMap = pMap; };
	void SetTrackPeriod(float period){ m_fTrackPeriod = period; }
	void SetMoveDistance(float dis){ m_fMoveDistance = dis; }	//�̵��Ÿ� ���� �Լ�

	//NPC �̵���� ���� �Լ�
	void NotMove();
	void MoveOneway();
	void MoveSurround();

	//NPC�� ĳ���� �������� �ȿ� �ִ��� Ȯ���ϴ� �Լ�
	bool IsInRecognizeRange();

	//NPC ��ȭ&����Ʈ ���� �Լ�
	void SetTalkTextBox(string Name, string Dialouge, bool isDialouge); //��ȭUI �����Լ�
	void LookAt(D3DXVECTOR3* vDir);	//NPC�� ĳ���� �ٶ�
	void SmoothTurn(OUT D3DXVECTOR3* vDir, D3DXVECTOR3* vFrom, D3DXVECTOR3* vTo);	//�ε巴�� ȸ�� ����
	void Talk();	//��ȭUI Update

	//NPC ���� ���� �Լ�: �ִϸ��̼� ����
	void ManageNPCState();

	//Ư�� ���� ���� �Լ�
	void Idle();
	void Yes();
	void No();
	void Sit();
	void Sleep();

	//����� ���� �Լ�
	void DebugRender_NPC();
};

