#include "stdafx.h"
#include "cMonsterController.h"
#include "cLoadMap.h"
#include "cAStar.h"
#include "cCube.h"
#include "cMonsterHPBar.h"

cMonsterController::cMonsterController()
	: m_vPosition(0, 0, 0)
	, m_vFirstPosition(0, 0, 0)
	, m_vCharacterPosition(0, 0, 0)
	, m_isMoving(false)
	, m_isStart(false)
	, m_isDiePlay(true)
	, m_fMoveSpeed(SPEED_WALK)
	, m_fRotY(0)
	, m_vDir(0, 0, -1)
	, m_vDest(0, 0, 0)
	, m_pMap(NULL)
	//A* ���� ���� �ʱ�ȭ
	, m_pAStar(NULL)
	, m_isNeededAStar(false)
	, m_isAStarStart(false)
	, m_isAStarGoHome(false)
	, m_isDebugMode(false)	//A* ������ ����� ����
	//���� ���� ���� ����
	, m_dwState(E_MONSTER_SLEEP)
	, m_dwCharacterState(E_ON_THE_GROUND)
	, m_pHPBar(NULL)
	, m_fTrackPeriod(0.0f)
	, m_fStackDeltaTime(0.0f)
	, m_isResetStackDeltaTime(false)
	//�������� ���� �ʱ�ȭ
	, m_fHeight(0)
	, m_fJumpPower(POWER_JUMP)
	, m_fGravity(POWER_GRAVITY)
	, m_fFallSpeed(0)
	, m_isJump(false)
	, m_isOnTheGround(true)
	//���� ���� ����
	, m_isAttack(false)
	, m_isCharge(false)
	, m_isReady(false)
	, m_isHit(false)
	, m_isHitBySnowBall(false)
	, m_nAttackPower(10)
	, m_nRandomAttack(0)
	//Ư�� ���� ���� ����
	, m_isSit(false)
	, m_isSleep(true)
	, m_isEmotion(false)
	, m_isWakeUp(false)
	, m_isForget(false)
	, m_isForgetOn(false)
	//����� ���� ���� ����
	, m_isOnKey3(false)
	//���� �浹 ���� ����
	, m_isCollision(false)
{
	D3DXMatrixIdentity(&m_matRotY);
	D3DXMatrixIdentity(&m_matWorld);

	//���� HP ���� �ʱ�ȭ
	m_stHp.m_fMaxHp = MAX_HP;
	m_stHp.m_fHp = MAX_HP;
	m_stHp.m_isDeath = false;

	//A* ���� ���� ���� �ʱ�ȭ
	m_vecTriGrid.clear();
	m_vecVertex.clear();
	m_vecIndex.clear();
	m_vecAStarDest.clear();
	m_vecObstcles.clear();

	//���� �νĹ��� ���� ���� �ʱ�ȭ
	D3DXCreateSphere(g_pD3DDevice, DISTANCE_RECOGNIZE, 10, 10, &m_pSphereRangeMeshRec, NULL);
	D3DXCreateSphere(g_pD3DDevice, DISTANCE_ATTACK, 10, 10, &m_pSphereRangeMeshAtk, NULL);
	D3DXCreateSphere(g_pD3DDevice, DISTANCE_CHARGE, 10, 10, &m_pSphereRangeMeshCharge, NULL);

	//���� ���� ����� �ؽ�Ʈ ��� ����
	m_pRect = new RECT;
	m_pRect->left = WINSIZEX - 400;
	m_pRect->top = 150;
	m_pRect->right = WINSIZEX;
	m_pRect->bottom = 210;

	D3DXCreateFont(g_pD3DDevice, 24, 12, 0, 1, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, false, "Arial", &m_pFont);
}


cMonsterController::~cMonsterController()
{
	SAFE_RELEASE(m_pSphereRangeMeshRec);
	SAFE_RELEASE(m_pSphereRangeMeshAtk);
	SAFE_RELEASE(m_pSphereRangeMeshCharge);
	SAFE_DELETE(m_pRect);
	SAFE_RELEASE(m_pFont);
}

