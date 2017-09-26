#include "stdafx.h"
#include "cAseCharacter.h"
#include "cAseLoader.h"


cAseCharacter::cAseCharacter()
	: m_pRoot(NULL)
{
}


cAseCharacter::~cAseCharacter()
{
}

void cAseCharacter::Load(char* szFullPath)
{
	cAseLoader* pAseLoader = new cAseLoader;
	m_pRoot = pAseLoader->Load(szFullPath, &m_stScene);
	SAFE_RELEASE(pAseLoader);
}

void cAseCharacter::Update(D3DXMATRIXA16* matWorld/*= NULL*/)
{
	/*
	ST_ASE_SCENE 정보를 사용해서 키프레임 계산
	*/
	int nFirstTick = m_stScene.nFirstFrame * m_stScene.nTicksPerFrame;
	int nLastTick = m_stScene.nLastFrame * m_stScene.nTicksPerFrame;
	int nTickSpeed = m_stScene.nFrameSpeed * m_stScene.nTicksPerFrame;
	int nKeyFrame = (nTickSpeed * GetTickCount() / 1000) % (nLastTick - nFirstTick) + nFirstTick;
	// 640 ~ 3199 반복된다.

	/*
	루트의 부모 설정(캐릭터의 크기, 시작시 바라보는 방향)
	*/
	D3DXMATRIXA16 matS, matR, matW;
	D3DXMatrixScaling(&matS, 1.0f, 1.0f, 1.0f);
	D3DXMatrixRotationY(&matR, D3DX_PI);
	matW = matS * matR * *matWorld;

	if (m_pRoot)
		m_pRoot->Update(nKeyFrame, &matW);
}

void cAseCharacter::Render()
{
	if (m_pRoot)
		m_pRoot->Render();
}
