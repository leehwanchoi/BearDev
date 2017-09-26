#include "stdafx.h"
#include "cCube.h"


cCube::cCube()
	: m_fRotX(0.0f)
	, m_fRotXSpeed(0.0f)
	, m_fRotY(0.0f)
	, m_fRotYSpeed(0.0f)
	, m_vPosition(0, 0, 0)
	, m_vStdRotPosition(0.0f, 0.0f, 0.0f)
	, m_vb(NULL)
	, m_ib(NULL)
{
	D3DXMatrixIdentity(&m_matWorld);
}


cCube::~cCube()
{
	SAFE_RELEASE(m_vb);
	SAFE_RELEASE(m_ib);
}

void cCube::Setup(D3DXMATRIXA16* pMat, vector<D3DXVECTOR2>* vecTexture,
	LPDIRECT3DTEXTURE9 pTexture, const D3DMATERIAL9* stMtrl)
{
	// 정점 8개를 셋팅
	m_vecVertex.push_back(D3DXVECTOR3(-1.0f,-1.0f,-1.0f));
	m_vecVertex.push_back(D3DXVECTOR3(-1.0f, 1.0f,-1.0f));
	m_vecVertex.push_back(D3DXVECTOR3( 1.0f, 1.0f,-1.0f));
	m_vecVertex.push_back(D3DXVECTOR3( 1.0f,-1.0f,-1.0f));
	m_vecVertex.push_back(D3DXVECTOR3(-1.0f,-1.0f, 1.0f));
	m_vecVertex.push_back(D3DXVECTOR3(-1.0f, 1.0f, 1.0f));
	m_vecVertex.push_back(D3DXVECTOR3( 1.0f, 1.0f, 1.0f));
	m_vecVertex.push_back(D3DXVECTOR3( 1.0f,-1.0f, 1.0f));

	m_vecIndex.push_back(0);
	m_vecIndex.push_back(1);
	m_vecIndex.push_back(2);
	m_vecIndex.push_back(0);
	m_vecIndex.push_back(2);
	m_vecIndex.push_back(3);

	m_vecIndex.push_back(4);
	m_vecIndex.push_back(5);
	m_vecIndex.push_back(1);
	m_vecIndex.push_back(4);
	m_vecIndex.push_back(1);
	m_vecIndex.push_back(0);

	m_vecIndex.push_back(7);
	m_vecIndex.push_back(6);
	m_vecIndex.push_back(5);
	m_vecIndex.push_back(7);
	m_vecIndex.push_back(5);
	m_vecIndex.push_back(4);

	m_vecIndex.push_back(3);
	m_vecIndex.push_back(2);
	m_vecIndex.push_back(6);
	m_vecIndex.push_back(3);
	m_vecIndex.push_back(6);
	m_vecIndex.push_back(7);

	m_vecIndex.push_back(4);
	m_vecIndex.push_back(0);
	m_vecIndex.push_back(3);
	m_vecIndex.push_back(4);
	m_vecIndex.push_back(3);
	m_vecIndex.push_back(7);

	m_vecIndex.push_back(1);
	m_vecIndex.push_back(5);
	m_vecIndex.push_back(6);
	m_vecIndex.push_back(1);
	m_vecIndex.push_back(6);
	m_vecIndex.push_back(2);

	// 큐브에 대한 초기 설정값(SRT매트릭스) 적용
	if (pMat)
	{
		for (int i = 0; i < m_vecVertex.size(); ++i)
		{
			D3DXVec3TransformCoord(&m_vecVertex[i], &m_vecVertex[i], pMat);
		}
	}

	if (vecTexture == NULL)
	{
		if (stMtrl)
		{
			SetupPNVertex(m_vecVertex);
			m_stMtrl = *stMtrl;
		}
		else
			SetupPCVertex(m_vecVertex);
	}
	else
	{
		if (stMtrl)
		{
			SetupPNTVertex(m_vecVertex, *vecTexture);
		}
		else
		{
			SetupPTVertex(m_vecVertex, *vecTexture);
		}
		m_pTexture = pTexture;
	}
}

