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
	cMonsterController*		m_pMonsterController;	//���� ��Ʈ�ѷ�
	cSkinnedMesh*			m_pSkinnedCharacter;	//ĳ����
	cSkinnedMesh*			m_pSkinnedMonster;		//����

	/*===================================================================================*/
	/*                                 New class, This Area                              */
	/*===================================================================================*/
	iMap*					m_pMap;
	cSkyBox*				m_pSkyBox;

	//�ε��
	cLoadMap*				m_pLoadMap;

	//�ε��� + ������
	cLoadingScene*			m_pLoadingScene;

	//�ĵ�
	cSinWave*				m_pSinWave;
	cWaveWall*				m_pWaveWall;

	//�Ȱ�ȿ��
	ID3DXEffect*			m_FogEffect;
	D3DXHANDLE				m_FogTechHandle;

	//FX
	cParticle_Snow*			m_pP_Snow;
	cParticle_Walk*			m_pP_Walk;
	vector<cSemicircleFX*>	m_vecSemicircleFX;
	cTeleporter*			m_pTeleporter;
	vector<cTornado*>		m_vecTornadoBarrier;
	vector<cTornado*>		m_vecTornado;

	//�޴�
	cShowMenu*				m_pShowMenu;
	cShowClear*				m_pShowClear;
	cShowYouDie*			m_pShowYouDie;
	cShowStageNum*			m_pShowStageNum;
	vector<cAchievementBox*>m_vecAchieveBox;
	/*===================================================================================*/
	
	//A* �˰��� �߰�
	cAStar*					m_pAStar;

	//�������� �ø�
	cFrustum*				m_pFrustum;

	//������ ����
	vector<cSnowBall*>		m_vecSnowBall;

	//��Ż
	BoundingBox				m_boundingBoxPortal;
	bool					m_isOpenPortal;
	int						m_nEffectSoundPorta;
	LPD3DXMESH				m_BoxMesh = 0;

	//�浹�� �ٿ�� �ڽ�
	vector<BoundingBox>		m_vecBoundingBox;

	//�浹�� �ٿ�� ��ü
	vector<ST_BUTTON>		m_vecBoundingSphereButton;

	//����� ���� ���� ����
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

	//����̵� ���� �Լ�
	void SetTornado();						//����̵� ���� �Լ�

	//��Ż ���� �Լ�
	void SetPortal();						//��Ż ���� �Լ�
	bool IsOpenPortal();					//��Ż ���ȴ��� Ȯ�� �Լ�
	void RenderOpenPortal();				//��Ż ��� �Լ�

	//�浹���� �Լ�
	void SetMapObjBoundingBox();			//�� ������Ʈ �浹���� �ٿ���ڽ� ���� �Լ�
	void SetButtonBoundingSphere();			//��ư �浹���� �ٿ����ü ���� �Լ�

	void UpdateSnowBallBoundingPosition();	//����ġ �浹���� �ٿ����ü ������ ������Ʈ �Լ�
	void UpdateTornado();					//����̵� ���� ������Ʈ �Լ�

	void CollisionCharacter_MapObj();		//ĳ���� X �� ������Ʈ �浹ó�� �Լ�
	void CollisionCharacter_Button();		//ĳ���� X ��ư �浹ó�� �Լ�
	void CollisionCharacter_Tornado();		//ĳ���� X ����̵� �浹ó�� �Լ�
	void CollisionCharacter_Monster();		//ĳ���� X ���� �浹ó�� �Լ�
	void CollisionMonster_Character();		//���� X ĳ���� �浹ó�� �Լ�
	void CollisionMonster_MapObj();			//���� X �� ������Ʈ �浹ó�� �Լ�
	void CollisionSnowBall_MapObj();		//����ġ X �� ������Ʈ �浹ó�� �Լ�
	void CollisionSnowBall_Button();		//����ġ X ��ư �浹ó�� �Լ�
	void CollisionSnowBall_Monster();		//����ġ X ���� �浹ó�� �Լ�
	void CollisionSnowBall_SnowBall();		//����ġ X ����ġ �浹ó�� �Լ�

	//����� ���� �Լ�
	void DebugRender();
	void DebugRender_Portal();				//��Ż ������Լ�
};

