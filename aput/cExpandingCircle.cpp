#include "stdafx.h"
#include "cExpandingCircle.h"


cExpandingCircle::cExpandingCircle()
	: m_pTexture(NULL)
	, m_fX(0)
	, m_fY(0)
	, m_isPause(false)
	, m_isFront(true)
	, m_nFullFrame(8)
	, m_nOneFrame(1)
	, m_fTime(0.0f)
	, m_fSetTime(0.0f)
	, m_fRotY(0.0f)
{
	D3DXMatrixIdentity(&m_matW);
}


cExpandingCircle::~cExpandingCircle()
{
	SAFE_RELEASE(m_pTexture);
}

void cExpandingCircle::Setup()
{
	g_pTextureManager->AddTexture("expanding_circle0", "./Textures/sci-fi-effects/expanding_circle/00.png", &m_pTexture);
	g_pTextureManager->AddTexture("expanding_circle1", "./Textures/sci-fi-effects/expanding_circle/01.png", NULL);
	g_pTextureManager->AddTexture("expanding_circle2", "./Textures/sci-fi-effects/expanding_circle/02.png", NULL);
	g_pTextureManager->AddTexture("expanding_circle3", "./Textures/sci-fi-effects/expanding_circle/03.png", NULL);
	g_pTextureManager->AddTexture("expanding_circle4", "./Textures/sci-fi-effects/expanding_circle/04.png", NULL);
	g_pTextureManager->AddTexture("expanding_circle5", "./Textures/sci-fi-effects/expanding_circle/05.png", NULL);
	g_pTextureManager->AddTexture("expanding_circle6", "./Textures/sci-fi-effects/expanding_circle/06.png", NULL);
	g_pTextureManager->AddTexture("expanding_circle7", "./Textures/sci-fi-effects/expanding_circle/07.png", NULL);

	m_fSetTime = 1.0f / m_nFullFrame;

	ST_PT_VERTEX stVertex[4];
	stVertex[0].p = D3DXVECTOR3(-3, -3, 0);
	stVertex[0].t = D3DXVECTOR2(0, 1);

	stVertex[1].p = D3DXVECTOR3(-3, 3, 0);
	stVertex[1].t = D3DXVECTOR2(0, 0);

	stVertex[2].p = D3DXVECTOR3(3, 3, 0);
	stVertex[2].t = D3DXVECTOR2(1, 0);

	stVertex[3].p = D3DXVECTOR3(3, -3, 0);
	stVertex[3].t = D3DXVECTOR2(1, 1);

	m_vecPTVertex.push_back(stVertex[0]);
	m_vecPTVertex.push_back(stVertex[1]);
	m_vecPTVertex.push_back(stVertex[2]);

	m_vecPTVertex.push_back(stVertex[0]);
	m_vecPTVertex.push_back(stVertex[2]);
	m_vecPTVertex.push_back(stVertex[3]);
}

void cExpandingCircle::Update()
{
	//시간 저장
	m_fTime += g_pTimeManager->getElapsedTime();

	//정지가 아니고
	if (!m_isPause)
	{
		//시간이 설정시간보다 커지면
		if (m_fSetTime < m_fTime)
		{
			//앞으로 재생이면
			if (m_isFront)
				NextImage();

			//뒤로 재생이면
			else
				PrevImage();

			m_fTime = 0;
		}
	}
}

void cExpandingCircle::Render()
{
	g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);		// 알파 블렌드 옵션 킨다.
	g_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);		// Z버퍼(깊이 버퍼)에 의해서 그리는 순서가 정해지는 옵션을 끈다. : 반투명을 표현할 때 속도 향상 효과

	g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	g_pD3DDevice->SetFVF(ST_PT_VERTEX::FVF);
	g_pD3DDevice->SetMaterial(&WHITE_MTRL);
	g_pD3DDevice->SetTexture(0, m_pTexture);
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, false);

	/*==========================================================================*/

	g_pD3DDevice->SetTransform(D3DTS_WORLD, &m_matW);
	g_pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_vecPTVertex.size() / 3,
		&m_vecPTVertex[0], sizeof(ST_PT_VERTEX));

	/*==========================================================================*/

	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, true);
	g_pD3DDevice->SetFVF(ST_PNT_VERTEX::FVF);
	g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);	// 기본 값 복구
	g_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);			// 기본 값 복구
}

void cExpandingCircle::PrevImage()
{
	m_nOneFrame--;

	if (m_nOneFrame < 0)
		m_nOneFrame = m_nFullFrame - 1; //이미지 전체 갯수 - 1 (0부터 시작하기때문)

	m_pTexture = g_pTextureManager->GetTexture("expanding_circle" + to_string(m_nOneFrame));
}

void cExpandingCircle::NextImage()
{
	m_nOneFrame++;

	if (m_nOneFrame > m_nFullFrame - 1) //이미지 전체 갯수 - 1 (0부터 시작하기때문)
		m_nOneFrame = 0;

	m_pTexture = g_pTextureManager->GetTexture("expanding_circle" + to_string(m_nOneFrame));
}