//��ֹ� ���� �� �� ���� ����ȭ
void cMonsterController::Setup(D3DXVECTOR3* pos	/*NULL*/)
{
	m_vPosition = *pos;
	if (m_pMap)
		m_pMap->GetHeight(m_vPosition.x, m_vPosition.y, m_vPosition.z);

	m_vFirstPosition = m_vPosition;

	m_pHPBar = new cMonsterHPBar;
	m_pHPBar->AutoRelease();
	m_pHPBar->Setup();

	/*D3DXMATRIXA16 matS, matT, matW;

	// ��ֹ� ���� (20��)
	for (int i = 0; i < 20; ++i)
	{
		// ���̴� 30���� �����ϰ� ����, ���� ũ�⸦ �������� �����.
		D3DXMatrixScaling(&matS, rand() % 10 + 3.0f, 30.0f, rand() % 10 + 3.0f);
		// ��ġ�� �� ������ 10 �����ŭ �������� �������� ���� �Ѵ�.
		D3DXMatrixTranslation(&matT, rand() % 237 + 10.0f, 0.0f, rand() % 237 + 10.0f);
		matW = matS * matT;

		cCube* pCube = new cCube;
		pCube->Setup(&matW, NULL, NULL, NULL);

		float y;	// ���̸ʿ� ���� ���� ������ ��ֹ��� ���̸� �����. (���� ����)
		m_pMap->GetHeight(pCube->GetPCVertex()[0].p.x, y, pCube->GetPCVertex()[0].p.z);

		for (int j = 0; j < pCube->GetPCVertex().size(); ++j)
		{
			pCube->GetPCVertex()[j].p.y += y;
		}

		for (int j = 0; j < pCube->GetPCVertex().size(); ++j)
		{
			m_vecObstcles.push_back(pCube->GetPCVertex()[j].p);
		}
		SAFE_RELEASE(pCube);
	}

	m_pAStar->SetObstacles(m_vecObstcles);*/

	// ����Ʈ�ʿ� ���� ��� ������ ����ȭ ���Ѽ� �����Ѵ�.
	// ����ȭ ��Ų ����� ������ ���� ���� �������� ���� ��ŷ ��ġ������ ������ Ŀ����.
	float nSimpleX = 16;	//32
	float nSimpleZ = 16;	//32
	for (int z = 0; z < nSimpleX; ++z)
	{
		for (int x = 0; x < nSimpleZ; ++x)
		{
			// ����ȭ ��Ű�� ����� ���� ��ŭ�� �������� �ε��� ��ȣ�� ã�Ƽ� ��� ������ �����Ѵ�.
			m_vecTriGrid.push_back(m_vecVertex[(x + 0) / nSimpleX * TILE_N + (z + 0) / nSimpleZ * TILE_N * VERTEX_DIM]);
			m_vecTriGrid.push_back(m_vecVertex[(x + 0) / nSimpleX * TILE_N + (z + 1) / nSimpleZ * TILE_N * VERTEX_DIM]);
			m_vecTriGrid.push_back(m_vecVertex[(x + 1) / nSimpleX * TILE_N + (z + 0) / nSimpleZ * TILE_N * VERTEX_DIM]);

			m_vecTriGrid.push_back(m_vecVertex[(x + 1) / nSimpleX * TILE_N + (z + 0) / nSimpleZ * TILE_N * VERTEX_DIM]);
			m_vecTriGrid.push_back(m_vecVertex[(x + 0) / nSimpleX * TILE_N + (z + 1) / nSimpleZ * TILE_N * VERTEX_DIM]);
			m_vecTriGrid.push_back(m_vecVertex[(x + 1) / nSimpleX * TILE_N + (z + 1) / nSimpleZ * TILE_N * VERTEX_DIM]);
		}
	}
}

void cMonsterController::Update(D3DXVECTOR3* pos, DWORD characterState)
{
	if (g_pKeyManager->isOnceKeyDown(VK_F2))
	{
		Reset();
	}

	if (m_pHPBar)
	{
		m_pHPBar->Update();
	}

	if (IsDeath())
	{
		m_pHPBar->SetCurrentHp(0);
		m_pHPBar->SetIsDamageOnUpdate(true);
		Death();
		return;
	}

	m_vCharacterPosition = *pos;
	m_dwCharacterState = characterState;

	if (m_isResetStackDeltaTime)
	{
		m_fStackDeltaTime = 0.0f;
		m_isResetStackDeltaTime = false;
	}

	if (IsInRecognizeRange(pos))		//���Ͱ� ĳ���͸� ������ ���
	{
		m_isForgetOn = false;

		//�ɾ��־��ų� �ڰ��־��ٸ� �Ͼ�� ��ȿ �� AStar
		if (m_isSit || m_isSleep)
		{
			m_isStart = true;
			m_isWakeUp = true;
			m_fStackDeltaTime = 0.0f;
			m_isSit = false;
			m_isSleep = false;
		}
		//�̵���(���ڸ���)�̾��ų� ���־��ų� ��û�� �ǿ���־��ٸ� ��ȿ �� AStar
		else if (m_isForget || m_isAStarGoHome)	
		{										
			m_isEmotion = true;

			if (m_isForget)
			{
				m_isForget = false;
				m_isAStarGoHome = false;
				m_fStackDeltaTime = 0.0f;
			}
			else if (m_isAStarGoHome)
			{
				m_isAStarGoHome = false;
				m_fStackDeltaTime = 0.0f;
			}
		}
		else if (IsInChargeRange(pos) && !m_isWakeUp)	//ĳ���Ͱ� ������ �������� ������ ���� ���
		{
			m_isCharge = true;
		}
		else if (IsInAttackRange(pos) && !m_isWakeUp)	//ĳ���Ͱ� ������ ���ݹ��� ������ ���� ���
		{
			m_isAttack = true;
		}
	}
	else if (!IsInRecognizeRange(pos) && m_isStart)		//ĳ���Ͱ� ������ �������� �ȿ� ���� ���
	{
		m_isAStarStart = false;	//AStar ����
		m_isCharge = false;

		if (!m_isAStarGoHome)
		{
			m_vecAStarDest.clear();
			m_pAStar->ResetNodes();
			m_vDest = m_vPosition;	//�ڸ�����
		}

		//�ڸ��� ���� �� �����ð����� ������ �ִٰ� ��û�θ� �� �����ð� ���� �ٽ� ���� �ڸ��� ���ư�(AStar)
		if (!m_isForgetOn)
		{
			m_isForget = true;
			m_isForgetOn = true;
			m_fStackDeltaTime = 0.0f;
		}

		//���� �ڸ��� ���ư� �� �ڸ��� ����. �����ð� ���� ���
		if (m_vPosition == m_vFirstPosition)	//�ȸ���
		{
			m_isAStarGoHome = false;
			m_isSit = true;
		}

		if (m_isWakeUp)
		{
			m_isWakeUp = false;
			m_fStackDeltaTime = 0.0f;
		}
	}

	static float dist = 0;
	if (!m_isAttack && !m_isHit && !m_isCharge && (m_isAStarStart || m_isAStarGoHome))
	{
		// �������� ���� ��ġ������ ����� �Ÿ��� ���Ѵ�.
		if (!m_isMoving && !m_vecAStarDest.empty())
		{
			m_vDest = m_vecAStarDest.back();
			m_vecAStarDest.pop_back();

			D3DXVECTOR3 vDir;
			m_vDest.y = m_vPosition.y;
			vDir = m_vDest - m_vPosition;
			dist = D3DXVec3Length(&vDir);			// �Ÿ�

			if (dist > 0)
			{
				D3DXVec3Normalize(&m_vDir, &vDir);	// ����
				m_isMoving = true;
			}
		}

		if (dist > 0 && dist > m_fMoveSpeed * g_pTimeManager->getElapsedTime())
		{
			m_vPosition += m_vDir * m_fMoveSpeed * g_pTimeManager->getElapsedTime();
			dist -= m_fMoveSpeed * g_pTimeManager->getElapsedTime();
		}
		else
		{
			m_vPosition = m_vDest;
			m_isMoving = false;
		}
	}

	if (m_pMap)
		m_pMap->GetHeight(m_vPosition.x, m_vPosition.y, m_vPosition.z);

	D3DXMATRIXA16 matT;
	D3DXMatrixTranslation(&matT, m_vPosition.x, m_vPosition.y, m_vPosition.z);

	// �ٶ󺸴� ���⿡ ���� ���Ʈ������ �����.
	m_vDir.y = 0;
	D3DXMatrixLookAtLH(&m_matRotY, &D3DXVECTOR3(0, 0, 0), &m_vDir, &D3DXVECTOR3(0, 1, 0));
	// ȸ������ ������ ��Ʈ������ Transpose �� ��Ű�� Inverse ��Ʈ������ �ȴ�.
	D3DXMatrixTranspose(&m_matRotY, &m_matRotY);

	m_matWorld = m_matRotY * matT;


	ManageMonsterState();	//���� ���°��� �Լ�(�ִϸ��̼� ��)

	if (m_isAStarStart)
	{
		m_isEmotion = false;
		SetDestinationAStar(pos);
	}
}