void cCube::SetupPCVertex(vector<D3DXVECTOR3>& vecVertex)
{
	D3DCOLOR c;

	c = D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256);	// RGB 색상값 설정
	m_vecPCVertex.push_back(ST_PC_VERTEX(vecVertex[0], c));
	m_vecPCVertex.push_back(ST_PC_VERTEX(vecVertex[1], c));
	m_vecPCVertex.push_back(ST_PC_VERTEX(vecVertex[2], c));
	m_vecPCVertex.push_back(ST_PC_VERTEX(vecVertex[0], c));
	m_vecPCVertex.push_back(ST_PC_VERTEX(vecVertex[2], c));
	m_vecPCVertex.push_back(ST_PC_VERTEX(vecVertex[3], c));

	c = D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256);
	m_vecPCVertex.push_back(ST_PC_VERTEX(vecVertex[4], c));
	m_vecPCVertex.push_back(ST_PC_VERTEX(vecVertex[5], c));
	m_vecPCVertex.push_back(ST_PC_VERTEX(vecVertex[1], c));
	m_vecPCVertex.push_back(ST_PC_VERTEX(vecVertex[4], c));
	m_vecPCVertex.push_back(ST_PC_VERTEX(vecVertex[1], c));
	m_vecPCVertex.push_back(ST_PC_VERTEX(vecVertex[0], c));

	c = D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256);
	m_vecPCVertex.push_back(ST_PC_VERTEX(vecVertex[7], c));
	m_vecPCVertex.push_back(ST_PC_VERTEX(vecVertex[6], c));
	m_vecPCVertex.push_back(ST_PC_VERTEX(vecVertex[5], c));
	m_vecPCVertex.push_back(ST_PC_VERTEX(vecVertex[7], c));
	m_vecPCVertex.push_back(ST_PC_VERTEX(vecVertex[5], c));
	m_vecPCVertex.push_back(ST_PC_VERTEX(vecVertex[4], c));

	c = D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256);
	m_vecPCVertex.push_back(ST_PC_VERTEX(vecVertex[3], c));
	m_vecPCVertex.push_back(ST_PC_VERTEX(vecVertex[2], c));
	m_vecPCVertex.push_back(ST_PC_VERTEX(vecVertex[6], c));
	m_vecPCVertex.push_back(ST_PC_VERTEX(vecVertex[3], c));
	m_vecPCVertex.push_back(ST_PC_VERTEX(vecVertex[6], c));
	m_vecPCVertex.push_back(ST_PC_VERTEX(vecVertex[7], c));

	c = D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256);
	m_vecPCVertex.push_back(ST_PC_VERTEX(vecVertex[4], c));
	m_vecPCVertex.push_back(ST_PC_VERTEX(vecVertex[0], c));
	m_vecPCVertex.push_back(ST_PC_VERTEX(vecVertex[3], c));
	m_vecPCVertex.push_back(ST_PC_VERTEX(vecVertex[4], c));
	m_vecPCVertex.push_back(ST_PC_VERTEX(vecVertex[3], c));
	m_vecPCVertex.push_back(ST_PC_VERTEX(vecVertex[7], c));

	c = D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256);
	m_vecPCVertex.push_back(ST_PC_VERTEX(vecVertex[1], c));
	m_vecPCVertex.push_back(ST_PC_VERTEX(vecVertex[5], c));
	m_vecPCVertex.push_back(ST_PC_VERTEX(vecVertex[6], c));
	m_vecPCVertex.push_back(ST_PC_VERTEX(vecVertex[1], c));
	m_vecPCVertex.push_back(ST_PC_VERTEX(vecVertex[6], c));
	m_vecPCVertex.push_back(ST_PC_VERTEX(vecVertex[2], c));
}

void cCube::SetupPNVertex(vector<D3DXVECTOR3>& vecVertex)
{
	SetupD3DVertex();
}

