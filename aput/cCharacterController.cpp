#include "stdafx.h"
#include "cCharacterController.h"
#include "cHeightMap.h"
#include "cSnowBall.h"
#include "cHPBar.h"
#include "cSnowBallUI.h"
#include "cParticle_Firework.h"

cCharacterController::cCharacterController()
	: m_vPosition(0, 0, 0)
	, m_vFirstPosition(0, 0, 0)
	, m_fGroundHeight(0)
	, m_isMoving(false)
	, m_fMoveSpeed(SPEED_WALK)
	, m_fRotY(0)
	, m_dwDirState(E_NOTMOVE)
	, m_vDir(0, 0, 1)
	, m_pMap(NULL)
	//���콺���� ���� �ʱ�ȭ
	, m_isRButtonDown(false)
	, m_fMouseSensitivity(100.0f)
	//ĳ���� ���� ���� ����
	, m_dwState(E_ON_THE_GROUND)
	, m_pHPBar(NULL)
	, m_pSnowBallUI(NULL)
	, m_fTrackPeriod(0.0f)
	, m_fStackDeltaTime(0.0f)
	//�������� ���� �ʱ�ȭ
	, m_fHeight(0)
	, m_fJumpPower(POWER_JUMP)
	, m_fGravity(POWER_GRAVITY)
	, m_fFallSpeed(0)
	, m_isJump(false)
	, m_isOnTheGround(true)
	//���� ���� ����
	, m_isAttack(false)
	, m_isThrow(false)
	, m_isHit(false)
	, m_nAttackPower(10)
	, m_nSnowBallNum(0)
	, m_nBigSnowBallNum(0)
	//Ư�� ���� ���� ����
	, m_isSpecial(false)
	, m_isYes(false)
	, m_isNo(false)
	, m_isBuild(false)
	, m_isPickObj(false)
	, m_isSit(false)
	, m_isSleep(false)
	, m_isCollisionTornado(false)
	, m_isFallByTornado(false)
	, m_isSlideSlope(false)
	, m_isCanTalk(false)
	//����Ʈ ���� ����
	, m_nCntSoundEffectDestroySnow(0)
	//Ư��ȿ�� ���� ����
	, m_pP_Firework(NULL)
	//ĳ�����浹 ���� ����
	, m_isCollision(false)
	//����� ���� ���� ����
	, m_isOnKey2(false)
{
	D3DXMatrixIdentity(&m_matRotY);
	D3DXMatrixIdentity(&m_matWorld);

	//���콺���� ���� �ʱ�ȭ
	m_ptPrevMouse.x = WINSIZEX / 2;
	m_ptPrevMouse.y = WINSIZEY / 2;

	//ĳ���� HP ���� �ʱ�ȭ
	m_stHp.m_fMaxHp = MAX_HP;
	m_stHp.m_fHp = MAX_HP;
	m_stHp.m_isDeath = false;
	
	//ĳ���Ͱ� ���� ������ ���� �ʱ�ȭ
	m_vecSnowBall.clear();

	//ĳ���� ���� ����� �ؽ�Ʈ ��� ����
	m_pRect = new RECT;
	m_pRect->left = 10;
	m_pRect->top = 150;
	m_pRect->right = 110;
	m_pRect->bottom = 210;

	D3DXCreateFont(g_pD3DDevice, 24, 12, 0, 1, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, false, "Arial", &m_pFont);
}


cCharacterController::~cCharacterController()
{
	m_vecSnowBall.clear();

	SAFE_DELETE(m_pRect);			//�߰�
	SAFE_RELEASE(m_pFont);			//�߰�

	if (m_pP_Firework)
		m_pP_Firework->Destroy();
}

void cCharacterController::Setup(D3DXVECTOR3* pos)
{
	m_vPosition = *pos;
	m_vFirstPosition = m_vPosition;

	m_pHPBar = new cHPBar;
	m_pHPBar->AutoRelease();
	m_pHPBar->Setup();

	m_pSnowBallUI = new cSnowBallUI;
	m_pSnowBallUI->AutoRelease();
	m_pSnowBallUI->Setup();

	//���� �߰� 
	AddEffectSound();
}