void cMonsterController::Render()
{
	D3DXMATRIXA16 mat;
	D3DXMatrixIdentity(&mat);

	g_pD3DDevice->SetTransform(D3DTS_WORLD, &mat); //m_matWorld
	g_pD3DDevice->SetTexture(0, NULL);
	g_pD3DDevice->SetFVF(D3DFVF_XYZ);

	if (m_pHPBar)
		m_pHPBar->Render();

	//A* ���� ����
	/*if (m_vecObstcles.size() > 0)
	{

		g_pD3DDevice->SetMaterial(&BLUE_MTRL);
		g_pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_vecObstcles.size() / 3,
			&m_vecObstcles[0], sizeof(D3DXVECTOR3));
	}

	if (GetAsyncKeyState(VK_F2) & 0x0001)
		m_isDebugMode = !m_isDebugMode;

	if (m_isDebugMode)
	{
		g_pD3DDevice->SetMaterial(&YELLOW_MTRL);
		g_pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_vecTriGrid.size() / 3,
			&m_vecTriGrid[0], sizeof(D3DXVECTOR3));
	}*/

	//����� ����
	DebugRender_Monster();
}

//A* ������ ����
void cMonsterController::SetDestinationAStar(D3DXVECTOR3* dest)
{
	m_pAStar->ResetNodes();
	m_isNeededAStar = false;

	m_vDest = *dest;	//������ ����

	// �÷��̾��� ��ġ���� �������� ��� ���̸� �����ؼ�
	// �߰��� ��ֹ� ������ �Ǵ��Ѵ�.
	cRay ray;
	ray.m_vOrg = m_vPosition;
	ray.m_vDir = m_vDest - m_vPosition;

	// ������ ���� �� ���� ��ֹ��� �ִ��� ������ ���⿡ �ִٸ�
	// ���� �浹�� �Ͼ�� ������ �÷���� ��ֹ� ���� �Ÿ��� ����ؼ�
	// ��ֹ��� �÷��̾�� ������ ���̿� �ִ� ��ֹ����� �Ǻ��Ѵ�.
	float length = D3DXVec3Length(&ray.m_vDir);

	D3DXVec3Normalize(&ray.m_vDir, &ray.m_vDir);
	// ���� �� ���̸� ���� ��ֹ����� �浹 �˻�
	for (int i = 0; i < m_vecObstcles.size(); i += 3)
	{
		float f;
		if (D3DXIntersectTri(&m_vecObstcles[i], &m_vecObstcles[i + 1],
			&m_vecObstcles[i + 2], &ray.m_vOrg, &ray.m_vDir, NULL, NULL, &f)
			&& f < length)
		{
			m_isNeededAStar = true;
			break;
		}
	}

	if (m_isNeededAStar)
	{
		assert(m_pAStar && "cMonsterController.cpp: A* ����");

		int nStartIndex = m_pAStar->FindClosestNode(m_vPosition);
		int nEndIndex = m_pAStar->FindClosestNode(m_vDest);

		m_isMoving = false;
		m_vecAStarDest.clear();
		m_vecAStarDest.push_back(m_vDest);
		m_pAStar->FindAStarPath(nStartIndex, nEndIndex, m_vecAStarDest);
		m_pAStar->MakeDirectPath(m_vPosition, m_vDest, m_vecAStarDest);
	}
	else
	{
		m_isMoving = false;
		m_vecAStarDest.clear();
		m_vecAStarDest.push_back(m_vDest);
	}
}

