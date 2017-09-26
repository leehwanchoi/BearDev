#include "stdafx.h"
#include "cNPCController.h"
#include "cTextBox.h"

cNPCController::cNPCController()
	: m_vPosition(0, 0, 0)
	, m_vFirstPosition(0, 0, 0)
	, m_vCharacterPosition(0, 0, 0)
	, m_isMoving(false)
	, m_fMoveSpeed(SPEED_WALK)
	, m_fRotY(0)
	, m_fMoveDistance(0)
	, m_vDir(0, 0, 1)
	, m_vFirstDir(0, 0, 1)
	, m_vLastDir(0, 0, 1)
	, m_vLookFirstDir(0, 0, 1)
	, m_vLookDir(0, 0, 1)
	, m_pMap(NULL)
	//ĳ���� ���� ���� ����
	, m_dwState(E_ON_THE_GROUND)
	, m_dwMoveMode(E_NPC_NOT_MOVE)
	, m_fTrackPeriod(0.0f)
	, m_fStackDeltaTime(0.0f)
	//Ư�� ���� ���� ����
	, m_isYes(false)
	, m_isNo(false)
	, m_isSit(false)
	, m_isSleep(false)
	//NPC ��ȭ&����Ʈ ���� ����
	, m_pUITextBox(NULL)
	, m_canTalk(false)
	, m_isLookAt(false)
	, m_isEndTalk(false)
	, m_isSmoothTurn(false)
	, m_isShowTalkUI(false)
	, m_fActionTime(0.5f)
	, m_fPassedTime(0.0f)
	, m_vFromDir(0, 0, 1)
	, m_vToDir(0, 0, 1)
	//NPC ��ȭ Ŭ���� ����
	, m_isClear(false)
	//NPC�浹 ���� ����
	, m_isCollision(false)
	//����� ���� ���� ����
	, m_isOnKey4(false)

{
	D3DXMatrixIdentity(&m_matRotY);
	D3DXMatrixIdentity(&m_matWorld);

	//�ȱ� �ִϸ��̼� ���� ����
	srand(time(NULL));
	rand();
	m_nRandomWalk = rand() % 3;

	//NPC ���� ����� �ؽ�Ʈ ��� ����
	m_pRect = new RECT;
	m_pRect->left = WINSIZEX - 400;
	m_pRect->top = 30;
	m_pRect->right = WINSIZEX;
	m_pRect->bottom = 90;

	D3DXCreateFont(g_pD3DDevice, 24, 12, 0, 1, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, false, "Arial", &m_pFont);

	D3DXCreateSphere(g_pD3DDevice, DISTANCE_RECOGNIZE, 10, 10, &m_pSphereRange, NULL);
}


cNPCController::~cNPCController()
{
	SAFE_DELETE(m_pRect);
	SAFE_RELEASE(m_pFont);
	SAFE_RELEASE(m_pSphereRange);
}

void cNPCController::Setup(D3DXVECTOR3* pos, float fRotY, DWORD mode)
{
	m_vPosition = *pos;		//������ ��ġ�� NPC ����
	m_vFirstPosition = m_vPosition;

	if (m_pMap)
	{
		m_pMap->GetHeight(m_vPosition.x, m_vPosition.y, m_vPosition.z);	//��������
	}

	m_fRotY = fRotY;		//������ ������ NPC Y�� ���� ����

	//ȸ�� ��Ʈ���� ����
	D3DXMatrixRotationY(&m_matRotY, m_fRotY);

	//NPC ���� ����
	D3DXVECTOR3 vDir(0, 0, 1);
	D3DXVec3TransformNormal(&vDir, &vDir, &m_matRotY);
	D3DXVec3Normalize(&m_vDir, &vDir);
	m_vFirstDir = m_vDir;

	D3DXMATRIXA16 matR;
	D3DXMatrixRotationY(&matR, D3DX_PI);
	D3DXVec3TransformNormal(&m_vLastDir, &m_vFirstDir, &matR);
	//m_vLastDir = -m_vDir;

	m_dwMoveMode = mode;	//NPC�� �̵���� ����

	if (m_dwMoveMode == E_NPC_NOT_MOVE)
		m_vLookFirstDir = m_vFirstDir;

	if (m_dwMoveMode == E_NPC_MOVE_ONEWAY)
	{
		m_isMoving = true;
	}

	m_pUITextBox = new cTextBox;
	m_pUITextBox->AutoRelease();
	m_pUITextBox->Setup();
	m_pUITextBox->SetTextSpeed(0.1f);
}