void cCharacterController::Update(float fRotY)
{
	if (g_pKeyManager->isOnceKeyDown(VK_F1))
	{
		m_pHPBar->SetFullHP();
		m_stHp.m_fHp = m_stHp.m_fMaxHp;
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

	//ĳ���� �⺻����
	D3DXMATRIXA16 matRotY, matT;
	D3DXVECTOR3 vUp(0, 1, 0);

	/* ȸ�� */
	//D3DXVECTOR3 vDir;
	//vDir = *vLookAt - *vEye;
	//D3DXVec3Normalize(&m_vDir, &vDir);

	D3DXVECTOR3 vDir(0, 0, 1);

	if (!m_isSit && !m_isSleep && !m_isBuild && !m_isCollisionTornado)
	{
		//ȸ�� ��Ʈ���� ����
		D3DXMatrixRotationY(&m_matRotY, fRotY);
		//D3DXMatrixRotationY(&m_matRotY, m_fRotY);


		//ĳ���� ���� ����
		D3DXVec3TransformNormal(&vDir, &vDir, &m_matRotY);
		D3DXVec3Normalize(&m_vDir, &vDir);
	}

	/* �̵� */
	D3DXVECTOR3 vPos = m_vPosition;

	//�޸���
	//if (g_pKeyManager->isStayKeyDown(VK_SHIFT))
	if (g_pKeyManager->isStayKeyDown(VK_SHIFT) ||
		(g_DI_Joystick->m_Controllers->bConnected && g_DI_Joystick->GetLSHOULDER()))
	{
		m_fMoveSpeed = SPEED_RUN;
		
		for (int i = 0; i < m_vecSnowBall.size(); ++i)
		{
			if (m_vecSnowBall[i]->GetState() == E_SNOW_BIG)
				m_vecSnowBall[i]->SetMoveSpeed(SPEED_RUN);
		}
	}
	else
	{
		m_fMoveSpeed = SPEED_WALK;

		for (int i = 0; i < m_vecSnowBall.size(); ++i)
		{
			if (m_vecSnowBall[i]->GetState() == E_SNOW_BIG)
				m_vecSnowBall[i]->SetMoveSpeed(SPEED_WALK);
		}
	}

	//���� �� �밢�� �̵�
	if (!m_isBuild && !m_isHit && !m_isPickObj && !m_isThrow && !m_isYes && !m_isNo && !m_isCollisionTornado)
	{
		//if (g_pKeyManager->isStayKeyDown('W') && g_pKeyManager->isStayKeyDown('A'))
		if ((g_pKeyManager->isStayKeyDown('W') && g_pKeyManager->isStayKeyDown('A')) ||
			(g_DI_Joystick->m_Controllers->bConnected && g_DI_Joystick->GetLYTOP() && g_DI_Joystick->GetLXLEFT()))
		{
			D3DXMatrixRotationY(&matRotY, -D3DX_PI / 4);
			D3DXVec3TransformNormal(&vDir, &m_vDir, &matRotY);
			vPos = vPos + vDir * m_fMoveSpeed * g_pTimeManager->getElapsedTime();
			m_dwDirState = E_DIAGONAL_FL;
			m_isMoving = true;

			if (m_isOnTheGround)
			{
				if (m_fMoveSpeed == SPEED_WALK)
					m_dwState = E_WALK_LEFT_45;
				else m_dwState = E_RUN_LEFT_45;
			}
		}
		//������ �� �밢�� �̵�
		//else if (g_pKeyManager->isStayKeyDown('W') && g_pKeyManager->isStayKeyDown('D'))
		else if ((g_pKeyManager->isStayKeyDown('W') && g_pKeyManager->isStayKeyDown('D')) || 
			(g_DI_Joystick->m_Controllers->bConnected && g_DI_Joystick->GetLYTOP() && g_DI_Joystick->GetLXRIGHT()))
		{
			D3DXMatrixRotationY(&matRotY, D3DX_PI / 4);
			D3DXVec3TransformNormal(&vDir, &m_vDir, &matRotY);
			vPos = vPos + vDir * m_fMoveSpeed * g_pTimeManager->getElapsedTime();
			m_dwDirState = E_DIAGONAL_FR;
			m_isMoving = true;

			if (m_isOnTheGround)
			{
				if (m_fMoveSpeed == SPEED_WALK)
					m_dwState = E_WALK_RIGHT_45;
				else m_dwState = E_RUN_RIGHT_45;
			}
		}
		//���� �Ʒ� �밢�� �̵�
		//else if (g_pKeyManager->isStayKeyDown('S') && g_pKeyManager->isStayKeyDown('A'))
		else if ((g_pKeyManager->isStayKeyDown('S') && g_pKeyManager->isStayKeyDown('A')) || 
			(g_DI_Joystick->m_Controllers->bConnected && g_DI_Joystick->GetLYBOTTOM() && g_DI_Joystick->GetLXLEFT()))
		{
			D3DXMatrixRotationY(&matRotY, 5 * D3DX_PI / 4);
			D3DXVec3TransformNormal(&vDir, &m_vDir, &matRotY);
			vPos = vPos + vDir * m_fMoveSpeed * g_pTimeManager->getElapsedTime();
			m_dwDirState = E_DIAGONAL_BL;
			m_isMoving = true;

			if (m_isOnTheGround)
			{
				if (m_fMoveSpeed == SPEED_WALK)
					m_dwState = E_WALK_LEFT_45;
				else m_dwState = E_RUN_LEFT_45;
			}
		}
		//������ �Ʒ� �밢�� �̵�
		//else if (g_pKeyManager->isStayKeyDown('S') && g_pKeyManager->isStayKeyDown('D'))
		else if ((g_pKeyManager->isStayKeyDown('S') && g_pKeyManager->isStayKeyDown('D')) || 
			(g_DI_Joystick->m_Controllers->bConnected && g_DI_Joystick->GetLYBOTTOM() && g_DI_Joystick->GetLXRIGHT()))
		{
			D3DXMatrixRotationY(&matRotY, -5 * D3DX_PI / 4);
			D3DXVec3TransformNormal(&vDir, &m_vDir, &matRotY);
			vPos = vPos + vDir * m_fMoveSpeed * g_pTimeManager->getElapsedTime();
			m_dwDirState = E_DIAGONAL_BR;
			m_isMoving = true;

			if (m_isOnTheGround)
			{
				if (m_fMoveSpeed == SPEED_WALK)
					m_dwState = E_WALK_RIGHT_45;
				else m_dwState = E_RUN_RIGHT_45;
			}
		}
		//������ �̵�
		//else if (g_pKeyManager->isStayKeyDown('W')) //g_DI_Keyboard->KeyDown(DIKEYBOARD_W)
		else if (g_pKeyManager->isStayKeyDown('W') || 
			(g_DI_Joystick->m_Controllers->bConnected && g_DI_Joystick->GetLYTOP()))
		{
			vPos = vPos + m_vDir * m_fMoveSpeed * g_pTimeManager->getElapsedTime();
			m_dwDirState = E_FRONT;
			m_isMoving = true;

			if (m_isOnTheGround)
			{
				if (m_fMoveSpeed == SPEED_WALK)
					m_dwState = E_WALK_FRONT;
				else m_dwState = E_RUN_FRONT;
			}
		}
		//�ڷ� �̵�
		//else if (g_pKeyManager->isStayKeyDown('S')) //g_DI_Keyboard->KeyDown(DIKEYBOARD_S)
		else if (g_pKeyManager->isStayKeyDown('S') || 
			(g_DI_Joystick->m_Controllers->bConnected && g_DI_Joystick->GetLYBOTTOM()))
		{
			m_vDir = -m_vDir;
			vPos = vPos + m_vDir * m_fMoveSpeed * g_pTimeManager->getElapsedTime();
			m_dwDirState = E_BACK;
			m_isMoving = true;

			if (m_isOnTheGround)
			{
				if (m_fMoveSpeed == SPEED_WALK)
					m_dwState = E_WALK_BACK;
				else m_dwState = E_RUN_BACK;
			}
		}
		//�������� �̵�
		//else if (g_pKeyManager->isStayKeyDown('A'))
		else if (g_pKeyManager->isStayKeyDown('A') || 
			(g_DI_Joystick->m_Controllers->bConnected && g_DI_Joystick->GetLXLEFT()))
		{
			D3DXVec3Cross(&vDir, &m_vDir, &vUp);
			vPos = vPos + vDir * m_fMoveSpeed * g_pTimeManager->getElapsedTime();
			m_dwDirState = E_LEFT;
			m_isMoving = true;

			if (m_isOnTheGround)
			{
				if (m_fMoveSpeed == SPEED_WALK)
					m_dwState = E_WALK_LEFT_90;
				else m_dwState = E_RUN_LEFT_90;
			}
		}
		//���������� �̵�
		//else if (g_pKeyManager->isStayKeyDown('D'))
		else if (g_pKeyManager->isStayKeyDown('D') || 
			(g_DI_Joystick->m_Controllers->bConnected && g_DI_Joystick->GetLXRIGHT()))
		{
			D3DXVec3Cross(&vDir, &vUp, &m_vDir);
			vPos = vPos + vDir * m_fMoveSpeed * g_pTimeManager->getElapsedTime();
			m_dwDirState = E_RIGHT;
			m_isMoving = true;

			if (m_isOnTheGround)
			{
				if (m_fMoveSpeed == SPEED_WALK)
					m_dwState = E_WALK_RIGHT_90;
				else m_dwState = E_RUN_RIGHT_90;
			}
		}
		else
		{
			m_isMoving = false;
			m_dwDirState = E_NOTMOVE;

			if (m_isOnTheGround && !m_isAttack && !m_isHit && !m_isYes && !m_isNo && !m_isSit && !m_isSleep && !m_isBuild && !m_isPickObj && !m_isCollisionTornado && !m_isFallByTornado)
				m_dwState = E_ON_THE_GROUND;
		}

		//���� �߰�
		/*if (m_isMoving && m_isOnTheGround && !m_isHit && !m_isAttack)
		{
			if (!g_pSoundManager->isPlaySound("walk"))
				g_pSoundManager->play("walk", 0.4f);
		}
		else
		{
			if (!g_pSoundManager->isPauseSound("walk"))
				g_pSoundManager->stop("walk");
		}*/

		//����
		//if (g_pKeyManager->isOnceKeyDown(VK_SPACE))
		if (g_pKeyManager->isOnceKeyDown(VK_SPACE) || (g_DI_Joystick->m_Controllers->bConnected && g_DI_Joystick->GetRSHOULDER()))
		{
			if (!m_isJump && !g_pSoundManager->isPlaySound("jump"))
			{
				g_pSoundManager->play("jump", 0.5f);	//�߰�
			}

			Jump();
		}

		//���ݴ���
		if (g_pKeyManager->isOnceKeyDown('C'))
		{
			m_isHit = true;
		}
		//����
		//else if (g_pKeyManager->isStayKeyDown('Q')) //isStayKeyDown isOnceKeyDown
		else if (g_pKeyManager->isStayKeyDown('Q') || (g_DI_Joystick->m_Controllers->bConnected && g_DI_Joystick->GetButtonPressed(XINPUT_GAMEPAD_A)))
		{
			if (m_isJump)
			{
				if (m_vPosition.y >= m_fGroundHeight + 3.0f)	//3.4f
				{
					m_isAttack = true;
				}
			}
			else
			{
				m_isAttack = true;
			}
		}
		//������ ������
		//else if (g_pKeyManager->isOnceKeyDown('E'))
		else if (g_pKeyManager->isOnceKeyDown('E') || (g_DI_Joystick->m_Controllers->bConnected && g_DI_Joystick->GetButtonPressed(XINPUT_GAMEPAD_B)))
		{
			if (m_nSnowBallNum > 0)
			{
				m_isThrow = true;
				--m_nSnowBallNum;
			}
		}
		//�ɱ� �� ����
		//else if (g_pKeyManager->isOnceKeyDown('Z'))
		else if (g_pKeyManager->isOnceKeyDown('Z') || (g_DI_Joystick->m_Controllers->bConnected && g_DI_Joystick->GetLTRIGGER()))
		{
			static int cnt = 0;
			if (cnt == 0)
			{
				m_isSit = true;
				++cnt;
			}
			else
			{
				m_isSit = false;
				m_isSleep = false;
				cnt = 0;
				m_fStackDeltaTime = 0.0f;
			}
		}
		//����
		//else if (g_pKeyManager->isOnceKeyDown('R'))
		else if (g_pKeyManager->isOnceKeyDown('R') || (g_DI_Joystick->m_Controllers->bConnected && g_DI_Joystick->GetButtonPressed(XINPUT_GAMEPAD_Y)))
		{
			if (m_nSnowBallNum < 10 || m_nBigSnowBallNum < 5)
				m_isBuild = true;
			else
				m_isNo = true;
		}
		//�ݱ�
		//else if (!m_isCanTalk && g_pKeyManager->isOnceKeyDown('F'))
		else if (!m_isCanTalk && 
			(g_pKeyManager->isOnceKeyDown('F') || 
			(g_DI_Joystick->m_Controllers->bConnected && g_DI_Joystick->GetButtonPressed(XINPUT_GAMEPAD_X))))
		{
			if (m_nSnowBallNum < 10)
				m_isPickObj = true;
			else
			{
				m_isNo = true;
			}
		}
	}

	Sit();
	Hit();
	Attack();
	Throw();

	if (m_isCollisionTornado)
		CollisionTornado(&vPos);

	AirUpdate(&vPos);
	Collision(&vPos);

	/* ����(y��) ���� */
	if (m_pMap && (m_isOnTheGround || m_isFallByTornado))
	{
		float fTempY = vPos.y;

		if (m_pMap->GetHeight(vPos.x, vPos.y, vPos.z))
		{
			if (fTempY - vPos.y >= 0.5f)	//�������� ���� �̻� ���� ������
			{
				vPos.y = fTempY;
				//m_fFallSpeed = 0.0f;
				Fall(&vPos);
			}

			if (m_isOnTheGround)
				m_fGroundHeight = vPos.y;

			if (vPos.y == m_fGroundHeight)
			{
				m_isOnTheGround = true;
				m_isCollisionTornado = false;
				m_isFallByTornado = false;
			}

			m_vPosition = vPos;
		}
	}
	else
	{
		m_vPosition = vPos;
	}

	//�̵� ��Ʈ������ ����
	D3DXMatrixTranslation(&matT, m_vPosition.x, m_vPosition.y, m_vPosition.z);
	m_fHeight = m_vPosition.y;

	//���� ��Ʈ������ ����
	//D3DXMatrixLookAtLH(&m_matRotY, &D3DXVECTOR3(0, 0, 0), &m_vDir, &D3DXVECTOR3(0, 1, 0));
	//D3DXMatrixTranspose(&m_matRotY, &m_matRotY);

	m_matWorld = m_matRotY * matT;

	//ManageCharacterState();
	
	Build();
	Yes();
	No();
	PickObj();

	//������ ���� Update
	for (int i = 0; i < m_vecSnowBall.size(); ++i)
	{
		if (m_vecSnowBall[i]->GetIsShoot())
		{
			m_vecSnowBall[i]->AirUpdate(&m_vPosition);
		}
		if (m_vecSnowBall[i]->GetIsMove())	//�̵�
		{
			m_vecSnowBall[i]->Move(&m_vPosition, &m_vDir, m_dwDirState);
		}
		if (m_vecSnowBall[i]->GetIsCollision())
		{
			DestroySnowBall(i);
		}
	}

	if (m_pSnowBallUI)
	{
		m_pSnowBallUI->SetBigNum(m_nBigSnowBallNum);
		m_pSnowBallUI->SetSmallNum(m_nSnowBallNum);
		m_pSnowBallUI->Update();
	}

	if (m_pP_Firework)
	{
		m_pP_Firework->Update();
	}

	if (m_nCntSoundEffectDestroySnow > 0 && !g_pSoundManager->isPlaySound("snow_explosion"))
	{
		g_pSoundManager->stop("snow_explosion");
		m_nCntSoundEffectDestroySnow = 0;
	}
}

void cCharacterController::Render()
{
	D3DXMATRIXA16 mat;
	D3DXMatrixIdentity(&mat);

	g_pD3DDevice->SetTransform(D3DTS_WORLD, &mat);
	g_pD3DDevice->SetTexture(0, NULL);
	g_pD3DDevice->SetFVF(D3DFVF_XYZ);

	//�߰�: ������ ����
	for (int i = 0; i < m_vecSnowBall.size(); ++i)
	{
		m_vecSnowBall[i]->Render();
	}

	if (m_pP_Firework)
		m_pP_Firework->Render();

	if (m_pHPBar)
		m_pHPBar->Render();

	if (m_pSnowBallUI)
		m_pSnowBallUI->Render();

	//����� ����
	DebugRender_Character();
}

//���콺 ���� �Լ�(ĳ���� y�� ȸ��)
void cCharacterController::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	/*switch (message)
	{
	case WM_RBUTTONDOWN:	// ��Ŭ���� �ϸ� ���� ���콺 ��ġ�� �����ϰ�
	//	m_ptPrevMouse.x = LOWORD(lParam);
	//	m_ptPrevMouse.y = HIWORD(lParam);
		m_isRButtonDown = true;
		break;

	case WM_RBUTTONUP:
		m_isRButtonDown = false;
		break;

	case WM_MOUSEMOVE:
		POINT ptCurrMouse;	// ������ �� ���� ���� ��ġ�� ã�Ƽ�

		if (!m_isRButtonDown)
		{
			ptCurrMouse.x = LOWORD(lParam);
			ptCurrMouse.y = HIWORD(lParam);

			// Y�� ȸ��
			m_fRotY += (ptCurrMouse.x - m_ptPrevMouse.x) / m_fMouseSensitivity;

			// Y�� ȸ���� ���� �ְ� ������ �����ϱ� ���ؼ� ���� ���� �ش�.
			if (m_fRotY <= -D3DX_PI * 2 + D3DX_16F_EPSILON)
				m_fRotY = -D3DX_PI * 2 + D3DX_16F_EPSILON;
			else if (m_fRotY >= D3DX_PI * 2 - D3DX_16F_EPSILON)
				m_fRotY = D3DX_PI * 2 - D3DX_16F_EPSILON;

			m_ptPrevMouse = ptCurrMouse;
			break;
		}	
	}*/
}