//�������� ���� �����ϴ��� �Ǵ��ϴ� �Լ�
bool cMonsterController::IsInRecognizeRange(D3DXVECTOR3* pos)
{
	D3DXVECTOR3 vDis = m_vPosition - *pos;
	float fDis = D3DXVec3Length(&vDis);

	if (fDis <= DISTANCE_RECOGNIZE + D3DX_16F_EPSILON)
	{
		if (m_vCharacterPosition.y < 5.0f)
			return false;

		return true;
	}

	return false;
}

//���ݹ��� ���� �����ϴ��� �Ǵ��ϴ� �Լ�
bool cMonsterController::IsInAttackRange(D3DXVECTOR3* pos)
{
	D3DXVECTOR3 vDis = m_vPosition - *pos;
	float fDis = D3DXVec3Length(&vDis);

	if (fDis <= DISTANCE_ATTACK + D3DX_16F_EPSILON)
	{
		if (m_vCharacterPosition.y < 5.0f || m_isCharge || m_isHit)
		{
			return false;
		}
		
		return true;
	}

	return false;
}

//�������� ���� �����ϴ��� �Ǵ��ϴ� �Լ�
bool cMonsterController::IsInChargeRange(D3DXVECTOR3* pos)
{
	D3DXVECTOR3 vDis = m_vPosition - *pos;
	float fDis = D3DXVec3Length(&vDis);

	if (fDis <= DISTANCE_CHARGE + D3DX_16F_EPSILON && fDis > DISTANCE_ATTACK + D3DX_16F_EPSILON)
	{
		if (m_vCharacterPosition.y < 5.0f || m_isReady)
		{
			m_fStackDeltaTime = 0.0f;
			return false;
		}
		
		return true;
	}

	return false;
}

//pMap ���� �Լ�(Set)
void cMonsterController::SetMap(iMap* pMap)
{
	m_pMap = pMap;
	m_vecVertex = ((cLoadMap*)pMap)->GetVertex();

	m_vecIndex = ((cLoadMap*)pMap)->GetIndex();

	//���� ó�� ��ġ������ ���̰� ����
	if (m_pMap)
		m_pMap->GetHeight(m_vFirstPosition.x, m_vFirstPosition.y, m_vFirstPosition.z);
};

//���� ���� ���� �Լ�: �ִϸ��̼� ����
void cMonsterController::ManageMonsterState()
{
	if (m_isHit)
	{
		//����
		if (m_dwState == E_MONSTER_ATTACK1 || m_dwState == E_MONSTER_ATTACK2 || m_dwState == E_MONSTER_ATTACK3
			|| m_dwState == E_MONSTER_RUN_FRONT && !m_isHitBySnowBall)
		{
			m_isHit = false;
		}
		else Hit();
	}
	else if (m_isAttack)
	{
		if (m_nRandomAttack == 0)
			Attack1();
		else if (m_nRandomAttack == 1)
			Attack2();
		else if (m_nRandomAttack ==2)
			Attack3();
	}
	else if (m_isCharge)
	{
		ChargeAttack();
	}
	//�ٲ���ġ
	else if (m_isSit)
	{
		Sit();
	}
	else if (m_isSleep)
	{
		m_dwState = E_MONSTER_SLEEP;
	}
	//
	else if (m_isWakeUp)
	{
		WakeUpEmotion();
	}
	else if (m_isEmotion)
	{
		Emotion();
	}
	else if (m_isForget)
	{
		ForgetAndGoHome();
	}
	//���� ��ġ
	//
	else if (m_isMoving)
	{
		m_dwState = E_MONSTER_WALK;
		m_isEmotion = false;
	}
	else if (!m_isMoving && m_isOnTheGround)
	{
		m_dwState = E_MONSTER_ON_THE_GROUND;
	}
}

//���� ���� �Լ�
void cMonsterController::Attack1()
{
	if (!m_isAttack) return;

	float fDeltaTime = g_pDeltaTimeManager->GetDeltaTime();
	static int playSoundCntAttack1 = 0;

	//���� ����
	D3DXVECTOR3 vDir;
	vDir = m_vCharacterPosition - m_vPosition;

	if (m_dwCharacterState == E_JUMP_UP)
		vDir.y = 0;
	if (m_vCharacterPosition != m_vPosition)
		D3DXVec3Normalize(&m_vDir, &vDir);

	if (m_fStackDeltaTime < 0.5)
	{
		m_fStackDeltaTime += fDeltaTime;
		m_isAttack = true;
		m_dwState = E_MONSTER_ATTACK1;

		//���� �߰�
		if (playSoundCntAttack1 == 0 && !g_pSoundManager->isPlaySound("monster_attack"))
		{
			g_pSoundManager->play("monster_attack", 1.0f);
			++playSoundCntAttack1;
		}
	}
	else if (m_fStackDeltaTime < 2.0)
	{
		m_fStackDeltaTime += fDeltaTime;
		m_isAttack = true;
		m_dwState = E_MONSTER_IDLE;

		//����
		if (m_isHit)
		{
			m_fStackDeltaTime = 0.0f;
			m_isAttack = false;
			m_nRandomAttack = rand() % 3;
			playSoundCntAttack1 = 0;
			g_pSoundManager->stop("monster_attack");
			return;
		}
	}
	else
	{
		m_fStackDeltaTime = 0.0f;
		m_isAttack = false;
		m_nRandomAttack = rand() % 3;
		playSoundCntAttack1 = 0;
		g_pSoundManager->stop("monster_attack");
	}
}

