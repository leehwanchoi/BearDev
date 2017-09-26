#include "stdafx.h"
#include "cSkyBox.h"


cSkyBox::cSkyBox()
{
	m_pVB = NULL;
	for (int i = 0; i < 6; ++i)
	{
		m_pTex[i] = NULL;
	}
}


cSkyBox::~cSkyBox()
{
	if (m_pVB != NULL)
		m_pVB->Release();
}

void cSkyBox::CreateSkyVertex()
{
	if ((g_pD3DDevice->CreateVertexBuffer(24 * sizeof(SKYVERTEX), 0, D3DFVF_SKYVERTEX,
		D3DPOOL_DEFAULT, &m_pVB, NULL)))
	{
		if (m_pVB != NULL)
			m_pVB->Release();
		for (int i = 0; i < 6; ++i)
		{
			if (m_pTex[i] != NULL)
				m_pTex[i]->Release();
		}
		
	}
	SKYVERTEX vertices[24] =
	{
		{ -250.0f, 250.0f, -250.0f, 0.0f, 0.0f },		// Top vertices
		{ 250.0f, 250.0f, -250.0f, 1.0f, 0.0f },
		{ -250.0f, 250.0f,  250.0f, 0.0f, 1.0f },
		{ 250.0f, 250.0f,  250.0f, 1.0f, 1.0f },

		{ -250.0f,  250.0f, 250.0f, 0.0f, 0.0f },		// Front vertices
		{ 250.0f,  250.0f, 250.0f, 1.0f, 0.0f },
		{ -250.0f, -250.0f, 250.0f, 0.0f, 1.0f },
		{ 250.0f, -250.0f, 250.0f, 1.0f, 1.0f },

		{ 250.0f,  250.0f, -250.0f, 0.0f, 0.0f },		// Back vertices
		{ -250.0f,  250.0f, -250.0f, 1.0f, 0.0f },
		{ 250.0f, -250.0f, -250.0f, 0.0f, 1.0f },
		{ -250.0f, -250.0f, -250.0f, 1.0f, 1.0f },

		{ -250.0f,  250.0f, -250.0f, 0.0f, 0.0f },		// Left vertices
		{ -250.0f,  250.0f,  250.0f, 1.0f, 0.0f },
		{ -250.0f, -250.0f, -250.0f, 0.0f, 1.0f },
		{ -250.0f, -250.0f,  250.0f, 1.0f, 1.0f },

		{ 250.0f,  250.0f,  250.0f, 0.0f, 0.0f },		// Right vertices
		{ 250.0f,  250.0f, -250.0f, 1.0f, 0.0f },
		{ 250.0f, -250.0f,  250.0f, 0.0f, 1.0f },
		{ 250.0f, -250.0f, -250.0f, 1.0f, 1.0f },

		{ -250.0f, -250.0f,  250.0f, 0.0f, 0.0f },		// Bottom vertices
		{ 250.0f, -250.0f,  250.0f, 1.0f, 0.0f },
		{ -250.0f, -250.0f, -250.0f, 0.0f, 1.0f },
		{ 250.0f, -250.0f, -250.0f, 1.0f, 1.0f }
	};
	VOID* pVertices;
	if (FAILED(m_pVB->Lock(0, 24 * sizeof(SKYVERTEX), (void**)&pVertices, 0)))
	{
		if (m_pVB != NULL)
			m_pVB->Release();
	}
	memcpy(pVertices, vertices, 24 * sizeof(SKYVERTEX));
	m_pVB->Unlock();

}

void cSkyBox::init()
{
	g_pTextureManager->AddTexture("Top", "SkyBox/Top.jpg", NULL);
	g_pTextureManager->AddTexture("Front", "SkyBox/Front.jpg", NULL);
	g_pTextureManager->AddTexture("Back", "SkyBox/Back.jpg", NULL);
	g_pTextureManager->AddTexture("Right", "SkyBox/Right.jpg", NULL);
	g_pTextureManager->AddTexture("Left", "SkyBox/Left.jpg", NULL);
	g_pTextureManager->AddTexture("Bottom", "SkyBox/Bottom.jpg", NULL);

	m_pTex[0] = g_pTextureManager->GetTexture("Top");
	m_pTex[1] = g_pTextureManager->GetTexture("Front");
	m_pTex[2] = g_pTextureManager->GetTexture("Back");
	m_pTex[3] = g_pTextureManager->GetTexture("Left");
	m_pTex[4] = g_pTextureManager->GetTexture("Right");
	m_pTex[5] = g_pTextureManager->GetTexture("Bottom");

	CreateSkyVertex();
}



void cSkyBox::render()
{
	g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP); //v축으로 미러링
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP); //u축으로 미러링
	g_pD3DDevice->SetMaterial(&WHITE_MTRL);
	//SetupLight();
	g_pD3DDevice->SetFVF((D3DFVF_XYZ | D3DFVF_TEX1));

	D3DVERTEXBUFFER_DESC vbDesc;
	m_pVB->GetDesc(&vbDesc);
	g_pD3DDevice->SetStreamSource(0, m_pVB, 0, sizeof(SKYVERTEX));
	
	for (int i = 0; i < 6; ++i)
	{
		g_pD3DDevice->SetTexture(0, m_pTex[i]);
		g_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, i*4, 2);
	}
	
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP); //원본으로 변환 (v 0.0 ~ 1.0) 1.0이 넘어가면 다시 0.0
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP); //원본으로 변환 (u 0.0 ~ 1.0) 1.0이 넘어가면 다시 0.0
	g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void cSkyBox::SetupLight()
{
	D3DXVECTOR3 vDir;
	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(D3DLIGHT9));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Diffuse.r = 1.0f;
	light.Diffuse.g = 1.0f;
	light.Diffuse.b = 1.0f;
	vDir = D3DXVECTOR3(1, 1, 1);
	D3DXVec3Normalize((D3DXVECTOR3*)&light.Direction, &vDir);
	light.Range = 1000.0f;
	g_pD3DDevice->SetLight(0, &light);
	g_pD3DDevice->LightEnable(0, TRUE);
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	g_pD3DDevice->SetRenderState(D3DRS_AMBIENT, 0x00909090);
}