//ĳ���� ���� ���� �Լ�: �ִϸ��̼� ����
void cCharacterController::ManageCharacterState()
{
	//1. ����
	if (IsDeath())
	{
		int random = rand() % 3;

		if (random == 0)
			m_dwState = E_DEATH1;
		else if (random == 1)
			m_dwState = E_DEATH2;
		else m_dwState = E_DEATH3;
	}
	else if (m_isHit)
	{
		Hit();
	}
	else if (m_isAttack)
	{
		Attack();
	}
	else if (m_isJump)
	{
		m_dwState = E_JUMP_DOWN;
	}
	else if (m_isMoving)
	{

	}
	else if (m_isSpecial)
	{
		if (m_isSit)
			m_dwState = E_SIT;
		else if (m_isSleep)
			m_dwState = E_SLEEP;
	}
	else if (m_isCollisionTornado)
	{
	}
	else
	{
		m_dwState = E_ON_THE_GROUND;
	}
}

//��Ȱ �Լ�
void cCharacterController::Revival(D3DXVECTOR3* pos/*NULL*/)
{
	m_isMoving = false;
	m_fMoveSpeed = SPEED_WALK;
	m_fRotY = 0;
	m_dwDirState = E_NOTMOVE;
	m_vDir = D3DXVECTOR3(0, 0, 1);
	//ĳ���� ���� ���� ����
	m_dwState = E_ON_THE_GROUND;
	m_fTrackPeriod = 0.0f;
	m_fStackDeltaTime = 0.0f;
	//�������� ���� �ʱ�ȭ
	m_fFallSpeed = 0;
	m_isJump = false;
	m_isOnTheGround = true;
	//���� ���� ����
	m_isAttack = false;
	m_isThrow = false;
	m_isHit = false;
	m_nSnowBallNum= 0;
	m_nBigSnowBallNum = 0;
	//Ư�� ���� ���� ����
	m_isSpecial = false;
	m_isYes = false;
	m_isNo = false;
	m_isBuild = false;
	m_isPickObj = false;
	m_isSit = false;
	m_isSleep = false;
	m_isCollisionTornado = false;
	m_isFallByTornado = false;
	m_isSlideSlope = false;
	m_isCanTalk = false;
	m_nCntSoundEffectDestroySnow = 0;

	//ĳ�����浹 ���� ����
	m_isCollision = false;

	D3DXMatrixIdentity(&m_matRotY);
	D3DXMatrixIdentity(&m_matWorld);

	//ĳ���Ͱ� ���� ������ ���� �ʱ�ȭ
	m_vecSnowBall.clear();

	m_pHPBar->SetFullHP();
	m_pHPBar->SetCurrentHp(HPENDX);
	m_pHPBar->SetIsDeath(false);
	m_pHPBar->SetIsOneKill(false);

	m_stHp.m_fHp = m_stHp.m_fMaxHp;
	m_stHp.m_isDeath = false;

	if (*pos)
	{
		m_vPosition = *pos;
		m_pMap->GetHeight(m_vPosition.x, m_vPosition.y, m_vPosition.z);
	}
}