void cMonsterController::Attack2()
{
	if (!m_isAttack) return;

	float fDeltaTime = g_pDeltaTimeManager->GetDeltaTime();
	static int playSoundCntAttack2 = 0;

	//���� ����
	D3DXVECTOR3 vDir;
	vDir = m_vCharacterPosition - m_vPosition;

	if (m_dwCharacterState == E_JUMP_UP)
		vDir.y = 0;

	if (m_vCharacterPosition != m_vPosition)
		D3DXVec3Normalize(&m_vDir, &vDir);

	if (m_fStackDeltaTime < 0.4)
	{
		m_fStackDeltaTime += fDeltaTime;
		m_isAttack = true;
		m_dwState = E_MONSTER_ATTACK2;

		//���� �߰�
		if (playSoundCntAttack2 == 0 && !g_pSoundManager->isPlaySound("monster_attack"))
		{
			g_pSoundManager->play("monster_attack", 1.0f);
			++playSoundCntAttack2;
		}
	}
	else if (m_fStackDeltaTime < 2.2)
	{
		m_fStackDeltaTime += fDeltaTime;
		m_isAttack = true;
		m_dwState = E_MONSTER_IDLE;

		//����
		if (m_isHit)
		{
			m_fStackDeltaTime = 0.0f;
			m_isAttack = false;
			m_isHit = true;
			m_nRandomAttack = rand() % 3;
			playSoundCntAttack2 = 0;
			g_pSoundManager->stop("monster_attack");
			return;
		}
	}
	else
	{
		m_fStackDeltaTime = 0.0f;
		m_isAttack = false;
		m_nRandomAttack = rand() % 3;
		playSoundCntAttack2 = 0;
		g_pSoundManager->stop("monster_attack");
	}
}

void cMonsterController::Attack3()
{
	if (!m_isAttack) return;

	float fDeltaTime = g_pDeltaTimeManager->GetDeltaTime();
	static int playSoundCntAttack3 = 0;

	//���� ����
	D3DXVECTOR3 vDir;
	vDir = m_vCharacterPosition - m_vPosition;

	if (m_dwCharacterState == E_JUMP_UP)
		vDir.y = 0;

	if (m_vCharacterPosition != m_vPosition)
		D3DXVec3Normalize(&m_vDir, &vDir);

	if (m_fStackDeltaTime < 0.6)
	{
		m_fStackDeltaTime += fDeltaTime;
		m_isAttack = true;
		m_dwState = E_MONSTER_ATTACK3;

		//���� �߰�
		if (playSoundCntAttack3 == 0 && !g_pSoundManager->isPlaySound("monster_attack"))
		{
			g_pSoundManager->play("monster_attack", 1.0f);
			++playSoundCntAttack3;
		}
	}
	else if (m_fStackDeltaTime < 2.5)
	{
		m_fStackDeltaTime += fDeltaTime;
		m_isAttack = true;
		m_dwState = E_MONSTER_IDLE;

		//����
		if (m_isHit)
		{
			m_fStackDeltaTime = 0.0f;
			m_isAttack = false;
			m_isHit = true;
			m_nRandomAttack = rand() % 3;
			playSoundCntAttack3 = 0;
			g_pSoundManager->stop("monster_attack");
			return;
		}
	}
	else
	{
		m_fStackDeltaTime = 0.0f;
		m_isAttack = false;
		m_nRandomAttack = rand() % 3;
		playSoundCntAttack3 = 0;
		g_pSoundManager->stop("monster_attack");
	}
}

//���� ���� �Լ�
void cMonsterController::ChargeAttack()
{
	if (!m_isCharge) return;
	static int playSoundCntCharge = 0;

	if (!IsInChargeRange(&m_vCharacterPosition) && m_dwState == E_ON_THE_GROUND)
	{
		m_fStackDeltaTime = 0.0f;
		m_isCharge = false;
		m_isReady = false;
		return;
	}

	float fDeltaTime = g_pDeltaTimeManager->GetDeltaTime();

	static D3DXVECTOR3 vDestPos(0, 0, 0);

	if (m_fStackDeltaTime < 1.0)
	{
		m_fStackDeltaTime += fDeltaTime;
		m_isCharge = true;
		m_isReady = false;
		m_dwState = E_MONSTER_READY;

		D3DXVECTOR3 vDir;
		vDir = m_vCharacterPosition - m_vPosition;
		
		if (m_dwCharacterState == E_JUMP_UP)
			vDir.y = 0;
		if (m_vCharacterPosition != m_vPosition)
			D3DXVec3Normalize(&m_vDir, &vDir);

		vDestPos = m_vPosition;
	}
	else if (m_fStackDeltaTime < 2.3)
	{
		m_fStackDeltaTime += fDeltaTime;
		m_isCharge = true;
		m_isReady = false;
		m_dwState = E_MONSTER_RUN_FRONT;

		D3DXVECTOR3 vDir;
		vDir = m_vDir;
		m_vPosition += m_vDir * SPEED_CHARGE * g_pTimeManager->getElapsedTime();

		//���� �߰�
		if (playSoundCntCharge == 0 && !g_pSoundManager->isPlaySound("monster_attack"))
		{
			g_pSoundManager->play("monster_attack", 1.0f);
			++playSoundCntCharge;
		}
	}
	else if (m_fStackDeltaTime < 4.3f)
	{
		m_fStackDeltaTime += fDeltaTime;
		m_dwState = E_MONSTER_ON_THE_GROUND;
		m_isReady = true;
		playSoundCntCharge = 0;
		g_pSoundManager->stop("monster_attack");
	}
	else
	{
		m_fStackDeltaTime = 0.0f;
		m_isCharge = false;
		m_isReady = true;
		m_dwState = E_MONSTER_ON_THE_GROUND;
		m_nRandomAttack = rand() % 3;
		playSoundCntCharge = 0;
		g_pSoundManager->stop("monster_attack");
	}
}