void cNPCController::Update(D3DXVECTOR3* pos)
{
	m_vCharacterPosition = *pos;

	//NPC �������� ����
	if (m_dwMoveMode == E_NPC_NOT_MOVE)
	{
		NotMove();
	}
	//NPC Ư�� ���� ������ �̵�
	else if (m_dwMoveMode == E_NPC_MOVE_ONEWAY)
	{
		MoveOneway();
	}
	//NPC Ư�� ������ �ֺ��� ������
	else if (m_dwMoveMode == E_NPC_MOVE_SURROUND)
	{
		MoveSurround();
	}

	//NPC�� ĳ���� ���������ȿ� �������� Ȯ��
	if (IsInRecognizeRange())	//���������ȿ� ���� ���, ��ȭ������ ����
	{
		m_canTalk = true;

		//��ȭ ��ȣ�ۿ� 
		if (m_canTalk && g_pKeyManager->isOnceKeyDown('F'))
		/*if (m_canTalk &&
			(g_pKeyManager->isOnceKeyDown('F') ||
			(g_DI_Joystick->m_Controllers->bConnected && g_DI_Joystick->GetButtonPressed(XINPUT_GAMEPAD_X))))*/
		{
			m_isLookAt = true;
		}
	}
	else
	{
		m_canTalk = false;
	}

	LookAt(&m_vLookFirstDir);	//NPC �ٶ󺸴� ��������
	Talk();						//UI ������Ʈ

	ManageNPCState();			//NPC ���°��� �Լ�(�ִϸ��̼� ��)
}

void cNPCController::Render()
{
	//D3DXMATRIXA16 mat;
	//D3DXMatrixIdentity(&mat);

	g_pD3DDevice->SetTransform(D3DTS_WORLD, &m_matWorld);
	g_pD3DDevice->SetTexture(0, NULL);
	g_pD3DDevice->SetFVF(D3DFVF_XYZ);

	//����� ����
	DebugRender_NPC();

	//UI ����
	if (m_pUITextBox && m_isShowTalkUI)
	{
		m_pUITextBox->Render();
	}
}

//NPC �̵���� ���� �Լ�
void cNPCController::NotMove()
{
	//���� ����
	if (m_pMap)
		m_pMap->GetHeight(m_vPosition.x, m_vPosition.y, m_vPosition.z);

	//�̵� ��Ʈ������ ����
	D3DXMATRIXA16 matT;
	D3DXMatrixTranslation(&matT, m_vPosition.x, m_vPosition.y, m_vPosition.z);
	m_matWorld = m_matRotY * matT;
}

void cNPCController::MoveOneway()
{
	static bool isRight = false;
	static bool isLeft = true;

	if (!m_isLookAt && !m_isCollision)
	{
		m_isMoving = true;

		if (isLeft && m_isMoving)
		{
			m_vDir = m_vFirstDir;
			m_vLookFirstDir = m_vFirstDir;

			if (g_pTimeManager->getElapsedTime() < 1 / 6.0f)	//10�������̻��� ���;� ���ư�(��������)
			{
				if (m_isMoving)
					m_vPosition = m_vPosition + m_vDir * m_fMoveSpeed * g_pTimeManager->getElapsedTime();
			}

			//�̵��Ÿ��� ���� �̵����� ����
			D3DXVECTOR3 vDis = m_vPosition - m_vFirstPosition;
			float fDis = D3DXVec3Length(&vDis);

			if (fDis >= m_fMoveDistance)
			{
				isLeft = false;
				isRight = true;
			}
		}
		else if (isRight && m_isMoving)
		{
			m_vDir = m_vLastDir;
			m_vLookFirstDir = m_vLastDir;
			m_vPosition = m_vPosition + m_vDir * m_fMoveSpeed * g_pTimeManager->getElapsedTime();

			//�̵��Ÿ��� ���� �̵����� ����
			D3DXVECTOR3 vDis = m_vPosition - m_vFirstPosition;
			float fDis = D3DXVec3Length(&vDis);

			if (fDis >= m_fMoveDistance)
			{
				isLeft = true;
				isRight = false;
			}	
		}
	}
	else m_isMoving = false;

	//���� ����
	if (m_pMap)
		m_pMap->GetHeight(m_vPosition.x, m_vPosition.y, m_vPosition.z);

	//�̵� ��Ʈ������ ����
	D3DXMATRIXA16 matT;
	D3DXMatrixTranslation(&matT, m_vPosition.x, m_vPosition.y, m_vPosition.z);
	m_matWorld = m_matRotY * matT;
}

