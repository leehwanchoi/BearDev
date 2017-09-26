#include "stdafx.h"
#include "cCubeMan.h"
#include "cCube.h"


cCubeMan::cCubeMan()
	: m_pRoot(NULL)
	, m_pTexture(NULL)
{
}


cCubeMan::~cCubeMan()
{
}

void cCubeMan::Setup()
{
	g_pTextureManager->AddTexture("batman", "textures/batman.png", &m_pTexture);

	D3DXMATRIXA16 matW, matS, matT;
	cCube* pCube;

	// �� ������ ������ �� ��ġ �����ؼ� ť�긦 �����Ѵ�.

	vector<D3DXVECTOR2> vecTexture;
	RECT rc;

	// ����
	rc.left = 8;
	rc.top = 5;
	rc.right = 10;
	rc.bottom = 8;
	SetTextureVertex(OUT vecTexture, IN rc);

	rc.left = 7;
	rc.right = 8;
	SetTextureVertex(OUT vecTexture, IN rc);

	rc.left = 5;
	rc.right = 7;
	SetTextureVertex(OUT vecTexture, IN rc);

	rc.left = 4;
	rc.right = 5;
	SetTextureVertex(OUT vecTexture, IN rc);

	rc.left = 7;
	rc.top = 4;
	rc.right = 9;
	rc.bottom = 5;
	SetTextureVertex(OUT vecTexture, IN rc);

	rc.left = 5;
	rc.right = 7;
	SetTextureVertex(OUT vecTexture, IN rc);

	D3DXMatrixScaling(&matS, 1.0f, 1.0f, 0.5f);
	matW = matS;
	pCube = new cCube;
	pCube->AutoRelease();
	pCube->Setup(&matW, &vecTexture, m_pTexture, NULL);
	pCube->SetPosition(D3DXVECTOR3(0.0f, 3.0f, 0.0f));
	m_pRoot = pCube;
	vecTexture.clear();

	// �Ӹ�
	rc.left = 6;
	rc.top = 2;
	rc.right = 8;
	rc.bottom = 4;
	SetTextureVertex(OUT vecTexture, IN rc);

	rc.left = 4;
	rc.right = 6;
	SetTextureVertex(OUT vecTexture, IN rc);

	rc.left = 2;
	rc.right = 4;
	SetTextureVertex(OUT vecTexture, IN rc);

	rc.left = 0;
	rc.right = 2;
	SetTextureVertex(OUT vecTexture, IN rc);

	rc.left = 4;
	rc.top = 0;
	rc.right = 6;
	rc.bottom = 2;
	SetTextureVertex(OUT vecTexture, IN rc);

	rc.left = 2;
	rc.right = 4;
	SetTextureVertex(OUT vecTexture, IN rc);

	D3DXMatrixScaling(&matS, 0.7f, 0.7f, 0.7f);
	matW = matS;
	pCube = new cCube;
	pCube->AutoRelease();
	pCube->Setup(&matW, &vecTexture, m_pTexture, NULL);
	pCube->SetPosition(D3DXVECTOR3(0.0f, 1.7f, 0.0f));
	pCube->SetRotYSpeed(0.1f);
	m_pRoot->AddChild(pCube);
	m_pHead = pCube;
	vecTexture.clear();

	// ����
	rc.left = 13;
	rc.top = 5;
	rc.right = 14;
	rc.bottom = 8;
	SetTextureVertex(OUT vecTexture, IN rc);

	rc.left = 12;
	rc.right = 13;
	SetTextureVertex(OUT vecTexture, IN rc);

	rc.left = 11;
	rc.right = 12;
	SetTextureVertex(OUT vecTexture, IN rc);

	rc.left = 10;
	rc.right = 11;
	SetTextureVertex(OUT vecTexture, IN rc);

	rc.left = 12;
	rc.top = 4;
	rc.right = 13;
	rc.bottom = 5;
	SetTextureVertex(OUT vecTexture, IN rc);

	rc.left = 11;
	rc.right = 12;
	SetTextureVertex(OUT vecTexture, IN rc);

	D3DXMATRIXA16 matR;
	D3DXMatrixScaling(&matS, 0.5f, 1.0f, 0.5f);
	D3DXMatrixRotationY(&matR, D3DX_PI);
	matW = matS * matR;
	pCube = new cCube;
	pCube->AutoRelease();
	pCube->Setup(&matW, &vecTexture, m_pTexture, NULL);
	pCube->SetPosition(D3DXVECTOR3(-1.5f, 0.0f, 0.0f));
	pCube->SetRotXSpeed(0.1f);
	pCube->SetStdRotPosition(D3DXVECTOR3(0.0f, -1.0f, 0.0f));
	m_pRoot->AddChild(pCube);
	vecTexture.clear();

	// ������
	rc.left = 13;
	rc.top = 5;
	rc.right = 14;
	rc.bottom = 8;
	SetTextureVertex(OUT vecTexture, IN rc);

	rc.left = 12;
	rc.right = 13;
	SetTextureVertex(OUT vecTexture, IN rc);

	rc.left = 11;
	rc.right = 12;
	SetTextureVertex(OUT vecTexture, IN rc);

	rc.left = 10;
	rc.right = 11;
	SetTextureVertex(OUT vecTexture, IN rc);

	rc.left = 12;
	rc.top = 4;
	rc.right = 13;
	rc.bottom = 5;
	SetTextureVertex(OUT vecTexture, IN rc);

	rc.left = 11;
	rc.right = 12;
	SetTextureVertex(OUT vecTexture, IN rc);

	D3DXMatrixScaling(&matS, 0.5f, 1.0f, 0.5f);
	matW = matS;
	pCube = new cCube;
	pCube->AutoRelease();
	pCube->Setup(&matW, &vecTexture, m_pTexture, NULL);
	pCube->SetPosition(D3DXVECTOR3(1.5f, 0.0f, 0.0f));
	pCube->SetRotXSpeed(-0.1f);
	pCube->SetStdRotPosition(D3DXVECTOR3(0.0f, -1.0f, 0.0f));
	m_pRoot->AddChild(pCube);
	vecTexture.clear();

	// ���� �ٸ�
	rc.left = 3;
	rc.top = 5;
	rc.right = 4;
	rc.bottom = 8;
	SetTextureVertex(OUT vecTexture, IN rc);

	rc.left = 2;
	rc.right = 3;
	SetTextureVertex(OUT vecTexture, IN rc);

	rc.left = 1;
	rc.right = 2;
	SetTextureVertex(OUT vecTexture, IN rc);

	rc.left = 0;
	rc.right = 1;
	SetTextureVertex(OUT vecTexture, IN rc);

	rc.left = 2;
	rc.top = 4;
	rc.right = 3;
	rc.bottom = 5;
	SetTextureVertex(OUT vecTexture, IN rc);

	rc.left = 1;
	rc.right = 2;
	SetTextureVertex(OUT vecTexture, IN rc);

	D3DXMatrixScaling(&matS, 0.5f, 1.0f, 0.5f);
	matW = matS;
	pCube = new cCube;
	pCube->AutoRelease();
	pCube->Setup(&matW, &vecTexture, m_pTexture, NULL);
	pCube->SetPosition(D3DXVECTOR3(-0.5f, -2.0f, 0.0f));
	pCube->SetRotXSpeed(-0.1f);
	pCube->SetStdRotPosition(D3DXVECTOR3(0.0f, -2.0f, 0.0f));
	m_pRoot->AddChild(pCube);
	vecTexture.clear();

	// ������ �ٸ�
	rc.left = 3;
	rc.top = 5;
	rc.right = 4;
	rc.bottom = 8;
	SetTextureVertex(OUT vecTexture, IN rc);

	rc.left = 2;
	rc.right = 3;
	SetTextureVertex(OUT vecTexture, IN rc);

	rc.left = 1;
	rc.right = 2;
	SetTextureVertex(OUT vecTexture, IN rc);

	rc.left = 0;
	rc.right = 1;
	SetTextureVertex(OUT vecTexture, IN rc);

	rc.left = 2;
	rc.top = 4;
	rc.right = 3;
	rc.bottom = 5;
	SetTextureVertex(OUT vecTexture, IN rc);

	rc.left = 1;
	rc.right = 2;
	SetTextureVertex(OUT vecTexture, IN rc);

	D3DXMatrixScaling(&matS, 0.5f, 1.0f, 0.5f);
	matW = matS;
	pCube = new cCube;
	pCube->AutoRelease();
	pCube->Setup(&matW, &vecTexture, m_pTexture, NULL);
	pCube->SetPosition(D3DXVECTOR3(0.5f, -2.0f, 0.0f));
	pCube->SetRotXSpeed(0.1f);
	pCube->SetStdRotPosition(D3DXVECTOR3(0.0f, -2.0f, 0.0f));
	m_pRoot->AddChild(pCube);
	vecTexture.clear();
}

