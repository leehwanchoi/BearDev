#include "stdafx.h"
#include "cAlphaBlendTest.h"


cAlphaBlendTest::cAlphaBlendTest()
{
	D3DXMatrixIdentity(&m_matWorld);
}

cAlphaBlendTest::~cAlphaBlendTest()
{
}

void cAlphaBlendTest::Setup()
{
	vector<D3DXVECTOR3> vecV;

	vecV.push_back(D3DXVECTOR3(-1,-1, 1));
	vecV.push_back(D3DXVECTOR3(-1, 1, 1));
	vecV.push_back(D3DXVECTOR3( 1, 1, 1));
	vecV.push_back(D3DXVECTOR3( 1,-1, 1));

	SetPCPlane(vecV, D3DXVECTOR3(10, 10, 10), D3DXVECTOR3(5, 15, 5), D3DCOLOR_RGBA(0, 255, 0, 128));
	SetPCPlane(vecV, D3DXVECTOR3(10, 10, 10), D3DXVECTOR3(10, 20, 10), D3DCOLOR_RGBA(255, 0, 0, 128));

	g_pTextureManager->AddTexture("alpha_box.dds", "textures/alpha_box.dds", NULL);
	g_pTextureManager->AddTexture("box.jpg", "textures/box.jpg", NULL);
	
	SetPTPlane(vecV, D3DXVECTOR3(10, 10, 10), D3DXVECTOR3(15, 25, 15));
	SetPNTPlane(vecV, D3DXVECTOR3(10, 10, 10), D3DXVECTOR3(20, 30, 20));
}

void cAlphaBlendTest::SetPCPlane(vector<D3DXVECTOR3>& vecV, D3DXVECTOR3 s, D3DXVECTOR3 p, D3DCOLOR c)
{
	vector<D3DXVECTOR3> vecTrans(6);

	D3DXMATRIXA16 matS, matT, LocalTM;

	D3DXMatrixScaling(&matS, s.x, s.y, s.z);
	D3DXMatrixTranslation(&matT, p.x, p.y, p.z);
	LocalTM = matS * matT;

	for (int i = 0; i < vecV.size(); ++i)
	{
		D3DXVec3TransformCoord(&vecTrans[i], &vecV[i], &LocalTM);
	}

	m_vecPCVertex.push_back(ST_PC_VERTEX(vecTrans[0], c));
	m_vecPCVertex.push_back(ST_PC_VERTEX(vecTrans[1], c));
	m_vecPCVertex.push_back(ST_PC_VERTEX(vecTrans[2], c));
	m_vecPCVertex.push_back(ST_PC_VERTEX(vecTrans[0], c));
	m_vecPCVertex.push_back(ST_PC_VERTEX(vecTrans[2], c));
	m_vecPCVertex.push_back(ST_PC_VERTEX(vecTrans[3], c));
}

void cAlphaBlendTest::SetPTPlane(vector<D3DXVECTOR3>& vecV, D3DXVECTOR3 s, D3DXVECTOR3 p)
{
	vector<D3DXVECTOR3> vecTrans(6);

	D3DXMATRIXA16 matS, matT, LocalTM;

	D3DXMatrixScaling(&matS, s.x, s.y, s.z);
	D3DXMatrixTranslation(&matT, p.x, p.y, p.z);
	LocalTM = matS * matT;

	for (int i = 0; i < vecV.size(); ++i)
	{
		D3DXVec3TransformCoord(&vecTrans[i], &vecV[i], &LocalTM);
	}

	m_vecPTVertex.push_back(ST_PT_VERTEX(vecTrans[0], D3DXVECTOR2(0, 1)));
	m_vecPTVertex.push_back(ST_PT_VERTEX(vecTrans[1], D3DXVECTOR2(0, 0)));
	m_vecPTVertex.push_back(ST_PT_VERTEX(vecTrans[2], D3DXVECTOR2(1, 0)));
	m_vecPTVertex.push_back(ST_PT_VERTEX(vecTrans[0], D3DXVECTOR2(0, 1)));
	m_vecPTVertex.push_back(ST_PT_VERTEX(vecTrans[2], D3DXVECTOR2(1, 0)));
	m_vecPTVertex.push_back(ST_PT_VERTEX(vecTrans[3], D3DXVECTOR2(1, 1)));
}