void cNPCController::MoveSurround()
{

}

//NPC�� ĳ���� �������� �ȿ� �ִ��� Ȯ���ϴ� �Լ�
bool cNPCController::IsInRecognizeRange()
{
	D3DXVECTOR3 vDis = m_vPosition - m_vCharacterPosition;
	float fDis = D3DXVec3Length(&vDis);

	if (fDis <= DISTANCE_RECOGNIZE + D3DX_16F_EPSILON)
	{
		return true;
	}

	return false;
}

//NPC ��ȭ&����Ʈ ���� �Լ�
void cNPCController::SetTalkTextBox(string Name, string Dialouge, bool isDialouge)	//��ȭUI �����Լ�
{
	m_pUITextBox->InitializeString(Name, Dialouge, isDialouge);
}

void cNPCController::LookAt(D3DXVECTOR3* vDir)	//NPC�� ĳ���� �ٶ�
{
	if (m_isLookAt)
	{
		m_vFromDir = *vDir;

		D3DXVECTOR3 vPos = m_vCharacterPosition;
		if (m_vCharacterPosition.y != m_vPosition.y)
			vPos.y = m_vPosition.y;

		m_vLookDir = vPos - m_vPosition;
		m_vToDir = m_vLookDir;

		SmoothTurn(&m_vDir, &m_vFromDir, &m_vToDir);

		if (m_isSmoothTurn)
			m_isShowTalkUI = true;	//��ȭâUI on
	}
	else
	{
		if (m_isEndTalk)
		{
			m_vFromDir = m_vLookDir;
			m_vToDir = *vDir;
			SmoothTurn(&m_vDir, &m_vFromDir, &m_vToDir);
		}
	}

	D3DXMatrixLookAtLH(&m_matRotY, &D3DXVECTOR3(0, 0, 0), &m_vDir, &D3DXVECTOR3(0, 1, 0));
	D3DXMatrixTranspose(&m_matRotY, &m_matRotY);
}

void cNPCController::SmoothTurn(OUT D3DXVECTOR3* vDir, D3DXVECTOR3* vFrom, D3DXVECTOR3* vTo)	//�ε巴�� ȸ�� ����
{
	if (m_isSmoothTurn) return;

	//���� �ð� ����
	if (D3DXVec3Dot(vFrom, vTo) < 0)
		m_fActionTime = 1.0f;
	else if (D3DXVec3Dot(vFrom, vTo) >= 0)
		m_fActionTime = 0.5f;

	// ���� ����
	m_fPassedTime += g_pTimeManager->getElapsedTime();
	float t = m_fPassedTime / m_fActionTime;

	D3DXVec3Lerp(vDir, &m_vFromDir, &m_vToDir, t);

	if (m_fPassedTime >= m_fActionTime)
	{
		m_fPassedTime = 0.0f;

		if (m_isLookAt)
			m_isSmoothTurn = true;

		if (m_isEndTalk)
			m_isEndTalk = false;
	}
}

void cNPCController::Talk()		//��ȭUI Update
{
	if (m_pUITextBox && m_isShowTalkUI)
	{
		m_pUITextBox->SetisDialouge(true);
		m_pUITextBox->Update();

		if (m_pUITextBox->GetIsOk())
		{
			m_isLookAt = false;			//ĳ���� �Ĵٺ��� ����
			m_isSmoothTurn = false;
			m_isShowTalkUI = false;		//��ȭâUI off
			m_isEndTalk = true;
			m_isClear = true;

			m_pUITextBox->SetisDialouge(false);
			m_pUITextBox->SetisOK(false);
		}
	}
}