void cMonsterController::Hit()
{
	if (!m_isHit) return;

	float fDeltaTime = g_pDeltaTimeManager->GetDeltaTime();
	static int playSoundCntHit = 0;

	if (m_fStackDeltaTime < 0.5f)	//0.5
	{
		m_fStackDeltaTime += fDeltaTime;
		m_isHit = true;

		if (m_isHitBySnowBall)
			m_dwState = E_MONSTER_HIT2;	 //E_MONSTER_HIT2: ����ġ�� �¾�����
		else
			m_dwState = E_MONSTER_HIT1;

		//���� �߰�
		if (playSoundCntHit == 0 && !g_pSoundManager->isPlaySound("hit"))
		{
			g_pSoundManager->play("hit", 1.0f);
			++playSoundCntHit;
			m_pHPBar->DamagedOn();
		}
	}
	else
	{
		m_fStackDeltaTime = 0.0f;
		m_isHit = false;
		m_isHitBySnowBall = false;
		m_stHp.m_fHp -= 10.0f;	//������
		playSoundCntHit = 0;
		g_pSoundManager->stop("hit");
	}
}

void cMonsterController::DamageOn()
{
	m_pHPBar->DamagedOn();
	m_stHp.m_fHp -= 10.0f;	//������
}

//Ư�� ���� ���� �Լ�
//�ɾ��ִ� �����ð� �� �ڱ�
void cMonsterController::Sit()
{
	if (!m_isSit && !m_isSleep) return;

	float fDeltaTime = g_pDeltaTimeManager->GetDeltaTime();

	if (m_fStackDeltaTime < 0.3f)
	{
		m_fStackDeltaTime += fDeltaTime;
		m_dwState = E_MONSTER_SIT_START;
	}
	else if (m_fStackDeltaTime < 8.0)
	{
		m_fStackDeltaTime += fDeltaTime;
		m_dwState = E_MONSTER_SIT_LOOP;
	}
	else if (m_fStackDeltaTime < 8.5)
	{
		m_fStackDeltaTime += fDeltaTime;
		m_dwState = E_MONSTER_SIT_SLEEP;
	}
	else
	{
		m_isSit = false;
		m_isSleep = true;
		m_dwState = E_MONSTER_SLEEP;
	}
	/*if (m_isWakeUp)
	{
		m_fStackDeltaTime = 0.0f;
		m_isSit = false;
		m_isSleep = false;
	}*/
}

//�Ͼ�� ��ȿ�� �� ĳ���� �Ѿư���
void cMonsterController::WakeUpEmotion()
{
	if (!m_isWakeUp) return;

	float fDeltaTime = g_pDeltaTimeManager->GetDeltaTime();
	static int playSoundCntWakeUp = 0;

	if (m_fStackDeltaTime < 0.5)
	{
		m_fStackDeltaTime += fDeltaTime;
		m_isWakeUp = true;
		m_dwState = E_MONSTER_SIT_LOOP;
	}
	else if (m_fStackDeltaTime < 1.0)
	{
		m_fStackDeltaTime += fDeltaTime;
		m_isWakeUp = true;
		m_dwState = E_MONSTER_STAND_UP;
	}
	else if (m_fStackDeltaTime < 3.0)
	{
		m_fStackDeltaTime += fDeltaTime;
		m_isWakeUp = true;
		m_dwState = E_MONSTER_EMO3;

		if (playSoundCntWakeUp == 0 && !g_pSoundManager->isPlaySound("monster_scream"))
		{
			g_pSoundManager->play("monster_scream", 1.0f);
			++playSoundCntWakeUp;
		}
	}
	else
	{
		m_fStackDeltaTime = 0.0f;
		m_isWakeUp = false;
		m_isAStarStart = true;
		playSoundCntWakeUp = 0;
		g_pSoundManager->stop("monster_scream");
	}
}

//��ȿ�ϱ�
void cMonsterController::Emotion()
{
	if (!m_isEmotion) return;

	float fDeltaTime = g_pDeltaTimeManager->GetDeltaTime();
	static int playSoundCntEmotion = 0;

	if (m_fStackDeltaTime < 2.0)
	{
		m_fStackDeltaTime += fDeltaTime;
		m_isEmotion = true;
		m_dwState = E_MONSTER_EMO1;

		if (playSoundCntEmotion == 0 && !g_pSoundManager->isPlaySound("monster_scream"))
		{
			g_pSoundManager->play("monster_scream", 1.0f);
			++playSoundCntEmotion;
		}
	}
	else
	{
		m_fStackDeltaTime = 0.0f;
		m_isEmotion = false;
		m_isAStarStart = true;
		playSoundCntEmotion = 0;
		g_pSoundManager->stop("monster_scream");
	}
}