//���� ���� �Լ� �߰�
void cCharacterController::Jump()
{
	if (m_dwState == E_JUMP_UP || m_dwState == E_JUMP_DOWN) return;
	if (!m_isOnTheGround) return;
	if (m_isCollisionTornado) return;

	m_dwState = E_JUMP_UP;
	m_fFallSpeed = -m_fJumpPower;
	m_isJump = true;
	m_isOnTheGround = false;
}

void cCharacterController::AirUpdate(D3DXVECTOR3* pos)
{
	if (!m_isOnTheGround && m_isJump && !m_isCollisionTornado)
	{
		m_fFallSpeed += g_pTimeManager->getElapsedTime() * m_fGravity;

		if (m_fFallSpeed >= MAX_SPEED_PER_SEC)
			m_fFallSpeed = MAX_SPEED_PER_SEC;

		pos->y -= m_fFallSpeed * g_pTimeManager->getElapsedTime();
	}
}

void cCharacterController::Collision(D3DXVECTOR3* pos)
{
	if (m_isOnTheGround) return;
	if (!m_isJump) return;
	if (!m_pMap->GetHeight(pos->x, m_fHeight, pos->z)) return;

	if (pos && m_pMap && m_fFallSpeed > 0)
	{
		m_pMap->GetHeight(pos->x, m_fHeight, pos->z);

		//�� �浹
		if (pos->y < m_fHeight + D3DX_16F_EPSILON)
		{
			m_dwState = E_ON_THE_GROUND;
			m_isJump = false;
			m_isOnTheGround = true;
			pos->y = m_fHeight;	//���Ϲ��� ��������
		}
	}	
}

void cCharacterController::Fall(D3DXVECTOR3* pos)
{
	//static float fallSpeed = 0;
	//m_dwState = E_AIR;
	m_dwState = E_JUMP_UP;

	m_fFallSpeed += g_pTimeManager->getElapsedTime() * m_fGravity;

	if (m_fFallSpeed >= 12)
		m_fFallSpeed = 12;

	pos->y -= m_fFallSpeed * g_pTimeManager->getElapsedTime();
}