//NPC ���� ���� �Լ�: �ִϸ��̼� ����
void cNPCController::ManageNPCState()
{
	if (m_isMoving)
	{
		if(m_nRandomWalk == 0)
			m_dwState = E_WALK_FRONT;
		else if (m_nRandomWalk == 1)
			m_dwState = E_WALK2;
		else if (m_nRandomWalk == 2)
			m_dwState = E_WALK3;
	}
	else if (m_isYes)
	{
		Yes();
	}
	else if (m_isNo)
	{
		No();
	}
	else if (m_isSit)
	{
		Sit();
	}
	else if (m_isSleep)
	{
		Sleep();
	}
	else if (m_isShowTalkUI)
	{
		m_dwState = E_ON_THE_GROUND;
	}
	else
	{
		Idle();
	}
}

//Ư�� ���� ���� �Լ�
void cNPCController::Idle()
{
	float fDeltaTime = g_pDeltaTimeManager->GetDeltaTime();

	if (m_fStackDeltaTime < 1.5)
	{
		m_fStackDeltaTime += fDeltaTime;
		m_dwState = E_ON_THE_GROUND;
	}
	else if (m_fStackDeltaTime < 4.5)
	{
		m_fStackDeltaTime += fDeltaTime;
		m_dwState = E_IDLE;
	}
	else
	{
		m_fStackDeltaTime = 0.0f;
	}
}

void cNPCController::Yes()
{
	if (!m_isYes) return;

	float fDeltaTime = g_pDeltaTimeManager->GetDeltaTime();

	if (m_fStackDeltaTime < 1.0)
	{
		m_fStackDeltaTime += fDeltaTime;
		m_isYes = true;
		m_dwState = E_YES;
	}
	else
	{
		m_isYes = false;
		m_fStackDeltaTime = 0.0f;
	}
}

void cNPCController::No()
{
	if (!m_isNo) return;

	float fDeltaTime = g_pDeltaTimeManager->GetDeltaTime();

	if (m_fStackDeltaTime < 1.0)
	{
		m_fStackDeltaTime += fDeltaTime;
		m_isNo = true;
		m_dwState = E_NO;
	}
	else
	{
		m_isNo = false;
		m_fStackDeltaTime = 0.0f;
	}
}

void cNPCController::Sit()
{
	if (!m_isSit) return;
	m_dwState = E_SIT;

	m_isMoving = false;
	m_isYes = false;
	m_isNo = false;
	m_isSleep = false;
}

void cNPCController::Sleep()
{
	if (!m_isSleep) return;
	m_dwState = E_SLEEP;

	m_isMoving = false;
	m_isYes = false;
	m_isNo = false;
	m_isSit = false;
}

//����� ���� �Լ�
void cNPCController::DebugRender_NPC()
{
	if (g_pKeyManager->isOnceKeyDown('4'))
	{
		m_isOnKey4 = !m_isOnKey4;
	}

	if (m_isOnKey4)
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
		}

		char szStr2[1024];
		sprintf(szStr2, "\nX: %f\nY: %f\nZ: %f\nspeed: %f", m_vPosition.x, m_vPosition.y, m_vPosition.z, m_fMoveSpeed);

		strcat(szStr, szStr2);

		if (m_canTalk)
			sprintf(szStr2, "\ncanTalk: true");
		else sprintf(szStr2, "\ncanTalk: false");

		strcat(szStr, szStr2);

		m_pFont->DrawTextA(NULL, szStr, strlen(szStr),
			m_pRect, DT_TOP | DT_LEFT | DT_NOCLIP, D3DCOLOR_XRGB(255, 0, 0));

		g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		g_pD3DDevice->SetTexture(0, NULL);

		D3DXMATRIXA16 matW;
		D3DXMatrixIdentity(&matW);
		D3DXMatrixTranslation(&matW, m_vPosition.x, m_vPosition.y, m_vPosition.z);
		g_pD3DDevice->SetTransform(D3DTS_WORLD, &matW);
		g_pD3DDevice->SetMaterial(&WHITE_MTRL);
		m_pSphereRange->DrawSubset(0);

		g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}
}