void cAlphaBlendTest::SetPNTPlane(vector<D3DXVECTOR3>& vecV, D3DXVECTOR3 s, D3DXVECTOR3 p)
{
	vector<D3DXVECTOR3> vecTrans(6);

	D3DXMATRIXA16 matS, matT, LocalTM;

	D3DXMatrixScaling(&matS, s.x, s.y, s.z);
	D3DXMatrixTranslation(&matT, p.x, p.y, p.z);
	LocalTM = matS * matT;

	for (int i = 0; i < vecV.size(); ++i)
	{
		D3DXVec3TransformCoord(&vecTrans[i], &vecV[i], &LocalTM);
	}

	D3DXVECTOR3 n;
	D3DXVec3Cross(&n, &(vecTrans[1] - vecTrans[0]), &(vecTrans[2] - vecTrans[0]));

	m_vecPNTVertex.push_back(ST_PNT_VERTEX(vecTrans[0], n, D3DXVECTOR2(0, 1)));
	m_vecPNTVertex.push_back(ST_PNT_VERTEX(vecTrans[1], n, D3DXVECTOR2(0, 0)));
	m_vecPNTVertex.push_back(ST_PNT_VERTEX(vecTrans[2], n, D3DXVECTOR2(1, 0)));
	m_vecPNTVertex.push_back(ST_PNT_VERTEX(vecTrans[0], n, D3DXVECTOR2(0, 1)));
	m_vecPNTVertex.push_back(ST_PNT_VERTEX(vecTrans[2], n, D3DXVECTOR2(1, 0)));
	m_vecPNTVertex.push_back(ST_PNT_VERTEX(vecTrans[3], n, D3DXVECTOR2(1, 1)));
}

void cAlphaBlendTest::Render()
{
	g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);		// 알파 블렌드 옵션 킨다.
	g_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);		// Z버퍼(깊이 버퍼)에 의해서 그리는 순서가 정해지는 옵션을 끈다. : 반투명을 표현할 때 속도 향상 효과

	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, false);

	// D3DBLEND_ZERO : (0, 0, 0, 0)
	// D3DBLEND_ONE : (1, 1, 1, 1)
	// D3DBLEND_SRCCOLOR : (rs, gs, bs, as)
	// D3DBLEND_INVSRCCOLOR : (1-rs, 1-gs, 1-bs, 1-as)
	// D3DBLEND_SRCALPHA : (as, as, as, as) (Source Blend 기본값)
	// D3DBLEND_INVSRCALPHA : (1-as, 1-as, 1-as, 1-as) (Destination Blend 기본값)
	// D3DBLEND_DESTALPHA : (ad, ad, ad, ad)
	// D3DBLEND_INVDESTALPHA : (1-ad, 1-ad, 1-ad, 1-ad)
	// D3DBLEND_DESTCOLOR : (rd, gd, bd, ad)
	// D3DBLEND_INVDESTCCOLOR : (1-rd, 1-gd, 1-bd, 1-ad)
	// D3DBLEND_SRCALPHASAT : (f, f, f, 1); f=min(as, 1-ad)
	g_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);	// 소스에 그릴 때(백버퍼) 설정 된 블렌드 옵션으로 그린다.
	g_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);// 데스트에 그릴 때(실제 화면) 설정 된 블렌드 옵션으로 그린다.

	// D3DBLENDOP_ADD (기본값)	: src + dest
	// D3DBLENDOP_SUBTRACT		: src - dest
	// D3DBLENDOP_REVSUBTRACT	: dest - src
	// D3DBLENDOP_MIN			: MIN(src, dest)
	// D3DBLENDOP_MAX			: MAX(src, dest)
	g_pD3DDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);	// 블렌딩 방식을 결정

	g_pD3DDevice->SetTransform(D3DTS_WORLD, &m_matWorld);

	if (!m_vecPCVertex.empty())
	{
		g_pD3DDevice->SetTexture(0, NULL);
		g_pD3DDevice->SetFVF(ST_PC_VERTEX::FVF);
		g_pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_vecPCVertex.size() / 3,
			&m_vecPCVertex[0], sizeof(ST_PC_VERTEX));
	}

	// 텍스쳐에 있는 알파값을 사용하도록 설정
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	if (!m_vecPTVertex.empty())
	{
		g_pD3DDevice->SetTexture(0, g_pTextureManager->GetTexture("alpha_box.dds"));
		g_pD3DDevice->SetFVF(ST_PT_VERTEX::FVF);
		g_pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_vecPTVertex.size() / 3,
			&m_vecPTVertex[0], sizeof(ST_PT_VERTEX));
	}

	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, true);

	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);

	if (!m_vecPNTVertex.empty())
	{
		g_pD3DDevice->SetMaterial(&REDA_MTRL);
		g_pD3DDevice->SetTexture(0, g_pTextureManager->GetTexture("box.jpg"));
		g_pD3DDevice->SetFVF(ST_PNT_VERTEX::FVF);
		g_pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_vecPNTVertex.size() / 3,
			&m_vecPNTVertex[0], sizeof(ST_PNT_VERTEX));
	}

	g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);	// 기본 값 복구
	g_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);			// 기본 값 복구
}