//���� ���� �Լ�
void cCharacterController::Attack()
{
	if (!m_isAttack) return;
	if (m_isCollisionTornado) return;

	float fDeltaTime = g_pDeltaTimeManager->GetDeltaTime();
	static int playSoundCntAttack = 0;

	if (m_fStackDeltaTime < 0.4) //0.666666687
	{
		m_fStackDeltaTime += fDeltaTime;
		m_isAttack = true;

		if (m_isJump)
			m_dwState = E_ATTACK2;
		else
			m_dwState = E_ATTACK3;

		if (playSoundCntAttack == 0 && !g_pSoundManager->isPlaySound("attack"))
		{
			g_pSoundManager->play("attack", 0.5f);	//�߰�
			++playSoundCntAttack;
		}
	}
	else
	{
		m_isAttack = false;
		if (m_isJump)
			m_dwState = E_JUMP_DOWN;
		else m_dwState = E_ON_THE_GROUND;
		m_fStackDeltaTime = 0.0f;

		if (playSoundCntAttack > 0 && !g_pSoundManager->isPlaySound("attack"))
		{
			playSoundCntAttack = 0;
			g_pSoundManager->stop("attack");
		}
	}	
}

void cCharacterController::Throw()
{
	if (!m_isThrow) return;
	if (m_isCollisionTornado) return;

	float fDeltaTime = g_pDeltaTimeManager->GetDeltaTime();
	static int playSoundCntThrow = 0;

	if (m_fStackDeltaTime < 0.65) //1.00000000
	{
		m_fStackDeltaTime += fDeltaTime;
		m_isThrow = true;
		m_dwState = E_THROW;

		if (playSoundCntThrow == 0 && !g_pSoundManager->isPlaySound("pickup"))
		{
			g_pSoundManager->play("pickup", 0.5f);		//�߰�
			++playSoundCntThrow;
		}
	}
	else
	{
		m_isThrow = false;
		if (m_isJump)
			m_dwState = E_JUMP_DOWN;
		else m_dwState = E_ON_THE_GROUND;
		m_fStackDeltaTime = 0.0f;
		if (playSoundCntThrow > 0 && !g_pSoundManager->isPlaySound("pickup"))
		{
			g_pSoundManager->stop("pickup");
			playSoundCntThrow = 0;
		}

		//������ ����
		for (int i = 0; i < m_vecSnowBall.size(); ++i)
		{
			if (m_vecSnowBall[i]->GetState() == E_SNOW_BIG) continue;
			if (m_vecSnowBall[i]->GetState() == E_SNOW_BUILD) continue;
			if (m_vecSnowBall[i]->GetState() == E_SNOW_PICKED)	//�߰�
				m_vecSnowBall[i]->Shoot(&m_vPosition, &m_vDir);
			break;
		}
	}
}

void cCharacterController::Hit()
{
	if (!m_isHit) return;
	
	float fDeltaTime = g_pDeltaTimeManager->GetDeltaTime();
	static int playSoundCntHit = 0;

	if (m_fStackDeltaTime < 0.500000000)
	{
		m_fStackDeltaTime += fDeltaTime;
		m_isHit = true;
		m_dwState = E_HIT2;

		if (playSoundCntHit == 0 && !g_pSoundManager->isPlaySound("hit"))
		{
			g_pSoundManager->play("hit", 0.4f);		//�߰�
			++playSoundCntHit;
		}
	}
	else
	{
		m_stHp.m_fHp -= m_stHp.m_fMaxHp / 10;
		m_pHPBar->DamagedOn();
		//m_pHPBar->SetCurrentHp(m_stHp.m_fHp);

		m_isHit = false;
		if (m_isJump)
			m_dwState = E_JUMP_DOWN;
		else m_dwState = E_ON_THE_GROUND;
		m_fStackDeltaTime = 0.0f;

		g_pSoundManager->stop("hit");
		playSoundCntHit = 0;
	}
}

//����ġ �ı� �Լ�
void cCharacterController::DestroySnowBall(int index)
{
	m_pP_Firework = new cParticle_Firework(m_vecSnowBall[index]->GetSphere().vCenter, 20);
	m_pP_Firework->Setup("Textures/flare.bmp");
	
	if (m_vecSnowBall[index]->GetState() == E_SNOW_BIG)
		m_pP_Firework->SetParticleSize(0.2f);

	m_vecSnowBall.erase(m_vecSnowBall.begin() + index);

	//���� �߰�
	if (m_nCntSoundEffectDestroySnow ==0 && !g_pSoundManager->isPlaySound("snow_explosion"))
	{
		g_pSoundManager->play("snow_explosion", 0.8f);
		++m_nCntSoundEffectDestroySnow;
	}
}

//Ư�� ���� ���� �Լ�
void cCharacterController::Yes()
{
	if (!m_isYes) return;

	float fDeltaTime = g_pDeltaTimeManager->GetDeltaTime();

	if (m_fStackDeltaTime < 0.8)
	{
		m_fStackDeltaTime += fDeltaTime;
		m_isYes = true;
		m_dwState = E_YES;
	}
	else
	{
		m_isYes = false;
		m_dwState = E_ON_THE_GROUND;
		m_fStackDeltaTime = 0.0f;
	}
}

void cCharacterController::No()
{
	if (!m_isNo) return;

	float fDeltaTime = g_pDeltaTimeManager->GetDeltaTime();

	if (m_fStackDeltaTime < 0.8)
	{
		m_fStackDeltaTime += fDeltaTime;
		m_isNo = true;
		m_dwState = E_NO;
	}
	else
	{
		m_isNo = false;
		m_dwState = E_ON_THE_GROUND;
		m_fStackDeltaTime = 0.0f;
	}
}

void cCharacterController::Build()
{
	if (m_isJump) return;
	if (!m_isBuild) return;
	if (m_isCollisionTornado) return;

	float fDeltaTime = g_pDeltaTimeManager->GetDeltaTime();
	static int soundEffectCntBuild = 0;

	if (m_fStackDeltaTime < 1.5) //1.83333337
	{
		m_fStackDeltaTime += fDeltaTime;
		m_isBuild = true;
		m_dwState = E_BUILD;

		if (soundEffectCntBuild > 0 && !g_pSoundManager->isPlaySound("pickup"))
		{
			g_pSoundManager->stop("pickup");
			soundEffectCntBuild = 0;
		}
	}
	else
	{
		m_isBuild = false;
		m_dwState = E_ON_THE_GROUND;
		m_fStackDeltaTime = 0.0f;

		//�߰�: ������ �����
		if (m_vecSnowBall.empty())	//������ ���ʷ� �����
		{
			cSnowBall* snowBall = new cSnowBall;
			snowBall->AutoRelease();
			snowBall->Build(&m_vPosition, &m_vDir);
			snowBall->SetMap(m_pMap);
			m_vecSnowBall.push_back(snowBall);
		}
		else
		{
			bool isExist = false;
			for (int i = 0; i < m_vecSnowBall.size(); ++i)
			{
				//�ٸ� ����캼�� ������ ���
				if (m_vecSnowBall[i]->IsCheckExist(&m_vPosition))
				{
					if (m_nBigSnowBallNum < 5)
					{
						isExist = true;
						m_vecSnowBall[i]->Build(&m_vPosition, &m_vDir);
						++m_nBigSnowBallNum;
						break;
					}
					else
						m_isNo = true;
				}
			}

			//�ٸ� ����캼�� �������� ���� ���
			if (!isExist)
			{
				cSnowBall* snowBall = new cSnowBall;
				snowBall->AutoRelease();
				snowBall->Build(&m_vPosition, &m_vDir);
				snowBall->SetMap(m_pMap);
				m_vecSnowBall.push_back(snowBall);
			}
		}

		//���� �߰�
		if (soundEffectCntBuild == 0 && !g_pSoundManager->isPlaySound("pickup"))
		{
			g_pSoundManager->play("pickup", 0.5f);
			++soundEffectCntBuild;
		}
	}
}

