#include "stdafx.h"
#include "cAnimationTexture.h"


cAnimationTexture::cAnimationTexture()
	: m_pTexture(NULL)
	, m_fX(0)
	, m_fY(0)
	, m_isPause(false)
	, m_isFront(true)
	, m_nFullFrame(0)
	, m_fOneFrame(0)
	, m_fTime(0.0f)
	, m_fSetTime(0.0f)
{
	D3DXMatrixIdentity(&m_matW);
}


cAnimationTexture::~cAnimationTexture()
{
}

void cAnimationTexture::Load(char* lpFileName, int FrameNum)
{
	g_pTextureManager->AddTexture(lpFileName, lpFileName, &m_pTexture);
	m_nFullFrame = FrameNum;
	m_fOneFrame = 1.0f / m_nFullFrame;
	m_fSetTime = 1.0f / m_nFullFrame;

	ST_PT_VERTEX stVertex[4];
	stVertex[0].p = D3DXVECTOR3(-3, -3, 0);
	stVertex[0].t = D3DXVECTOR2(0, 1);

	stVertex[1].p = D3DXVECTOR3(-3, 3, 0);
	stVertex[1].t = D3DXVECTOR2(0, 0);

	stVertex[2].p = D3DXVECTOR3(3, 3, 0);
	stVertex[2].t = D3DXVECTOR2(m_fOneFrame, 0);

	stVertex[3].p = D3DXVECTOR3(3, -3, 0);
	stVertex[3].t = D3DXVECTOR2(m_fOneFrame, 1);

	m_vecPTVertex.push_back(stVertex[0]);
	m_vecPTVertex.push_back(stVertex[1]);
	m_vecPTVertex.push_back(stVertex[2]);

	m_vecPTVertex.push_back(stVertex[0]);
	m_vecPTVertex.push_back(stVertex[2]);
	m_vecPTVertex.push_back(stVertex[3]);

	//이미지를 변경하기 위해 좌표값 따로 저장
	m_textureCoor[0] = stVertex[0].t;
	m_textureCoor[1] = stVertex[1].t;
	m_textureCoor[2] = stVertex[2].t;
	m_textureCoor[3] = stVertex[3].t;
}

void cAnimationTexture::Update()
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

void cAnimationTexture::Render()
{
	g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);		// 알파 블렌드 옵션 킨다.
	g_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);		// Z버퍼(깊이 버퍼)에 의해서 그리는 순서가 정해지는 옵션을 끈다. : 반투명을 표현할 때 속도 향상 효과

	g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	g_pD3DDevice->SetFVF(ST_PT_VERTEX::FVF);
	g_pD3DDevice->SetMaterial(&WHITE_MTRL);
	g_pD3DDevice->SetTexture(0, m_pTexture);
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, false);
	g_pD3DDevice->SetTransform(D3DTS_WORLD, &m_matW);

	g_pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_vecPTVertex.size() / 3,
		&m_vecPTVertex[0], sizeof(ST_PT_VERTEX));

	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, true);
	g_pD3DDevice->SetFVF(ST_PNT_VERTEX::FVF);
	g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);	// 기본 값 복구
	g_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);			// 기본 값 복구
}

void cAnimationTexture::PrevImage()
{
	//한 프레임 감소
	m_textureCoor[0].x -= m_fOneFrame;
	m_textureCoor[1].x -= m_fOneFrame;
	m_textureCoor[2].x -= m_fOneFrame;
	m_textureCoor[3].x -= m_fOneFrame;

	//맨 처음 프레임이면 마지막 프레임으로
	if (m_textureCoor[3].x <= 0)
	{
		m_textureCoor[0].x = 1.0f - m_fOneFrame;
		m_textureCoor[1].x = 1.0f - m_fOneFrame;
		m_textureCoor[2].x = 1.0f;
		m_textureCoor[3].x = 1.0f;
	}

	//프레임 적용
	m_vecPTVertex[0].t.x = m_textureCoor[0].x;
	m_vecPTVertex[1].t.x = m_textureCoor[1].x;
	m_vecPTVertex[2].t.x = m_textureCoor[2].x;

	m_vecPTVertex[3].t.x = m_textureCoor[0].x;
	m_vecPTVertex[4].t.x = m_textureCoor[2].x;
	m_vecPTVertex[5].t.x = m_textureCoor[3].x;
}

void cAnimationTexture::NextImage()
{
	//한 프레임 증가
	m_textureCoor[0].x += m_fOneFrame;
	m_textureCoor[1].x += m_fOneFrame;
	m_textureCoor[2].x += m_fOneFrame;
	m_textureCoor[3].x += m_fOneFrame;

	//마지막 프레임이면 처음으로
	if (m_textureCoor[0].x >= 1.0f)
	{
		m_textureCoor[0].x = 0;
		m_textureCoor[1].x = 0;
		m_textureCoor[2].x = m_fOneFrame;
		m_textureCoor[3].x = m_fOneFrame;
	}

	//프레임 적용
	m_vecPTVertex[0].t.x = m_textureCoor[0].x;
	m_vecPTVertex[1].t.x = m_textureCoor[1].x;
	m_vecPTVertex[2].t.x = m_textureCoor[2].x;

	m_vecPTVertex[3].t.x = m_textureCoor[0].x;
	m_vecPTVertex[4].t.x = m_textureCoor[2].x;
	m_vecPTVertex[5].t.x = m_textureCoor[3].x;
}