//�������ִٰ� ��ȿ ��, �����ϰ� ���ڸ��� ���ư���
void cMonsterController::ForgetAndGoHome()
{
	if (!m_isForget) return;
	if (m_isEmotion) return;

	float fDeltaTime = g_pDeltaTimeManager->GetDeltaTime();
	static int playSoundCntGoHome = 0;

	if (m_fStackDeltaTime < 4.0)
	{
		m_fStackDeltaTime += fDeltaTime;
		m_isForget = true;
		m_dwState = E_MONSTER_IDLE;
	}
	else if (m_fStackDeltaTime < 6.0)
	{
		m_fStackDeltaTime += fDeltaTime;
		m_isForget = true;
		m_dwState = E_MONSTER_EMO4;

		if (playSoundCntGoHome == 0 && !g_pSoundManager->isPlaySound("monster_scream"))
		{
			g_pSoundManager->play("monster_scream", 1.0f);
			++playSoundCntGoHome;
		}
	}
	else if (m_fStackDeltaTime < 8.0)
	{
		m_fStackDeltaTime += fDeltaTime;
		m_isForget = true;
		m_dwState = E_MONSTER_IDLE2;
	}
	else
	{
		m_fStackDeltaTime = 0.0f;
		m_isForget = false;
		playSoundCntGoHome = 0;
		g_pSoundManager->stop("monster_scream");

		m_isAStarGoHome = true;
		SetDestinationAStar(&m_vFirstPosition);	//ó���ڸ��� ���ư�
	}
}

//���� ���� �Լ�
bool cMonsterController::IsDeath()
{
	static int playSoundCntDeath = 0;

	if (m_stHp.m_fHp <= 0)
	{
		m_stHp.m_isDeath = true;

		if (playSoundCntDeath == 0 && !g_pSoundManager->isPlaySound("monster_scream"))
		{
			g_pSoundManager->play("monster_scream", 1.0f);
			++playSoundCntDeath;
		}
	}
	else
	{
		m_stHp.m_isDeath = false;

		if (playSoundCntDeath > 0 && !g_pSoundManager->isPlaySound("monster_scream"))
		{
			playSoundCntDeath = 0;
			g_pSoundManager->stop("monster_scream");
		}
	}

	return m_stHp.m_isDeath;
}

void cMonsterController::Death()
{
	if (!IsDeath()) return;
	if (!m_isDiePlay) return;

	//m_dwState = E_MONSTER_DEATH3;

	float fDeltaTime = g_pDeltaTimeManager->GetDeltaTime();

	if (m_fStackDeltaTime < 1.5)
	{
		m_fStackDeltaTime += fDeltaTime;
		m_isDiePlay = true;
		m_dwState = E_MONSTER_DEATH3;
	}
	else
	{
		m_fStackDeltaTime = 0.0f;
		m_isDiePlay = false;
	}
}

//���� ���� ���� �Լ�
void cMonsterController::Reset()
{
	m_vPosition = m_vFirstPosition;
	m_vCharacterPosition = D3DXVECTOR3(0, 0, 0);
	m_isMoving = false;
	m_isStart = false;
	m_isDiePlay = true;
	m_fMoveSpeed = SPEED_WALK;
	m_fRotY = 0;
	m_vDir = D3DXVECTOR3(0, 0, -1);
	m_vDest = D3DXVECTOR3(0, 0, 0);
	//A* ���� ���� �ʱ�ȭ
	m_isNeededAStar = false;
	m_isAStarStart = false;
	m_isAStarGoHome = false;
	//���� ���� ���� ����
	m_dwState = E_MONSTER_SLEEP;
	m_fTrackPeriod = 0.0f;
	m_fStackDeltaTime = 0.0f;
	m_isResetStackDeltaTime = false;
	//�������� ���� �ʱ�ȭ
	m_fFallSpeed = 0;
	m_isJump = false;
	m_isOnTheGround = true;
	//���� ���� ����
	m_isAttack = false;
	m_isCharge = false;
	m_isReady = false;
	m_isHit = false;
	m_nRandomAttack = 0;
	//Ư�� ���� ���� ����
	m_isSit = false;
	m_isSleep = true;
	m_isEmotion = false;
	m_isWakeUp = false;
	m_isForget = false;
	m_isForgetOn = false;
	m_isCollision = false;

	D3DXMatrixIdentity(&m_matRotY);
	D3DXMatrixIdentity(&m_matWorld);

	//���� HP ���� �ʱ�ȭ
	m_stHp.m_fMaxHp = MAX_HP;
	m_stHp.m_fHp = MAX_HP;
	m_stHp.m_isDeath = false;

	m_pHPBar->SetCurrentHp(HPENDX);
	m_pHPBar->SetFullHP();
	m_pHPBar->SetIsDeath(false);
}