void cCharacterController::PickObj()
{
	if (m_isJump) return;
	if (!m_isPickObj) return;
	if (m_isCollisionTornado) return;

	float fDeltaTime = g_pDeltaTimeManager->GetDeltaTime();
	static int playSoundCntPick = 0;

	if (m_fStackDeltaTime < 0.4) //0.666666687
	{
		m_fStackDeltaTime += fDeltaTime;
		m_isPickObj = true;
		m_dwState = E_PICK_OBJ;

		if (playSoundCntPick == 0 && !g_pSoundManager->isPlaySound("pickup"))
		{
			g_pSoundManager->play("pickup", 0.5f);		//�߰�
			++playSoundCntPick;
		}
	}
	else
	{
		m_isPickObj = false;
		m_dwState = E_ON_THE_GROUND;
		m_fStackDeltaTime = 0.0f;
		if (playSoundCntPick > 0 && !g_pSoundManager->isPlaySound("pickup"))
		{
			g_pSoundManager->stop("pickup");
			playSoundCntPick = 0;
		}

		//�߰�: ������ ������
		for (int i = 0; i < m_vecSnowBall.size(); ++i)
		{
			if (m_vecSnowBall[i]->PickedUp(&m_vPosition))
			{
				++m_nSnowBallNum;
			}
		}
	}
}

void cCharacterController::Sit()
{
	if (!m_isSit && !m_isSleep) return;
	if (m_isCollisionTornado || m_isFallByTornado) return;

	if (m_isMoving || m_isAttack || m_isThrow || m_isHit || m_isJump || m_isYes || m_isNo || m_isBuild || m_isPickObj)
	{
		//m_fStackDeltaTime = 0.0f;
		m_isSit = false;
		m_isSleep = false;
		m_isSpecial = false;

		if (m_isHit)
			m_dwState = E_HIT2;
		else
			m_dwState = E_ON_THE_GROUND;
	}

	float fDeltaTime = g_pDeltaTimeManager->GetDeltaTime();

	if (m_fStackDeltaTime < 10.0)
	{
		m_isSpecial = true;
		m_fStackDeltaTime += fDeltaTime;
		m_dwState = E_SIT;
	}
	else
	{
		m_isSpecial = true;
		m_isSit = false;
		m_isSleep = true;
		m_dwState = E_SLEEP;
	}
}

void cCharacterController::Sleep()
{
	if (!m_isSleep) return;

	m_dwState = E_SLEEP;
	m_isSpecial = true;
}

// ����̵��� �浹���� ��
void cCharacterController::CollisionTornado(D3DXVECTOR3* pos)
{
	if (!m_isCollisionTornado) return;
	if (m_isFallByTornado) return;

	float fDeltaTime = g_pDeltaTimeManager->GetDeltaTime();
	static int playSoundCntTornado = 0;

	if (m_fStackDeltaTime < 1.0)
	{
		m_fStackDeltaTime += fDeltaTime;
		m_isCollisionTornado = true;
		m_isOnTheGround = false;
		m_dwState = E_JUMP_UP;

		pos->y += 5.0f * g_pTimeManager->getElapsedTime();
		m_fRotY += (D3DX_PI * 2) * g_pTimeManager->getElapsedTime();

		//ĳ���� ���� ����
		D3DXVECTOR3 vDir;
		D3DXMatrixRotationY(&m_matRotY, m_fRotY);
		D3DXVec3TransformNormal(&vDir, &vDir, &m_matRotY);
		D3DXVec3Normalize(&m_vDir, &vDir);

		//���� �߰�
		if (playSoundCntTornado == 0 && !g_pSoundManager->isPlaySound("tornado"))
		{
			g_pSoundManager->play("tornado", 0.5f);
			++playSoundCntTornado;
		}
	}
	else
	{
		m_isCollisionTornado = false;
		m_isFallByTornado = true;
		m_fStackDeltaTime = 0.0f;

		if (playSoundCntTornado > 0 && !g_pSoundManager->isPlaySound("pickup"))
		{
			g_pSoundManager->stop("tornado");
			playSoundCntTornado = 0;
		}
	}
}

//��� �̲�����(����)
void cCharacterController::SlideSlopeVillage(D3DXVECTOR3* vPos)
{
	if (m_isJump) return;
	if (m_isCollisionTornado || m_isFallByTornado) return;
	if (m_isMoving) return;

	if (vPos->z < 2.25f)		//����
	{
		D3DXVECTOR3 vDir(0, 0, 1);
		D3DXVec3Normalize(&vDir, &vDir);

		if (vPos->y >= 0.7f)
		{
			m_isSlideSlope = true;
		}
		else if (vPos->y <= 0.0f)
		{
			m_isSlideSlope = false;
		}

		if (m_isSlideSlope)
		{
			*vPos += vDir * 2.0f * g_pTimeManager->getElapsedTime();
		}
	}
	else if (vPos->x < 2.2f)	//����
	{
		D3DXVECTOR3 vDir(1, 0, 0);
		D3DXVec3Normalize(&vDir, &vDir);

		if (vPos->y >= 0.7f)
		{
			m_isSlideSlope = true;
		}
		else if (vPos->y <= 0.0f)
		{
			m_isSlideSlope = false;
		}

		if (m_isSlideSlope)
		{
			*vPos += vDir * 2.0f * g_pTimeManager->getElapsedTime();
		}
	}
	else if (vPos->x > 61.97)	//������
	{
		D3DXVECTOR3 vDir(-1, 0, 0);
		D3DXVec3Normalize(&vDir, &vDir);

		if (vPos->y >= 0.2f)
		{
			m_isSlideSlope = true;
		}
		else if (vPos->y <= 0.0f)
		{
			m_isSlideSlope = false;
		}

		if (m_isSlideSlope)
		{
			*vPos += vDir * 2.0f * g_pTimeManager->getElapsedTime();
		}
	}
	else if (vPos->z > 61.55)	//����
	{
		D3DXVECTOR3 vDir(0, 0, -1);
		D3DXVec3Normalize(&vDir, &vDir);

		if (vPos->y >= 0.2f)
		{
			m_isSlideSlope = true;
		}
		else if (vPos->y <= 0.0f)
		{
			m_isSlideSlope = false;
		}

		if (m_isSlideSlope)
		{
			*vPos += vDir * 2.0f * g_pTimeManager->getElapsedTime();
		}
	}
}