void cCubeMan::SetTextureVertex(vector<D3DXVECTOR2>& vecTexture, RECT rcCoord)
{
	vecTexture.push_back(D3DXVECTOR2(rcCoord.left / 16.0f, rcCoord.bottom / 8.0f));
	vecTexture.push_back(D3DXVECTOR2(rcCoord.left / 16.0f, rcCoord.top / 8.0f));
	vecTexture.push_back(D3DXVECTOR2(rcCoord.right / 16.0f, rcCoord.top / 8.0f));
	vecTexture.push_back(D3DXVECTOR2(rcCoord.left / 16.0f, rcCoord.bottom / 8.0f));
	vecTexture.push_back(D3DXVECTOR2(rcCoord.right / 16.0f, rcCoord.top / 8.0f));
	vecTexture.push_back(D3DXVECTOR2(rcCoord.right / 16.0f, rcCoord.bottom / 8.0f));
}

void cCubeMan::Update(bool isMoving, D3DXMATRIXA16* mat/*= NULL*/)
{
	if (m_pRoot)
		m_pRoot->Update(isMoving, mat);
}

void cCubeMan::Render()
{
	// ���� �ɼ��� ���ش�.
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, false);

	if (m_pRoot)
		m_pRoot->Render();

	// DX������ �⺻������ ���� �ɼ��� Ű�� �����ϴ� ��찡 ���� ������ ���� ��� �ٷ� �ٽ� ���ش�.
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, true);
}

float cCubeMan::GetHeadRotation()
{
	return m_pHead->GetRotY();
}