void cCube::SetupPTVertex(vector<D3DXVECTOR3>& vecVertex, vector<D3DXVECTOR2>& vecTexture)
{
	m_vecPTVertex.push_back(ST_PT_VERTEX(vecVertex[0], vecTexture[0]));
	m_vecPTVertex.push_back(ST_PT_VERTEX(vecVertex[1], vecTexture[1]));
	m_vecPTVertex.push_back(ST_PT_VERTEX(vecVertex[2], vecTexture[2]));
	m_vecPTVertex.push_back(ST_PT_VERTEX(vecVertex[0], vecTexture[3]));
	m_vecPTVertex.push_back(ST_PT_VERTEX(vecVertex[2], vecTexture[4]));
	m_vecPTVertex.push_back(ST_PT_VERTEX(vecVertex[3], vecTexture[5]));

	m_vecPTVertex.push_back(ST_PT_VERTEX(vecVertex[4], vecTexture[6]));
	m_vecPTVertex.push_back(ST_PT_VERTEX(vecVertex[5], vecTexture[7]));
	m_vecPTVertex.push_back(ST_PT_VERTEX(vecVertex[1], vecTexture[8]));
	m_vecPTVertex.push_back(ST_PT_VERTEX(vecVertex[4], vecTexture[9]));
	m_vecPTVertex.push_back(ST_PT_VERTEX(vecVertex[1], vecTexture[10]));
	m_vecPTVertex.push_back(ST_PT_VERTEX(vecVertex[0], vecTexture[11]));

	m_vecPTVertex.push_back(ST_PT_VERTEX(vecVertex[7], vecTexture[12]));
	m_vecPTVertex.push_back(ST_PT_VERTEX(vecVertex[6], vecTexture[13]));
	m_vecPTVertex.push_back(ST_PT_VERTEX(vecVertex[5], vecTexture[14]));
	m_vecPTVertex.push_back(ST_PT_VERTEX(vecVertex[7], vecTexture[15]));
	m_vecPTVertex.push_back(ST_PT_VERTEX(vecVertex[5], vecTexture[16]));
	m_vecPTVertex.push_back(ST_PT_VERTEX(vecVertex[4], vecTexture[17]));

	m_vecPTVertex.push_back(ST_PT_VERTEX(vecVertex[3], vecTexture[18]));
	m_vecPTVertex.push_back(ST_PT_VERTEX(vecVertex[2], vecTexture[19]));
	m_vecPTVertex.push_back(ST_PT_VERTEX(vecVertex[6], vecTexture[20]));
	m_vecPTVertex.push_back(ST_PT_VERTEX(vecVertex[3], vecTexture[21]));
	m_vecPTVertex.push_back(ST_PT_VERTEX(vecVertex[6], vecTexture[22]));
	m_vecPTVertex.push_back(ST_PT_VERTEX(vecVertex[7], vecTexture[23]));

	m_vecPTVertex.push_back(ST_PT_VERTEX(vecVertex[4], vecTexture[24]));
	m_vecPTVertex.push_back(ST_PT_VERTEX(vecVertex[0], vecTexture[25]));
	m_vecPTVertex.push_back(ST_PT_VERTEX(vecVertex[3], vecTexture[26]));
	m_vecPTVertex.push_back(ST_PT_VERTEX(vecVertex[4], vecTexture[27]));
	m_vecPTVertex.push_back(ST_PT_VERTEX(vecVertex[3], vecTexture[28]));
	m_vecPTVertex.push_back(ST_PT_VERTEX(vecVertex[7], vecTexture[29]));

	m_vecPTVertex.push_back(ST_PT_VERTEX(vecVertex[1], vecTexture[30]));
	m_vecPTVertex.push_back(ST_PT_VERTEX(vecVertex[5], vecTexture[31]));
	m_vecPTVertex.push_back(ST_PT_VERTEX(vecVertex[6], vecTexture[32]));
	m_vecPTVertex.push_back(ST_PT_VERTEX(vecVertex[1], vecTexture[33]));
	m_vecPTVertex.push_back(ST_PT_VERTEX(vecVertex[6], vecTexture[34]));
	m_vecPTVertex.push_back(ST_PT_VERTEX(vecVertex[2], vecTexture[35]));
}