//��� �̲�����(��������)
void cCharacterController::SlideSlopeStage(D3DXVECTOR3* vPos, float fDepth)
{
	if (vPos->y < fDepth && vPos->y > fDepth / 2)
	{
		m_isSlideSlope = true;
	}
	else if (vPos->y <= fDepth / 2)
	{
		m_isSlideSlope = false;
		//vPos->y =  fDepth - 2.5f;

		if (vPos->y <= fDepth - 2.5f)
		{
			m_pHPBar->SetIsOneKill(true);
			m_stHp.m_fHp = 0;
		}
	}
	else
	{
		m_isSlideSlope = false;
	}

	if (m_isSlideSlope)
	{
		D3DXVECTOR3 vDir = m_vDir;
		vDir.y = 0;

		if (m_dwDirState == E_DIAGONAL_FL || m_dwDirState == E_DIAGONAL_BL || m_dwDirState == E_LEFT)
		{
			vDir = D3DXVECTOR3(0, 0, 1);
		}
		else if (m_dwDirState == E_DIAGONAL_FR || m_dwDirState == E_DIAGONAL_BR || m_dwDirState == E_RIGHT)
		{
			vDir = D3DXVECTOR3(0, 0, -1);
		}
		else if (m_dwDirState == E_FRONT)
		{
			vDir = m_vDir;
		}
		else if (m_dwDirState == E_BACK)
		{
			vDir = -m_vDir;
		}

		*vPos += vDir * 2.0f * g_pTimeManager->getElapsedTime();
	}
}

//���� ���� �Լ�
bool cCharacterController::IsDeath()
{
	static int playSoundCntDeath = 0;

	if (m_stHp.m_fHp <= 0)
	{
		m_stHp.m_isDeath = true;

		if (playSoundCntDeath == 0 && !g_pSoundManager->isPlaySound("water_falling"))
		{
			g_pSoundManager->play("water_falling", 0.5f);
			++playSoundCntDeath;
		}
	}
	else
	{
		m_stHp.m_isDeath = false;

		if (playSoundCntDeath > 0 && !g_pSoundManager->isPlaySound("water_falling"))
		{
			playSoundCntDeath = 0;
			g_pSoundManager->stop("water_falling");
		}
	}

	return m_stHp.m_isDeath;
}

void cCharacterController::Death()
{
	if (!IsDeath()) return;

	if (!m_isOnTheGround)
	{
		AirUpdate(&m_vPosition);
		Collision(&m_vPosition);

		/* ����(y��) ���� */
		if (m_pMap && m_isOnTheGround)
		{
			float fTempY = m_vPosition.y;
			m_pMap->GetHeight(m_vPosition.x, m_vPosition.y, m_vPosition.z);
		}

		//�̵� ��Ʈ������ ����
		D3DXMATRIXA16 matT;
		D3DXMatrixTranslation(&matT, m_vPosition.x, m_vPosition.y, m_vPosition.z);

		m_matWorld = m_matRotY * matT;
	}
	
	if (m_isCollisionTornado)
	{
		CollisionTornado(&m_vPosition);
	}
	else if (m_isFallByTornado)
	{
		float fTempY = m_vPosition.y;

		if (m_pMap->GetHeight(m_vPosition.x, m_vPosition.y, m_vPosition.z))
		{
			if (fTempY - m_vPosition.y >= 0.1f)	//�������� ���� �̻� ���� ������
			{
				m_vPosition.y = fTempY;
				//m_fFallSpeed = 0.0f;
				Fall(&m_vPosition);
			}

			if (m_vPosition.y == 4.0f)
			{
				m_isOnTheGround = true;
				m_isFallByTornado = false;
			}
		}
	}

	if (m_isOnTheGround)
	{
		float fDeltaTime = g_pDeltaTimeManager->GetDeltaTime();

		if (m_fStackDeltaTime < 0.7)
		{
			m_fStackDeltaTime += fDeltaTime;
			m_dwState = E_DEATH1;
		}
		else
		{
			m_fStackDeltaTime = 0.0f;
		}
	}

	//�׾��� �� ������ ó��
	for (int i = 0; i < m_vecSnowBall.size(); ++i)
	{
		if (m_vecSnowBall[i]->GetIsShoot())
		{
			m_vecSnowBall.erase(m_vecSnowBall.begin() + i);	//�߻��� ������ ����
		}
	}
}

//���� �߰� �Լ�
void cCharacterController::AddEffectSound()	//�� 15ä��
{
	//ĳ����
	g_pSoundManager->addSound("walk"			, "./Sound/EffectSound/walk.wav", false, false);
	g_pSoundManager->addSound("attack"			, "./Sound/EffectSound/attack.wav", false, false);
	g_pSoundManager->addSound("snow_explosion"	, "./Sound/EffectSound/snow_explosion.wav", false, false);
	g_pSoundManager->addSound("hit"				, "./Sound/EffectSound/hit.wav", false, false);
	g_pSoundManager->addSound("jump"			, "./Sound/EffectSound/jump.wav", false, false);
	g_pSoundManager->addSound("pickup"			, "./Sound/EffectSound/pickup.wav", false, false);
	g_pSoundManager->addSound("throw"			, "./Sound/EffectSound/throw.wav", false, false);
	g_pSoundManager->addSound("water_falling"	, "./Sound/EffectSound/water_falling.wav", false, false);
	g_pSoundManager->addSound("tornado"			, "./Sound/EffectSound/tornado.wav", false, false);

	//����
	g_pSoundManager->addSound("hit_monster"		, "./Sound/EffectSound/hit_monster.wav", false, false);
	g_pSoundManager->addSound("monster_attack"	, "./Sound/EffectSound/monster_attack.wav", false, false);
	g_pSoundManager->addSound("monster_scream"	, "./Sound/EffectSound/monster_scream.wav", false, false);

	//�̰�����
	g_pSoundManager->addSound("achievement"		, "./Sound/EffectSound/achievement.wav", false, false);
	g_pSoundManager->addSound("button"			, "./Sound/EffectSound/button.wav", false, false);
	g_pSoundManager->addSound("fanfare"			, "./Sound/EffectSound/fanfare.wav", false, false);
}