//���� ���� ����� ���� �Լ�
void cMonsterController::DebugRender_Monster()
{
	if (g_pKeyManager->isOnceKeyDown('3'))
	{
		m_isOnKey3 = !m_isOnKey3;
	}

	if (m_isOnKey3)
	{
		char szStr[1024];
		switch (m_dwState)
		{
		case E_MONSTER_ON_THE_GROUND:
			sprintf(szStr, "state: E_MONSTER_ON_THE_GROUND");
			break;
		case E_MONSTER_IDLE:
			sprintf(szStr, "state: E_MONSTER_IDLE");
			break;
		case E_MONSTER_IDLE2:
			sprintf(szStr, "state: E_MONSTER_IDLE2");
			break;
		case E_MONSTER_WALK:
			sprintf(szStr, "state: E_MONSTER_WALK");
			break;
		case E_MONSTER_RUN_FRONT:
			sprintf(szStr, "state: E_MONSTER_RUN_FRONT");
			break;
		case E_MONSTER_RUN2:
			sprintf(szStr, "state: E_MONSTER_RUN2");
			break;
		case E_MONSTER_JUMP_UP:
			sprintf(szStr, "state: E_MONSTER_JUMP_UP");
			break;
		case E_MONSTER_JUMP_DOWN:
			sprintf(szStr, "state: E_MONSTER_JUMP_DOWN");
			break;
		case E_MONSTER_AIR:
			sprintf(szStr, "state: E_MONSTER_AIR");
			break;
		case E_MONSTER_EMO1:
			sprintf(szStr, "state: E_MONSTER_EMO1");
			break;
		case E_MONSTER_EMO2:
			sprintf(szStr, "state: E_MONSTER_EMO2");
			break;
		case E_MONSTER_EMO3:
			sprintf(szStr, "state: E_MONSTER_EMO3");
			break;
		case E_MONSTER_EMO4:
			sprintf(szStr, "state: E_MONSTER_EMO4");
			break;
		case E_MONSTER_EMO5:
			sprintf(szStr, "state: E_MONSTER_EMO5");
			break;
		case E_MONSTER_EMO6:
			sprintf(szStr, "state: E_MONSTER_EMO6");
			break;
		case E_MONSTER_READY:
			sprintf(szStr, "state: E_MONSTER_READY");
			break;
		case E_MONSTER_HIT1:
			sprintf(szStr, "state: E_MONSTER_HIT1");
			break;
		case E_MONSTER_HIT2:
			sprintf(szStr, "state: E_MONSTER_HIT2");
			break;
		case E_MONSTER_HIT3:
			sprintf(szStr, "state: E_MONSTER_HIT3");
			break;
		case E_MONSTER_ATTACK1:
			sprintf(szStr, "state: E_MONSTER_ATTACK1");
			break;
		case E_MONSTER_ATTACK2:
			sprintf(szStr, "state: E_MONSTER_ATTACK2");
			break;
		case E_MONSTER_ATTACK3:
			sprintf(szStr, "state: E_MONSTER_ATTACK3");
			break;
		case E_MONSTER_DEATH1:
			sprintf(szStr, "state: E_MONSTER_DEATH1");
			break;
		case E_MONSTER_DEATH2:
			sprintf(szStr, "state: E_MONSTER_DEATH2");
			break;
		case E_MONSTER_DEATH3:
			sprintf(szStr, "state: E_MONSTER_DEATH3");
			break;
		case E_MONSTER_SIT_START:
			sprintf(szStr, "state: E_MONSTER_SIT_START");
			break;
		case E_MONSTER_SIT_LOOP:
			sprintf(szStr, "state: E_MONSTER_SIT_LOOP");
			break;
		case E_MONSTER_SIT_SLEEP:
			sprintf(szStr, "state: E_MONSTER_SIT_SLEEP");
			break;
		case E_MONSTER_SLEEP:
			sprintf(szStr, "state: E_MONSTER_SLEEP");
			break;
		case E_MONSTER_SLEEP_SIT:
			sprintf(szStr, "state: E_MONSTER_SLEEP_SIT");
			break;
		case E_MONSTER_STAND_UP:
			sprintf(szStr, "state: E_MONSTER_STAND_UP");
			break;
		}

		char szStr2[1024];
		sprintf(szStr2, "\nHp: %d\/%d\nX: %f\nY: %f\nZ: %f\nspeed: %f", m_stHp.m_fHp, m_stHp.m_fMaxHp, m_vPosition.x, m_vPosition.y, m_vPosition.z, m_fMoveSpeed);

		strcat(szStr, szStr2);

		if (m_isCollision)
			sprintf(szStr2, "\nisCollision: true");
		else sprintf(szStr2, "\nisCollision: false");

		strcat(szStr, szStr2);

		m_pFont->DrawTextA(NULL, szStr, strlen(szStr),
			m_pRect, DT_TOP | DT_LEFT | DT_NOCLIP, D3DCOLOR_XRGB(255, 0, 0));

		/////////////////////////////////////////////////////////////////////////
		//���� �νĹ��� ����
		g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		g_pD3DDevice->SetTexture(0, NULL);

		D3DXMATRIXA16 matW;
		D3DXMatrixIdentity(&matW);
		D3DXMatrixTranslation(&matW, m_vPosition.x, m_vPosition.y, m_vPosition.z);
		g_pD3DDevice->SetTransform(D3DTS_WORLD, &matW);
		g_pD3DDevice->SetMaterial(&WHITE_MTRL);
		m_pSphereRangeMeshRec->DrawSubset(0);

		D3DXMatrixTranslation(&matW, m_vPosition.x, m_vPosition.y, m_vPosition.z);
		g_pD3DDevice->SetTransform(D3DTS_WORLD, &matW);
		g_pD3DDevice->SetMaterial(&RED_MTRL);
		m_pSphereRangeMeshAtk->DrawSubset(0);

		D3DXMatrixTranslation(&matW, m_vPosition.x, m_vPosition.y, m_vPosition.z);
		g_pD3DDevice->SetTransform(D3DTS_WORLD, &matW);
		g_pD3DDevice->SetMaterial(&GREEN_MTRL);
		m_pSphereRangeMeshCharge->DrawSubset(0);

		g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}
}