void cCube::SetupPNTVertex(vector<D3DXVECTOR3>& vecVertex, vector<D3DXVECTOR2>& vecTexture)
{
	D3DXVECTOR3 n;

	ComputeNormal(&n, &vecVertex[0], &vecVertex[1], &vecVertex[2]);
	m_vecPNTVertex.push_back(ST_PNT_VERTEX(vecVertex[0], n, vecTexture[0]));
	m_vecPNTVertex.push_back(ST_PNT_VERTEX(vecVertex[1], n, vecTexture[1]));
	m_vecPNTVertex.push_back(ST_PNT_VERTEX(vecVertex[2], n, vecTexture[2]));
	m_vecPNTVertex.push_back(ST_PNT_VERTEX(vecVertex[0], n, vecTexture[3]));
	m_vecPNTVertex.push_back(ST_PNT_VERTEX(vecVertex[2], n, vecTexture[4]));
	m_vecPNTVertex.push_back(ST_PNT_VERTEX(vecVertex[3], n, vecTexture[5]));
	ComputeNormal(&n, &vecVertex[4], &vecVertex[5], &vecVertex[1]);
	m_vecPNTVertex.push_back(ST_PNT_VERTEX(vecVertex[4], n, vecTexture[6]));
	m_vecPNTVertex.push_back(ST_PNT_VERTEX(vecVertex[5], n, vecTexture[7]));
	m_vecPNTVertex.push_back(ST_PNT_VERTEX(vecVertex[1], n, vecTexture[8]));
	m_vecPNTVertex.push_back(ST_PNT_VERTEX(vecVertex[4], n, vecTexture[9]));
	m_vecPNTVertex.push_back(ST_PNT_VERTEX(vecVertex[1], n, vecTexture[10]));
	m_vecPNTVertex.push_back(ST_PNT_VERTEX(vecVertex[0], n, vecTexture[11]));
	ComputeNormal(&n, &vecVertex[7], &vecVertex[6], &vecVertex[5]);
	m_vecPNTVertex.push_back(ST_PNT_VERTEX(vecVertex[7], n, vecTexture[12]));
	m_vecPNTVertex.push_back(ST_PNT_VERTEX(vecVertex[6], n, vecTexture[13]));
	m_vecPNTVertex.push_back(ST_PNT_VERTEX(vecVertex[5], n, vecTexture[14]));
	m_vecPNTVertex.push_back(ST_PNT_VERTEX(vecVertex[7], n, vecTexture[15]));
	m_vecPNTVertex.push_back(ST_PNT_VERTEX(vecVertex[5], n, vecTexture[16]));
	m_vecPNTVertex.push_back(ST_PNT_VERTEX(vecVertex[4], n, vecTexture[17]));
	ComputeNormal(&n, &vecVertex[3], &vecVertex[2], &vecVertex[6]);
	m_vecPNTVertex.push_back(ST_PNT_VERTEX(vecVertex[3], n, vecTexture[18]));
	m_vecPNTVertex.push_back(ST_PNT_VERTEX(vecVertex[2], n, vecTexture[19]));
	m_vecPNTVertex.push_back(ST_PNT_VERTEX(vecVertex[6], n, vecTexture[20]));
	m_vecPNTVertex.push_back(ST_PNT_VERTEX(vecVertex[3], n, vecTexture[21]));
	m_vecPNTVertex.push_back(ST_PNT_VERTEX(vecVertex[6], n, vecTexture[22]));
	m_vecPNTVertex.push_back(ST_PNT_VERTEX(vecVertex[7], n, vecTexture[23]));
	ComputeNormal(&n, &vecVertex[4], &vecVertex[0], &vecVertex[3]);
	m_vecPNTVertex.push_back(ST_PNT_VERTEX(vecVertex[4], n, vecTexture[24]));
	m_vecPNTVertex.push_back(ST_PNT_VERTEX(vecVertex[0], n, vecTexture[25]));
	m_vecPNTVertex.push_back(ST_PNT_VERTEX(vecVertex[3], n, vecTexture[26]));
	m_vecPNTVertex.push_back(ST_PNT_VERTEX(vecVertex[4], n, vecTexture[27]));
	m_vecPNTVertex.push_back(ST_PNT_VERTEX(vecVertex[3], n, vecTexture[28]));
	m_vecPNTVertex.push_back(ST_PNT_VERTEX(vecVertex[7], n, vecTexture[29]));
	ComputeNormal(&n, &vecVertex[1], &vecVertex[5], &vecVertex[6]);
	m_vecPNTVertex.push_back(ST_PNT_VERTEX(vecVertex[1], n, vecTexture[30]));
	m_vecPNTVertex.push_back(ST_PNT_VERTEX(vecVertex[5], n, vecTexture[31]));
	m_vecPNTVertex.push_back(ST_PNT_VERTEX(vecVertex[6], n, vecTexture[32]));
	m_vecPNTVertex.push_back(ST_PNT_VERTEX(vecVertex[1], n, vecTexture[33]));
	m_vecPNTVertex.push_back(ST_PNT_VERTEX(vecVertex[6], n, vecTexture[34]));
	m_vecPNTVertex.push_back(ST_PNT_VERTEX(vecVertex[2], n, vecTexture[35]));

	SetupD3DVertex();
}