//ĳ���� ���� ��� ����� ���� �Լ�
void cCharacterController::DebugRender_Character()
{
	//ĳ���� ���� ���
	if (g_pKeyManager->isOnceKeyDown('2'))
	{
		m_isOnKey2 = !m_isOnKey2;
	}

	if (m_isOnKey2)
	{
		char szStr[1024];
		switch (m_dwState)
		{
		case E_ON_THE_GROUND:
			sprintf(szStr, "state: E_ON_THE_GROUND");
			break;
		case E_IDLE:
			sprintf(szStr, "state: E_IDLE");
			break;
		case E_WALK_FRONT:
			sprintf(szStr, "state: E_WALK_FRONT");
			break;
		case E_WALK_BACK:
			sprintf(szStr, "state: E_WALK_BACK");
			break;
		case E_WALK_LEFT_45:
			sprintf(szStr, "state: E_WALK_LEFT_45");
			break;
		case E_WALK_LEFT_90:
			sprintf(szStr, "state: E_WALK_LEFT_90");
			break;
		case E_WALK_RIGHT_45:
			sprintf(szStr, "state: E_WALK_RIGHT_45");
			break;
		case E_WALK_RIGHT_90:
			sprintf(szStr, "state: E_WALK_RIGHT_90");
			break;
		case E_RUN_FRONT:
			sprintf(szStr, "state: E_RUN_FRONT");
			break;
		case E_RUN_BACK:
			sprintf(szStr, "state: E_RUN_BACK");
			break;
		case E_RUN_LEFT_45:
			sprintf(szStr, "state: E_RUN_LEFT_45");
			break;
		case E_RUN_LEFT_90:
			sprintf(szStr, "state: E_RUN_LEFT_90");
			break;
		case E_RUN_RIGHT_45:
			sprintf(szStr, "state: E_RUN_RIGHT_45");
			break;
		case E_RUN_RIGHT_90:
			sprintf(szStr, "state: E_RUN_RIGHT_90");
			break;
		case E_ROLL_FRONT:
			sprintf(szStr, "state: E_ROLL_FRONT");
			break;
		case E_ROLL_BACK:
			sprintf(szStr, "state: E_ROLL_BACK");
			break;
		case E_ROLL_LEFT:
			sprintf(szStr, "state: E_ROLL_LEFT");
			break;
		case E_ROLL_RIGHT:
			sprintf(szStr, "state: E_ROLL_RIGHT");
			break;
		case E_JUMP_UP:
			sprintf(szStr, "state: E_JUMP_UP");
			break;
		case E_JUMP_DOWN:
			sprintf(szStr, "state: E_JUMP_DOWN");
			break;
		case E_AIR:
			sprintf(szStr, "state: E_AIR");
			break;
		case E_HIT1:
			sprintf(szStr, "state: E_HIT1");
			break;
		case E_HIT2:
			sprintf(szStr, "state: E_HIT2");
			break;
		case E_HIT_FALL:
			sprintf(szStr, "state: E_HIT_FALL");
			break;
		case E_GET_UP:
			sprintf(szStr, "state: E_GET_UP");
			break;
		case E_ATTACK1:
			sprintf(szStr, "state: E_ATTACK1");
			break;
		case E_ATTACK2:
			sprintf(szStr, "state: E_ATTACK2");
			break;
		case E_ATTACK3:
			sprintf(szStr, "state: E_ATTACK3");
			break;
		case E_THROW:
			sprintf(szStr, "state: E_THROW");
			break;
		case E_DEATH1:
			sprintf(szStr, "state: E_DEATH1");
			break;
		case E_DEATH2:
			sprintf(szStr, "state: E_DEATH2");
			break;
		case E_DEATH3:
			sprintf(szStr, "state: E_DEATH3");
			break;
		case E_YES:
			sprintf(szStr, "state: E_YES");
			break;
		case E_NO:
			sprintf(szStr, "state: E_NO");
			break;
		case E_SIT:
			sprintf(szStr, "state: E_SIT");
			break;
		case E_SLEEP:
			sprintf(szStr, "state: E_SLEEP");
			break;
		case E_BUILD:
			sprintf(szStr, "state: E_BUILD");
			break;
		case E_PICK_OBJ:
			sprintf(szStr, "state: E_PICK_OBJ");
			break;
		}
		//m_pFont->DrawTextA(NULL, szStr, strlen(szStr),
		//	m_pRect, DT_TOP | DT_LEFT | DT_NOCLIP, D3DCOLOR_XRGB(255, 0, 0));


		char szStr2[1024];
		switch (m_dwDirState)
		{
		case E_NOTMOVE:
			sprintf(szStr2, "\nMoveDir: E_NOTMOVE");
			break;
		case E_FRONT:
			sprintf(szStr2, "\nMoveDir: E_FRONT");
			break;
		case E_BACK:
			sprintf(szStr2, "\nMoveDir: E_BACK");
			break;
		case E_LEFT:
			sprintf(szStr2, "\nMoveDir: E_LEFT");
			break;
		case E_RIGHT:
			sprintf(szStr2, "\nMoveDir: E_RIGHT");
			break;
		case E_DIAGONAL_FL:
			sprintf(szStr2, "\nMoveDir: E_DIAGONAL_FL");
			break;
		case E_DIAGONAL_FR:
			sprintf(szStr2, "\nMoveDir: E_DIAGONAL_FR");
			break;
		case E_DIAGONAL_BL:
			sprintf(szStr2, "\nMoveDir: E_DIAGONAL_BL");
			break;
		case E_DIAGONAL_BR:
			sprintf(szStr2, "\nMoveDir: E_DIAGONAL_BR");
			break;
		}
		strcat(szStr, szStr2);

		sprintf(szStr2, "\nHp: %d\/%d\nX: %f\nY: %f\nZ: %f\nspeed: %f\nSnowBall: %d\nBigSnowBall: %d", 
			m_stHp.m_fHp, m_stHp.m_fMaxHp, m_vPosition.x, m_vPosition.y, m_vPosition.z, m_fMoveSpeed, m_nSnowBallNum, m_nBigSnowBallNum);

		strcat(szStr, szStr2);

		if (m_isOnTheGround)
			sprintf(szStr2, "\nisOnTheGround: true");
		else sprintf(szStr2, "\nisOnTheGround: false");

		strcat(szStr, szStr2);
			
		if (m_isCollision)
			sprintf(szStr2, "\nisCollision: true");
		else sprintf(szStr2, "\nisCollision: false");

		strcat(szStr, szStr2);

		if (m_isCollisionTornado)
			sprintf(szStr2, "\nisCollisionTornado: true");
		else sprintf(szStr2, "\nisCollisionTornado: false");

		strcat(szStr, szStr2);

		m_pFont->DrawTextA(NULL, szStr, strlen(szStr),
			m_pRect, DT_TOP | DT_LEFT | DT_NOCLIP, D3DCOLOR_XRGB(255, 0, 0));
	}
}

/* ĳ���� HP ������ ���� �Լ� */
//////////////////////////////////////////////////
void cCharacterController::SetHpBar(float hp){m_pHPBar->SetCurrentHp(hp);}
void cCharacterController::SetHpBarIsSetHp(bool isSetHp){ m_pHPBar->SetIsSetHp(isSetHp); }

float cCharacterController::GetHpBar(){return m_pHPBar->GetCurrentHp();}
bool cCharacterController::GetHpBarIsSetHp(){return m_pHPBar->GetIsSetHp();}
//////////////////////////////////////////////////