void cCube::SetupD3DVertex()
{
	if (!m_vecPNTVertex.empty())
	{
		ST_PNT_VERTEX*		pPNTVertex;

		g_pD3DDevice->CreateVertexBuffer(m_vecPNTVertex.size() * sizeof(ST_PNT_VERTEX),
			NULL, ST_PNT_VERTEX::FVF, D3DPOOL_MANAGED, &m_vb, NULL);

		m_vb->Lock(0, 0, (void**)&pPNTVertex, 0);

		for (int i = 0; i < m_vecPNTVertex.size(); ++i)
		{
			pPNTVertex[i] = m_vecPNTVertex[i];
		}

		m_vecPNTVertex.clear();

		m_vb->Unlock();
	}
	else
	{
		ST_PN_VERTEX*		pPNVertex;
		WORD*				pIndex;

		g_pD3DDevice->CreateVertexBuffer(m_vecVertex.size() * sizeof(ST_PN_VERTEX),
			NULL, ST_PN_VERTEX::FVF, D3DPOOL_MANAGED, &m_vb, NULL);
		g_pD3DDevice->CreateIndexBuffer(m_vecIndex.size() * sizeof(WORD),
			NULL, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_ib, NULL);

		m_vb->Lock(0, 0, (void**)&pPNVertex, 0);

		for (int i = 0; i < m_vecVertex.size(); ++i)
		{
			pPNVertex[i].n = D3DXVECTOR3(0, 1, 0);
			pPNVertex[i].p = m_vecVertex[i];
		}

		m_vecVertex.clear();

		m_vb->Unlock();

		m_ib->Lock(0, 0, (void**)&pIndex, 0);

		for (int i = 0; i < m_vecIndex.size(); i += 3)
		{
			pIndex[i + 0] = m_vecIndex[i + 0];
			pIndex[i + 1] = m_vecIndex[i + 1];
			pIndex[i + 2] = m_vecIndex[i + 2];
		}

		m_vecIndex.clear();

		m_ib->Unlock();

		m_vecPNVertex.clear();
	}
}

/*
컨트롤러로 부터 움직임이 있는지 없는지 정보를 받아와서
움직임이 있을 때는 팔다리를 움직이고 없을 때는 머리만 움직이도록 설정
*/
void cCube::Update(bool isMoving, D3DXMATRIXA16* pParent/*= NULL*/)
{
	D3DXMATRIXA16 matRX, matRY, matT, matInvT, matWT;

	// 회전량을 45도 각도로 제한 한다. (제한 값 도달시 반대 방향으로 회전)
	if (m_fRotX > D3DX_PI / 4 || m_fRotX < -D3DX_PI / 4)
		m_fRotXSpeed *= -1;

	if (m_fRotY > D3DX_PI / 4 || m_fRotY < -D3DX_PI / 4)
		m_fRotYSpeed *= -1;

	if (isMoving)	// 움직이고 있다면
	{
		m_fRotX += m_fRotXSpeed;	// X축 회전 (팔, 다리 해당)을 시킨다.
		m_fRotY = 0.0f;				// Y축 회전 (머리용)은 초기화(정면을 바라보도록)
	}
	else			// 정지 중 이라면
	{
		m_fRotX = 0.0f;				// X축 회전 (팔, 다리 해당)은 초기화(차렷)
		m_fRotY += m_fRotYSpeed;	// Y축 회전 (머리용)을 시킨다.
	}

	D3DXMatrixRotationX(&matRX, m_fRotX);	// X축 회전에 대한 매트릭스 생성
	D3DXMatrixRotationY(&matRY, m_fRotY);	// Y축 회전에 대한 매트릭스 생성

	// 각 부위별 원하는 축을 기준으로 회전 시키기 위해서 그에 맞게 이동을 시켜주는
	// 매트릭스를 생성
	D3DXMatrixTranslation(&matT, m_vStdRotPosition.x, m_vStdRotPosition.y, m_vStdRotPosition.z);
	// 회전 후에 원래의 위치로 되돌려 놓기 위해서 인벌스 매트릭스 생성
	D3DXMatrixTranslation(&matInvT, -m_vStdRotPosition.x, -m_vStdRotPosition.y, -m_vStdRotPosition.z);
	
	// 월드 공간에서의 정해진 위치로 이동해주는 매트릭스
	D3DXMatrixTranslation(&matWT, m_vPosition.x, m_vPosition.y, m_vPosition.z);

	// (원하는 축으로 이동) 후 (회전) (원래 위치로 되돌리기)
	m_matWorld = (matT * matRX * matRY * matInvT) * matWT;

	if (pParent)
	{
		m_matWorld *= *pParent;	// m_matWorld = pParent; 나의 매트릭스가 무조건 아이덴티티 매트릭스인 경우
	}

	for each (auto p in m_vecChild)
		p->Update(isMoving, &m_matWorld);
}

void cCube::Render()
{
	g_pD3DDevice->SetTransform(D3DTS_WORLD, &m_matWorld);

	if (m_vecPCVertex.size() > 0)
	{
		g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, false);
		g_pD3DDevice->SetTexture(0, NULL);
		g_pD3DDevice->SetFVF(ST_PC_VERTEX::FVF);
		g_pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_vecPCVertex.size() / 3,
			&m_vecPCVertex[0], sizeof(ST_PC_VERTEX));
		g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, true);
	}
	else if (m_vb && m_ib)
	{
		D3DVERTEXBUFFER_DESC vbDesc;
		m_vb->GetDesc(&vbDesc);
		D3DINDEXBUFFER_DESC ibDesc;
		m_ib->GetDesc(&ibDesc);

		g_pD3DDevice->SetTexture(0, NULL);
		g_pD3DDevice->SetFVF(ST_PN_VERTEX::FVF);
		g_pD3DDevice->SetMaterial(&m_stMtrl);
		//g_pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_vecPNVertex.size() / 3,
		//	&m_vecPNVertex[0], sizeof(ST_PN_VERTEX));
		g_pD3DDevice->SetStreamSource(0, m_vb, 0, sizeof(ST_PN_VERTEX));
		g_pD3DDevice->SetIndices(m_ib);
		g_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, vbDesc.Size, 0,
			ibDesc.Size / 3);
	}
	else if (m_vecPTVertex.size() > 0)
	{
		g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, false);
		g_pD3DDevice->SetTexture(0, m_pTexture);
		g_pD3DDevice->SetFVF(ST_PT_VERTEX::FVF);
		g_pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_vecPTVertex.size() / 3,
			&m_vecPTVertex[0], sizeof(ST_PT_VERTEX));
		g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, true);
	}
	else if (m_vb)
	{
		D3DVERTEXBUFFER_DESC vbDesc;
		m_vb->GetDesc(&vbDesc);

		g_pD3DDevice->SetTexture(0, m_pTexture);
		g_pD3DDevice->SetFVF(ST_PNT_VERTEX::FVF);
		g_pD3DDevice->SetMaterial(&m_stMtrl);
		//g_pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_vecPNTVertex.size() / 3,
		//	&m_vecPNTVertex[0], sizeof(ST_PNT_VERTEX));
		g_pD3DDevice->SetStreamSource(0, m_vb, 0, sizeof(ST_PNT_VERTEX));
		g_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, vbDesc.Size / 3);
	}

	for each (auto p in m_vecChild)
	{
		p->Render();
	}
}


void cCube::AddChild(cCube* pCube)
{
	m_vecChild.push_back(pCube);
}

void cCube::Collision(D3DXVECTOR3 target)
{
	D3DXVECTOR3 vDir = m_vPosition - target;

	if (D3DXVec3Length(&vDir) < 2.0f - D3DX_16F_EPSILON)
	{
		D3DXVec3Normalize(&vDir, &vDir);
		D3DXMATRIXA16 matT;
		D3DXMatrixTranslation(&matT, vDir.x, vDir.y, vDir.z);
		D3DXVec3TransformCoord(&m_vPosition, &m_vPosition, &matT);
	}
}
