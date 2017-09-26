#include "stdafx.h"
#include "cTerrain.h"
#include "cXobjectLoader.h"

/*
float rsq = x * x + z * z;
float fHeight = 3.0f * expf(-rsq / (2.0f * 3.0f));
vecPNTVertex[nIndex].p = D3DXVECTOR3(x, -5.0f + fHeight, z);
*/

/*
�׸��� �ɼ�
4
8
16
32
64
128
*/

cTerrain::cTerrain()
	: m_CameraRay()
	, m_isPicked(false)
	, m_isLButtonDown(false)
	, m_vDest(0, 0, 0)
	, m_pSphere(NULL)
	, m_isFirst(true)
	, m_nVertexNum(128 + 1)
	, m_nTileNum(m_nVertexNum - 1)
	, m_isLoaded(NULL)
	, m_fVertexMaxHeight(5.0f)
	, m_fVertexMinHeight(0.0f)
	, m_fFlatteningHeight(1.0f)
	, m_pTexture(NULL)
	, m_isMod(false)
	, m_XIndex(0)
	, m_isfastCheck(false)
	, m_fTerrainSpeed(5.0f)
{
	D3DXMatrixIdentity(&m_matWorld);
}


cTerrain::~cTerrain()
{
	SAFE_RELEASE(m_pSphere);

	for (int i = 0; i < m_vecXLoader.size(); i++)
		m_vecXLoader[i]->Destroy();
}

void cTerrain::Setup()
{
	if (m_isFirst)
		D3DXCreateSphere(g_pD3DDevice, 0.1f, 10, 10, &m_pSphere, NULL);

	m_TextureName = "./MapToolTexture/snowTexture.jpg";
	g_pTextureManager->AddTexture(m_TextureName, m_TextureName, &m_pTexture);

	//�׸��带 �ٽ� �����ϴ� ��츦 ���� ��� ���� Ŭ����
	D3DXMatrixIdentity(&m_matWorld);
	m_vDest = D3DXVECTOR3(0, 0, 0);
	m_vecVertex.clear();
	m_vecIndex.clear();
	m_fastVertex.clear();
	m_fastVertexHalf01.clear();
	m_fastVertexHalf02.clear();
	m_fastVertexHalf03.clear();
	m_fastVertexHalf04.clear();
	m_vecfast01Index.clear();
	m_vecfast02Index.clear();
	m_vecfast03Index.clear();
	m_vecfast04Index.clear();
	m_vecfast05Index.clear();
	m_vecfast06Index.clear();
	m_vecfast07Index.clear();
	m_vecfast08Index.clear();
	m_vecfast09Index.clear();
	m_vecfast10Index.clear();
	m_vecfast11Index.clear();
	m_vecfast12Index.clear();
	m_vecfast13Index.clear();
	m_vecfast14Index.clear();
	m_vecfast15Index.clear();
	m_vecfast16Index.clear();

	m_vecVertex.resize(m_nVertexNum * m_nVertexNum);
	m_vecPNTVertex.clear();
	m_vecPNTVertex.resize(m_nVertexNum * m_nVertexNum * sizeof(ST_PNT_VERTEX));

	for (int z = 0; z < m_nVertexNum; ++z)
	{
		for (int x = 0; x < m_nVertexNum; ++x)
		{
			// 1�߹迭 ����� �ε��� ��ȣ ����
			int nIndex = z * m_nVertexNum + x;
			float y = 0.0f;

			D3DXVECTOR3 n;
			ComputeNormal(&n, &m_vecPNTVertex[nIndex + 0].p, &m_vecPNTVertex[nIndex + 1].p, &m_vecPNTVertex[nIndex + 2].p);

			// ĳ������ ��ġ�� ���� y �� ��ȯ�� ���ؼ� ���ؽ� ������ ���� ���� �����Ѵ�.
			m_vecVertex[nIndex] = D3DXVECTOR3(x, y, z);

			m_vecPNTVertex[nIndex].p = D3DXVECTOR3(x, y, z);
			m_vecPNTVertex[nIndex].n = n;
			m_vecPNTVertex[nIndex].t = D3DXVECTOR2(x, z);

			if (m_matWorld)
				D3DXVec3TransformCoord(&m_vecVertex[nIndex], &m_vecVertex[nIndex], &m_matWorld);
		}
	}

	// ���� ���� �� ����� ���� ĳ������ ��ġ�� ����ؾ� �ϱ� ������
	// ���� �� ������ ������ �����Ѵ�.
	m_fSizeX = m_matWorld._11;
	m_fSizeZ = m_matWorld._33;

	// �ε��� ��ȣ�� �����Ѵ�.
	//vector<DWORD> vecIndex;
	//vecIndex.reserve(m_nTileNum * m_nTileNum * 3 * 2);

	for (int z = 0; z < m_nTileNum; ++z)
	{
		for (int x = 0; x < m_nTileNum; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * m_nVertexNum + x + 0;
			int _1 = (z + 1) * m_nVertexNum + x + 0;
			int _2 = (z + 0) * m_nVertexNum + x + 1;
			int _3 = (z + 1) * m_nVertexNum + x + 1;

			m_vecIndex.push_back(_0);
			m_vecIndex.push_back(_1);
			m_vecIndex.push_back(_2);
			m_vecIndex.push_back(_2);
			m_vecIndex.push_back(_1);
			m_vecIndex.push_back(_3);
		}
	}

	//���� �˻��� ���� �۾�
	SliceQuarterSetup();

	//���� ������ false�� ����, ���� �ȵǰ� ���´�. (���� �����Ҵ��� ����� ����)
	m_isFirst = false;

	//X File
}

void cTerrain::Update()
{
	// ���� ���� �� ����� ���� ĳ������ ��ġ�� ����ؾ� �ϱ� ������
	// ���� �� ������ ������ �����Ѵ�.
	m_fSizeX = m_matWorld._11;
	m_fSizeZ = m_matWorld._33;

	//X���� ��尡 �ƴҶ��� �ͷ��θ��
	if (!m_isMod)
		CreateTerrain(m_dwDest);

	//XLoader�ε��� �����ϴ� �Լ�
	FindXIndex();
}

void cTerrain::Render()
{
	g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	//X File
	for (int i = 0; i < m_vecXLoader.size(); i++)
		m_vecXLoader[i]->Render();

	//X ���� ���õ� �ε����� �浹ü �׸���(���õȰ͸� ����������)
	if (m_vecXLoader.size() > 0)
		m_vecXLoader[m_XIndex]->SelectedCollisionRender();

	D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity(&matWorld);

	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, true);
	//���콺 ��ġ ǥ���� �� �׸���.
	g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	g_pD3DDevice->SetTexture(0, NULL);
	D3DXMatrixTranslation(&matWorld, m_vDest.x, m_vDest.y, m_vDest.z);
	g_pD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);
	g_pD3DDevice->SetMaterial(&RED_MTRL);
	m_pSphere->DrawSubset(0);
	g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	//�����Ʈ���� ����
	g_pD3DDevice->SetTransform(D3DTS_WORLD, &m_matWorld);
	
	//���͸��� �ؽ��� ����
	if (!g_pKeyManager->isToggleKey(VK_CAPITAL))
		g_pD3DDevice->SetTexture(0, m_pTexture);

	//�׸��� �ؽ��� �׸����� ����Ʈ������.
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, false);
	g_pD3DDevice->SetFVF(ST_PNT_VERTEX::FVF);

	//Ʈ���̾ޱ۷� �ѹ� �׸�
	//g_pD3DDevice->DrawIndexedPrimitiveUP(
	//	D3DPT_TRIANGLELIST,				// ���� �׸��� Ÿ��
	//	0,								// �Ǿ�(����) ���ؽ��� �ε���
	//	m_vecVertex.size(),				// ���Ǵ� ���ؽ��� ��
	//	m_vecIndex.size() / 3,			// ������ ��
	//	&m_vecIndex[0],					// �ε��� ������ ������ �迭
	//	D3DFMT_INDEX32,					// �ε��� ���� ( 16��Ʈ���� 32��Ʈ���� )
	//	&m_vecVertex[0],				// ���ؽ� ������ ������ �迭
	//	sizeof(D3DXVECTOR3));			// ���ؽ��� ������

	g_pD3DDevice->SetMaterial(&WHITE_MTRL);
	//������ Ʈ���̾ޱ۷� �׸�
	g_pD3DDevice->DrawIndexedPrimitiveUP(
		D3DPT_TRIANGLELIST,				// ���� �׸��� Ÿ��
		0,								// �Ǿ�(����) ���ؽ��� �ε���
		m_vecPNTVertex.size(),				// ���Ǵ� ���ؽ��� ��
		m_vecIndex.size() / 3,			// ������ ��
		&m_vecIndex[0],					// �ε��� ������ ������ �迭
		D3DFMT_INDEX32,					// �ε��� ���� ( 16��Ʈ���� 32��Ʈ���� )
		&m_vecPNTVertex[0],				// ���ؽ� ������ ������ �迭
		sizeof(ST_PNT_VERTEX));			// ���ؽ��� ������

	//���� ��
	if (g_pKeyManager->isToggleKey(VK_NUMLOCK))
		g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, true);
	else
		g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, false);

	//���̾�� �׸���
	g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	//�ø� ����
	g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	g_pD3DDevice->SetMaterial(&GRAY_MTRL);

	//���̾�� �ѹ� �׸�
	//g_pD3DDevice->DrawIndexedPrimitiveUP(
	//	D3DPT_TRIANGLELIST,				// ���� �׸��� Ÿ��
	//	0,								// �Ǿ�(����) ���ؽ��� �ε���
	//	m_vecVertex.size(),				// ���Ǵ� ���ؽ��� ��
	//	m_vecIndex.size() / 3,			// ������ ��
	//	&m_vecIndex[0],					// �ε��� ������ ������ �迭
	//	D3DFMT_INDEX32,					// �ε��� ���� ( 16��Ʈ���� 32��Ʈ���� )
	//	&m_vecVertex[0],				// ���ؽ� ������ ������ �迭
	//	sizeof(D3DXVECTOR3));			// ���ؽ��� ������

	//������ ���̾�� �׸�
	g_pD3DDevice->DrawIndexedPrimitiveUP(
		D3DPT_TRIANGLELIST,				// ���� �׸��� Ÿ��
		0,								// �Ǿ�(����) ���ؽ��� �ε���
		m_vecPNTVertex.size(),				// ���Ǵ� ���ؽ��� ��
		m_vecIndex.size() / 3,			// ������ ��
		&m_vecIndex[0],					// �ε��� ������ ������ �迭
		D3DFMT_INDEX32,					// �ε��� ���� ( 16��Ʈ���� 32��Ʈ���� )
		&m_vecPNTVertex[0],				// ���ؽ� ������ ������ �迭
		sizeof(ST_PNT_VERTEX));			// ���ؽ��� ������

	//�׸��� �׽�Ʈ��
	//g_pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_fastVertex.size() / 3, m_fastVertex[0], sizeof(D3DXVECTOR3));
	//g_pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_fastVertexHalf01.size() / 3, m_fastVertexHalf01[0], sizeof(D3DXVECTOR3));
	//g_pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_fastVertexHalf02.size() / 3, m_fastVertexHalf02[0], sizeof(D3DXVECTOR3));
	//g_pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_fastVertexHalf03.size() / 3, m_fastVertexHalf03[0], sizeof(D3DXVECTOR3));
	//g_pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_fastVertexHalf04.size() / 3, m_fastVertexHalf04[0], sizeof(D3DXVECTOR3));

	//���� ��
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, false);
	//���̾�->�������
	g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	//�ø� ����
	g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void cTerrain::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_MOUSEMOVE:
	{
		m_CameraRay = cRay::RayAtWorldSpace(LOWORD(lParam), HIWORD(lParam));
		//Ŭ�������� ��ǥ ���� �� �ͷ��� �۵�
		SliceQuarterCalculation();

		//�� Ŭ���� ���콺 ��ǥ ���� �Լ�
		if (!m_isLButtonDown)
			RealtimeMousePositionCalculation();

		if (m_isLButtonDown)
		{

			////�������� �˻� (����ȭ ��)
			//for (int i = 0; i < m_vecIndex.size(); i += 3)
			//{
			//	float f;
			//	if (D3DXIntersectTri(&m_vecVertex[m_vecIndex[i + 0]],
			//		&m_vecVertex[m_vecIndex[i + 1]], &m_vecVertex[m_vecIndex[i + 2]],
			//		&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
			//	{
			//		if (f < minF)
			//		{
			//			minF = f;
			//			m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
			//			m_dwDest = m_vecIndex[i + 0];
			//			m_isPicked = true;
			//			break;
			//		}
			//	}
			//}
		}
	}
	break;
	case WM_LBUTTONDOWN:
	{
		m_isLButtonDown = true;

		//m_CameraRay = cRay::RayAtWorldSpace(LOWORD(lParam), HIWORD(lParam));
		//float minF = 100000;

		//for (int i = 0; i < m_vecIndex.size(); i += 3)
		//{
		//	float f;
		//	if (D3DXIntersectTri(&m_vecVertex[m_vecIndex[i + 0]],
		//		&m_vecVertex[m_vecIndex[i + 1]], &m_vecVertex[m_vecIndex[i + 2]],
		//		&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
		//	{
		//		if (f < minF)
		//		{
		//			minF = f;
		//			m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
		//			m_dwDest = m_vecIndex[i + 0];
		//			m_isPicked = true;
		//		}
		//	}
		//}
	}
	break;
	case WM_LBUTTONUP:
		m_isLButtonDown = false;
		m_isPicked = false;
		break;
	}
}

bool cTerrain::GetHeight(IN const float x, OUT float & y, IN const float z)
{
	// ���� ��� ��� (����� ���� �� ��� ���� �� ������� ��ǥ�� ������ ����Ѵ�.)
	if (x < 0 || x / m_fSizeX > m_nTileNum || z < 0 || z / m_fSizeZ > m_nTileNum)
		return false;

	// 1--3
	// |\ |
	// | \|
	// 0--2
	int nX = (int)(x / m_fSizeX);
	int nZ = (int)(z / m_fSizeZ);
	float fDeltaX = (x / m_fSizeX) - nX;
	float fDeltaZ = (z / m_fSizeZ) - nZ;

	// ĳ���� ��ġ x, z �� ���ؼ� 0 1 2 3 ������ �ε��� ��ȣ�� ����Ѵ�.
	int _0 = (nZ + 0) * m_nVertexNum + nX + 0;
	int _1 = (nZ + 1) * m_nVertexNum + nX + 0;
	int _2 = (nZ + 0) * m_nVertexNum + nX + 1;
	int _3 = (nZ + 1) * m_nVertexNum + nX + 1;

	// 1--3
	// |\ |
	// | \|
	// 0--2
	if (fDeltaX + fDeltaZ < 1.0f) // �Ʒ��� �ﰢ�� ���� ��ġ�ϴ� ���
	{
		// 1 - 0
		float zY = m_vecVertex[_1].y - m_vecVertex[_0].y;
		// 2 - 0
		float xY = m_vecVertex[_2].y - m_vecVertex[_0].y;
		// 0�� �ε����� y���� �������� ĳ���� ��ġ y�� ���
		y = m_vecVertex[_0].y + zY * fDeltaZ + xY * fDeltaX;
	}
	else // ���� �ﰢ��
	{
		// 2 - 3
		float zY = m_vecVertex[_2].y - m_vecVertex[_3].y;
		// 1 - 3
		float xY = m_vecVertex[_1].y - m_vecVertex[_3].y;
		// 3�� �ε����� y���� �������� ĳ���� ��ġ y�� ���
		// ��Ÿ ���� 0�� �ε��� �������� ����� �߱� ������
		// 3�� �ε��� �������� ��� �� ���� ������ �ʿ� �ϴ�.
		y = m_vecVertex[_3].y + zY * (1.0f - fDeltaZ) + xY * (1.0f - fDeltaX);
	}

	return true;
}

void cTerrain::CreateTerrain(DWORD index)
{
	if (m_isPicked)
	{
		//m_vecVertex[i].y -> PNT[i].p.y�� ����
		if (!g_pKeyManager->isStayKeyDown(VK_LSHIFT))
			m_vecPNTVertex[index + 0].p.y += m_fTerrainSpeed * g_pTimeManager->getElapsedTime();
		else if (g_pKeyManager->isStayKeyDown(VK_LSHIFT))
			m_vecPNTVertex[index + 0].p.y -= m_fTerrainSpeed * g_pTimeManager->getElapsedTime();

		if (m_vecPNTVertex[index + 0].p.y < m_fVertexMinHeight)
			m_vecPNTVertex[index + 0].p.y = m_fVertexMinHeight;

		if (m_vecPNTVertex[index + 0].p.y > m_fVertexMaxHeight)
			m_vecPNTVertex[index + 0].p.y = m_fVertexMaxHeight;

		if (g_pKeyManager->isStayKeyDown(VK_CONTROL))
			m_vecPNTVertex[index + 0].p.y = m_fFlatteningHeight;

		/*
		if (index + 1 + m_nVertexNum)
		{
			if (!g_pKeyManager->isStayKeyDown(VK_LSHIFT))
			{
				m_vecVertex[index + 0].y += +5.0f * g_pTimeManager->getElapsedTime();
				m_vecVertex[index + 1].y += +5.0f * g_pTimeManager->getElapsedTime();
				m_vecVertex[index + 0 + m_nVertexNum].y += +5.0f * g_pTimeManager->getElapsedTime();
				m_vecVertex[index + 1 + m_nVertexNum].y += +5.0f * g_pTimeManager->getElapsedTime();
			}
			else if (g_pKeyManager->isStayKeyDown(VK_LSHIFT))
			{
				m_vecVertex[index + 0].y -= +5.0f * g_pTimeManager->getElapsedTime();
				m_vecVertex[index + 1].y -= +5.0f * g_pTimeManager->getElapsedTime();
				m_vecVertex[index + 0 + m_nVertexNum].y -= +5.0f * g_pTimeManager->getElapsedTime();
				m_vecVertex[index + 1 + m_nVertexNum].y -= +5.0f * g_pTimeManager->getElapsedTime();
			}


			//������� ���� �ִ� �� ó��
			if (m_vecVertex[index + 0].y < 0.0f)
				m_vecVertex[index + 0].y = 0.0f;

			if (m_vecVertex[index + 0].y > 2.0f)
				m_vecVertex[index + 0].y = 2.0f;

			if (m_vecVertex[index + 1].y < 0.0f)
				m_vecVertex[index + 1].y = 0.0f;

			if (m_vecVertex[index + 1].y > 2.0f)
				m_vecVertex[index + 1].y = 2.0f;

			if (m_vecVertex[index + 0 + m_nVertexNum].y < 0.0f)
				m_vecVertex[index + 0 + m_nVertexNum].y = 0.0f;

			if (m_vecVertex[index + 0 + m_nVertexNum].y > 2.0f)
				m_vecVertex[index + 0 + m_nVertexNum].y = 2.0f;

			if (m_vecVertex[index + 1 + m_nVertexNum].y < 0.0f)
				m_vecVertex[index + 1 + m_nVertexNum].y = 0.0f;

			if (m_vecVertex[index + 1 + m_nVertexNum].y > 2.0f)
				m_vecVertex[index + 1 + m_nVertexNum].y = 2.0f;
		}
		*/
	}
}

void cTerrain::SliceQuarterSetup()
{
	//�ʱ� ���� �˻� �ε��� ����
	m_fastSearch[0] = 0;
	m_fastSearch[1] = m_nTileNum / 2;
	m_fastSearch[2] = m_nTileNum;
	m_fastSearch[3] = m_nTileNum * m_nVertexNum / 2;
	m_fastSearch[4] = (m_nTileNum * m_nVertexNum / 2) + (m_nTileNum / 2);
	m_fastSearch[5] = (m_nTileNum * m_nVertexNum / 2) + m_nTileNum;
	m_fastSearch[6] = m_nTileNum * m_nVertexNum;
	m_fastSearch[7] = ((m_nVertexNum * m_nVertexNum) - 1) - (m_nTileNum / 2);
	m_fastSearch[8] = m_nVertexNum * m_nVertexNum - 1;

	/*
	6 7 8
	3 4 5
	0 1 2
	*/

	// ũ�� �ѹ� �˻��ϱ� ����, �б⺰�� ����.(1~4�б�)
	if (m_fastSearch[8] > 0)
	{
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[0]]);//1~3
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[3]]);
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[1]]);
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[1]]);//4~6 //1�б� (x < m_fastSearch[1]) (z < m_fastSearch[3])
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[3]]);
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[4]]);
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[1]]);//7~9
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[4]]);
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[2]]);
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[2]]);//10~12 //2�б�
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[4]]);
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[5]]);
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[3]]);//13~15
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[6]]);
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[4]]);
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[4]]);//16~18 //3�б�
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[6]]);
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[7]]);
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[4]]);//19~21
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[7]]);
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[5]]);
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[5]]);//22~24 //4�б�
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[7]]);
		m_fastVertex.push_back(m_vecVertex[m_fastSearch[8]]);
	}


	//���� �˻��� ���� �۾� (����) 1�б� ����ȭ
	m_fastSearchHalf01[0] = 0;
	m_fastSearchHalf01[1] = m_fastSearch[1] / 2;
	m_fastSearchHalf01[2] = m_fastSearch[2] / 2;
	m_fastSearchHalf01[3] = m_fastSearch[3] / 2;
	m_fastSearchHalf01[4] = m_fastSearch[4] / 2;
	m_fastSearchHalf01[5] = m_fastSearch[5] / 2;
	m_fastSearchHalf01[6] = m_fastSearch[6] / 2;
	m_fastSearchHalf01[7] = m_fastSearch[7] / 2;
	m_fastSearchHalf01[8] = m_fastSearch[8] / 2;

	// �б⺰�� �������� �ٽ� ����. (���� �˻��� ����) 1�б� ����ȭ
	if (m_fastSearchHalf01[8] > 0)
	{
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[0]]);//1~3
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[3]]);
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[1]]);
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[1]]);//4~6 //1�б� (x < m_fastSearchHalf[1]) (z < m_fastSearchHalf[3])
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[3]]);
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[4]]);
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[1]]);//7~9
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[4]]);
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[2]]);
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[2]]);//10~12 //2�б�
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[4]]);
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[5]]);
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[3]]);//13~15
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[6]]);
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[4]]);
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[4]]);//16~18 //3�б�
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[6]]);
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[7]]);
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[4]]);//19~21
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[7]]);
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[5]]);
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[5]]);//22~24 //4�б�
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[7]]);
		m_fastVertexHalf01.push_back(m_vecVertex[m_fastSearchHalf01[8]]);
	}

	//���� �˻��� ���� �۾� (����) 2�б� ����ȭ
	m_fastSearchHalf02[0] = m_fastSearchHalf01[0] + m_fastSearchHalf01[2];
	m_fastSearchHalf02[1] = m_fastSearchHalf01[1] + m_fastSearchHalf01[2];
	m_fastSearchHalf02[2] = m_fastSearchHalf01[2] + m_fastSearchHalf01[2];
	m_fastSearchHalf02[3] = m_fastSearchHalf01[3] + m_fastSearchHalf01[2];
	m_fastSearchHalf02[4] = m_fastSearchHalf01[4] + m_fastSearchHalf01[2];
	m_fastSearchHalf02[5] = m_fastSearchHalf01[5] + m_fastSearchHalf01[2];
	m_fastSearchHalf02[6] = m_fastSearchHalf01[6] + m_fastSearchHalf01[2];
	m_fastSearchHalf02[7] = m_fastSearchHalf01[7] + m_fastSearchHalf01[2];
	m_fastSearchHalf02[8] = m_fastSearchHalf01[8] + m_fastSearchHalf01[2];

	// �б⺰�� �������� �ٽ� ����. (���� �˻��� ����) 2�б� ����ȭ
	if (m_fastSearchHalf02[8] > 0)
	{
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[0]]);//1~3
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[3]]);
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[1]]);
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[1]]);//4~6 //1�б� (x < m_fastSearchHalf[1]) (z < m_fastSearchHalf[3])
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[3]]);
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[4]]);
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[1]]);//7~9
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[4]]);
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[2]]);
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[2]]);//10~12 //2�б�
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[4]]);
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[5]]);
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[3]]);//13~15
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[6]]);
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[4]]);
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[4]]);//16~18 //3�б�
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[6]]);
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[7]]);
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[4]]);//19~21
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[7]]);
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[5]]);
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[5]]);//22~24 //4�б�
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[7]]);
		m_fastVertexHalf02.push_back(m_vecVertex[m_fastSearchHalf02[8]]);
	}

	//���� �˻��� ���� �۾� (����) 3�б� ����ȭ
	m_fastSearchHalf03[0] = m_fastSearchHalf01[0] + m_fastSearchHalf01[6];
	m_fastSearchHalf03[1] = m_fastSearchHalf01[1] + m_fastSearchHalf01[6];
	m_fastSearchHalf03[2] = m_fastSearchHalf01[2] + m_fastSearchHalf01[6];
	m_fastSearchHalf03[3] = m_fastSearchHalf01[3] + m_fastSearchHalf01[6];
	m_fastSearchHalf03[4] = m_fastSearchHalf01[4] + m_fastSearchHalf01[6];
	m_fastSearchHalf03[5] = m_fastSearchHalf01[5] + m_fastSearchHalf01[6];
	m_fastSearchHalf03[6] = m_fastSearchHalf01[6] + m_fastSearchHalf01[6];
	m_fastSearchHalf03[7] = m_fastSearchHalf01[7] + m_fastSearchHalf01[6];
	m_fastSearchHalf03[8] = m_fastSearchHalf01[8] + m_fastSearchHalf01[6];

	// �б⺰�� �������� �ٽ� ����. (���� �˻��� ����) 3�б� ����ȭ
	if (m_fastSearchHalf03[8] > 0)
	{
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[0]]);//1~3
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[3]]);
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[1]]);
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[1]]);//4~6 //1�б� (x < m_fastSearchHalf[1]) (z < m_fastSearchHalf[3])
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[3]]);
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[4]]);
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[1]]);//7~9
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[4]]);
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[2]]);
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[2]]);//10~12 //2�б�
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[4]]);
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[5]]);
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[3]]);//13~15
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[6]]);
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[4]]);
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[4]]);//16~18 //3�б�
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[6]]);
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[7]]);
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[4]]);//19~21
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[7]]);
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[5]]);
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[5]]);//22~24 //4�б�
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[7]]);
		m_fastVertexHalf03.push_back(m_vecVertex[m_fastSearchHalf03[8]]);
	}

	//���� �˻��� ���� �۾� (����) 4�б� ����ȭ
	m_fastSearchHalf04[0] = m_fastSearchHalf03[0] + m_fastSearchHalf01[2];
	m_fastSearchHalf04[1] = m_fastSearchHalf03[1] + m_fastSearchHalf01[2];
	m_fastSearchHalf04[2] = m_fastSearchHalf03[2] + m_fastSearchHalf01[2];
	m_fastSearchHalf04[3] = m_fastSearchHalf03[3] + m_fastSearchHalf01[2];
	m_fastSearchHalf04[4] = m_fastSearchHalf03[4] + m_fastSearchHalf01[2];
	m_fastSearchHalf04[5] = m_fastSearchHalf03[5] + m_fastSearchHalf01[2];
	m_fastSearchHalf04[6] = m_fastSearchHalf03[6] + m_fastSearchHalf01[2];
	m_fastSearchHalf04[7] = m_fastSearchHalf03[7] + m_fastSearchHalf01[2];
	m_fastSearchHalf04[8] = m_fastSearchHalf03[8] + m_fastSearchHalf01[2];

	// �б⺰�� �������� �ٽ� ����. (���� �˻��� ����) 4�б� ����ȭ
	if (m_fastSearchHalf04[8] > 0)
	{
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[0]]);//1~3
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[3]]);
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[1]]);
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[1]]);//4~6 //1�б� (x < m_fastSearchHalf[1]) (z < m_fastSearchHalf[3])
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[3]]);
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[4]]);
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[1]]);//7~9
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[4]]);
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[2]]);
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[2]]);//10~12 //2�б�
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[4]]);
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[5]]);
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[3]]);//13~15
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[6]]);
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[4]]);
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[4]]);//16~18 //3�б�
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[6]]);
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[7]]);
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[4]]);//19~21
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[7]]);
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[5]]);
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[5]]);//22~24 //4�б�
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[7]]);
		m_fastVertexHalf04.push_back(m_vecVertex[m_fastSearchHalf04[8]]);
	}


	// ���� �˻��� ���� ��к� �ε��� ����
	// 1���
	for (int z = m_fastSearchHalf01[0]; z < m_fastSearchHalf01[1]; ++z)
	{
		for (int x = m_fastSearchHalf01[0]; x < m_fastSearchHalf01[1]; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * m_nVertexNum + x + 0;
			int _1 = (z + 1) * m_nVertexNum + x + 0;
			int _2 = (z + 0) * m_nVertexNum + x + 1;
			int _3 = (z + 1) * m_nVertexNum + x + 1;

			m_vecfast01Index.push_back(_0);
			m_vecfast01Index.push_back(_1);
			m_vecfast01Index.push_back(_2);
			m_vecfast01Index.push_back(_2);
			m_vecfast01Index.push_back(_1);
			m_vecfast01Index.push_back(_3);
		}
	}

	// 2���
	for (int z = m_fastSearchHalf01[0]; z < m_fastSearchHalf01[1]; ++z)
	{
		for (int x = m_fastSearchHalf01[1]; x < m_fastSearchHalf01[2]; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * m_nVertexNum + x + 0;
			int _1 = (z + 1) * m_nVertexNum + x + 0;
			int _2 = (z + 0) * m_nVertexNum + x + 1;
			int _3 = (z + 1) * m_nVertexNum + x + 1;

			m_vecfast02Index.push_back(_0);
			m_vecfast02Index.push_back(_1);
			m_vecfast02Index.push_back(_2);
			m_vecfast02Index.push_back(_2);
			m_vecfast02Index.push_back(_1);
			m_vecfast02Index.push_back(_3);
		}
	}

	// 3���
	for (int z = m_fastSearchHalf01[1]; z < m_fastSearchHalf01[2]; ++z)
	{
		for (int x = m_fastSearchHalf01[0]; x < m_fastSearchHalf01[1]; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * m_nVertexNum + x + 0;
			int _1 = (z + 1) * m_nVertexNum + x + 0;
			int _2 = (z + 0) * m_nVertexNum + x + 1;
			int _3 = (z + 1) * m_nVertexNum + x + 1;

			m_vecfast03Index.push_back(_0);
			m_vecfast03Index.push_back(_1);
			m_vecfast03Index.push_back(_2);
			m_vecfast03Index.push_back(_2);
			m_vecfast03Index.push_back(_1);
			m_vecfast03Index.push_back(_3);
		}
	}

	// 4���
	for (int z = m_fastSearchHalf01[1]; z < m_fastSearchHalf01[2]; ++z)
	{
		for (int x = m_fastSearchHalf01[1]; x < m_fastSearchHalf01[2]; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * m_nVertexNum + x + 0;
			int _1 = (z + 1) * m_nVertexNum + x + 0;
			int _2 = (z + 0) * m_nVertexNum + x + 1;
			int _3 = (z + 1) * m_nVertexNum + x + 1;

			m_vecfast04Index.push_back(_0);
			m_vecfast04Index.push_back(_1);
			m_vecfast04Index.push_back(_2);
			m_vecfast04Index.push_back(_2);
			m_vecfast04Index.push_back(_1);
			m_vecfast04Index.push_back(_3);
		}
	}

	// 5���
	for (int z = m_fastSearchHalf01[0]; z < m_fastSearchHalf01[1]; ++z)
	{
		for (int x = m_fastSearchHalf02[0]; x < m_fastSearchHalf02[1]; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * m_nVertexNum + x + 0;
			int _1 = (z + 1) * m_nVertexNum + x + 0;
			int _2 = (z + 0) * m_nVertexNum + x + 1;
			int _3 = (z + 1) * m_nVertexNum + x + 1;

			m_vecfast05Index.push_back(_0);
			m_vecfast05Index.push_back(_1);
			m_vecfast05Index.push_back(_2);
			m_vecfast05Index.push_back(_2);
			m_vecfast05Index.push_back(_1);
			m_vecfast05Index.push_back(_3);
		}
	}

	// 6���
	for (int z = m_fastSearchHalf01[0]; z < m_fastSearchHalf01[1]; ++z)
	{
		for (int x = m_fastSearchHalf02[1]; x < m_fastSearchHalf02[2]; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * m_nVertexNum + x + 0;
			int _1 = (z + 1) * m_nVertexNum + x + 0;
			int _2 = (z + 0) * m_nVertexNum + x + 1;
			int _3 = (z + 1) * m_nVertexNum + x + 1;

			m_vecfast06Index.push_back(_0);
			m_vecfast06Index.push_back(_1);
			m_vecfast06Index.push_back(_2);
			m_vecfast06Index.push_back(_2);
			m_vecfast06Index.push_back(_1);
			m_vecfast06Index.push_back(_3);
		}
	}

	// 7���
	for (int z = m_fastSearchHalf01[1]; z < m_fastSearchHalf01[2]; ++z)
	{
		for (int x = m_fastSearchHalf02[0]; x < m_fastSearchHalf02[1]; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * m_nVertexNum + x + 0;
			int _1 = (z + 1) * m_nVertexNum + x + 0;
			int _2 = (z + 0) * m_nVertexNum + x + 1;
			int _3 = (z + 1) * m_nVertexNum + x + 1;

			m_vecfast07Index.push_back(_0);
			m_vecfast07Index.push_back(_1);
			m_vecfast07Index.push_back(_2);
			m_vecfast07Index.push_back(_2);
			m_vecfast07Index.push_back(_1);
			m_vecfast07Index.push_back(_3);
		}
	}

	// 8���
	for (int z = m_fastSearchHalf01[1]; z < m_fastSearchHalf01[2]; ++z)
	{
		for (int x = m_fastSearchHalf02[1]; x < m_fastSearchHalf02[2]; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * m_nVertexNum + x + 0;
			int _1 = (z + 1) * m_nVertexNum + x + 0;
			int _2 = (z + 0) * m_nVertexNum + x + 1;
			int _3 = (z + 1) * m_nVertexNum + x + 1;

			m_vecfast08Index.push_back(_0);
			m_vecfast08Index.push_back(_1);
			m_vecfast08Index.push_back(_2);
			m_vecfast08Index.push_back(_2);
			m_vecfast08Index.push_back(_1);
			m_vecfast08Index.push_back(_3);
		}
	}

	// 9���
	for (int z = m_fastSearchHalf02[0]; z < m_fastSearchHalf02[1]; ++z)
	{
		for (int x = m_fastSearchHalf01[0]; x < m_fastSearchHalf01[1]; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * m_nVertexNum + x + 0;
			int _1 = (z + 1) * m_nVertexNum + x + 0;
			int _2 = (z + 0) * m_nVertexNum + x + 1;
			int _3 = (z + 1) * m_nVertexNum + x + 1;

			m_vecfast09Index.push_back(_0);
			m_vecfast09Index.push_back(_1);
			m_vecfast09Index.push_back(_2);
			m_vecfast09Index.push_back(_2);
			m_vecfast09Index.push_back(_1);
			m_vecfast09Index.push_back(_3);
		}
	}

	// 10���
	for (int z = m_fastSearchHalf02[0]; z < m_fastSearchHalf02[1]; ++z)
	{
		for (int x = m_fastSearchHalf01[1]; x < m_fastSearchHalf01[2]; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * m_nVertexNum + x + 0;
			int _1 = (z + 1) * m_nVertexNum + x + 0;
			int _2 = (z + 0) * m_nVertexNum + x + 1;
			int _3 = (z + 1) * m_nVertexNum + x + 1;

			m_vecfast10Index.push_back(_0);
			m_vecfast10Index.push_back(_1);
			m_vecfast10Index.push_back(_2);
			m_vecfast10Index.push_back(_2);
			m_vecfast10Index.push_back(_1);
			m_vecfast10Index.push_back(_3);
		}
	}

	// 11���
	for (int z = m_fastSearchHalf02[1]; z < m_fastSearchHalf02[2]; ++z)
	{
		for (int x = m_fastSearchHalf01[0]; x < m_fastSearchHalf01[1]; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * m_nVertexNum + x + 0;
			int _1 = (z + 1) * m_nVertexNum + x + 0;
			int _2 = (z + 0) * m_nVertexNum + x + 1;
			int _3 = (z + 1) * m_nVertexNum + x + 1;

			m_vecfast11Index.push_back(_0);
			m_vecfast11Index.push_back(_1);
			m_vecfast11Index.push_back(_2);
			m_vecfast11Index.push_back(_2);
			m_vecfast11Index.push_back(_1);
			m_vecfast11Index.push_back(_3);
		}
	}

	// 12���
	for (int z = m_fastSearchHalf02[1]; z < m_fastSearchHalf02[2]; ++z)
	{
		for (int x = m_fastSearchHalf01[1]; x < m_fastSearchHalf01[2]; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * m_nVertexNum + x + 0;
			int _1 = (z + 1) * m_nVertexNum + x + 0;
			int _2 = (z + 0) * m_nVertexNum + x + 1;
			int _3 = (z + 1) * m_nVertexNum + x + 1;

			m_vecfast12Index.push_back(_0);
			m_vecfast12Index.push_back(_1);
			m_vecfast12Index.push_back(_2);
			m_vecfast12Index.push_back(_2);
			m_vecfast12Index.push_back(_1);
			m_vecfast12Index.push_back(_3);
		}
	}

	// 13���
	for (int z = m_fastSearchHalf02[0]; z < m_fastSearchHalf02[1]; ++z)
	{
		for (int x = m_fastSearchHalf02[0]; x < m_fastSearchHalf02[1]; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * m_nVertexNum + x + 0;
			int _1 = (z + 1) * m_nVertexNum + x + 0;
			int _2 = (z + 0) * m_nVertexNum + x + 1;
			int _3 = (z + 1) * m_nVertexNum + x + 1;

			m_vecfast13Index.push_back(_0);
			m_vecfast13Index.push_back(_1);
			m_vecfast13Index.push_back(_2);
			m_vecfast13Index.push_back(_2);
			m_vecfast13Index.push_back(_1);
			m_vecfast13Index.push_back(_3);
		}
	}

	// 14���
	for (int z = m_fastSearchHalf02[0]; z < m_fastSearchHalf02[1]; ++z)
	{
		for (int x = m_fastSearchHalf02[1]; x < m_fastSearchHalf02[2]; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * m_nVertexNum + x + 0;
			int _1 = (z + 1) * m_nVertexNum + x + 0;
			int _2 = (z + 0) * m_nVertexNum + x + 1;
			int _3 = (z + 1) * m_nVertexNum + x + 1;

			m_vecfast14Index.push_back(_0);
			m_vecfast14Index.push_back(_1);
			m_vecfast14Index.push_back(_2);
			m_vecfast14Index.push_back(_2);
			m_vecfast14Index.push_back(_1);
			m_vecfast14Index.push_back(_3);
		}
	}

	// 15���
	for (int z = m_fastSearchHalf02[1]; z < m_fastSearchHalf02[2]; ++z)
	{
		for (int x = m_fastSearchHalf02[0]; x < m_fastSearchHalf02[1]; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * m_nVertexNum + x + 0;
			int _1 = (z + 1) * m_nVertexNum + x + 0;
			int _2 = (z + 0) * m_nVertexNum + x + 1;
			int _3 = (z + 1) * m_nVertexNum + x + 1;

			m_vecfast15Index.push_back(_0);
			m_vecfast15Index.push_back(_1);
			m_vecfast15Index.push_back(_2);
			m_vecfast15Index.push_back(_2);
			m_vecfast15Index.push_back(_1);
			m_vecfast15Index.push_back(_3);
		}
	}

	// 16���
	for (int z = m_fastSearchHalf02[1]; z < m_fastSearchHalf02[2]; ++z)
	{
		for (int x = m_fastSearchHalf02[1]; x < m_fastSearchHalf02[2]; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * m_nVertexNum + x + 0;
			int _1 = (z + 1) * m_nVertexNum + x + 0;
			int _2 = (z + 0) * m_nVertexNum + x + 1;
			int _3 = (z + 1) * m_nVertexNum + x + 1;

			m_vecfast16Index.push_back(_0);
			m_vecfast16Index.push_back(_1);
			m_vecfast16Index.push_back(_2);
			m_vecfast16Index.push_back(_2);
			m_vecfast16Index.push_back(_1);
			m_vecfast16Index.push_back(_3);
		}
	}
}

void cTerrain::SliceQuarterCalculation()
{
	float minF = 100000;

	bool isBig = false;
	bool isSmall = false;

	int searchBig = -100000; //������ ������ ���� -ū�� (�б� Ȯ�ο�)
	int searchSmall = -100000; //������ ������ ���� -ū�� (����ȭ �б� Ȯ�ο�)

	//���÷� �˻� (4�б�� �˻�)
	if (m_isLButtonDown)
	{
		for (int i = 0; i < m_fastVertex.size(); i += 3)
		{
			float f;
			if (D3DXIntersectTri(&m_fastVertex[i + 0],
				&m_fastVertex[i + 1], &m_fastVertex[i + 2],
				&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
			{
				if (f < minF)
				{
					minF = f;
					//m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
					//m_dwDest = i + 0;
					//m_isPicked = true;
					isBig = true;
					searchBig = i + 0;

					//�׸��� ��ŷ����� Ȯ���ϱ� ���� ����
					m_isfastCheck = true;

			
				}
			}
		}
	}

	//�ʱ�ȭ
	minF = 100000;
	//���÷� �˻� (4�б⿡�� 4�б�� �˻�) //16���
	if (isBig && !isSmall)
	{
		if (searchBig < 6) // (4/1)�б�
		{
			for (int i = 0; i < m_fastVertexHalf01.size(); i += 3)
			{
				float f;
				if (D3DXIntersectTri(&m_fastVertexHalf01[i + 0],
					&m_fastVertexHalf01[i + 1], &m_fastVertexHalf01[i + 2],
					&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
				{
					if (f < minF)
					{
						minF = f;
						//m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
						//m_dwDest = i + 0;
						isSmall = true;
						searchSmall = i + 0;

					
					}
				}
			}
		}
		else if (searchBig < 12) // (4/2)�б�
		{
			for (int i = 0; i < m_fastVertexHalf02.size(); i += 3)
			{
				float f;
				if (D3DXIntersectTri(&m_fastVertexHalf02[i + 0],
					&m_fastVertexHalf02[i + 1], &m_fastVertexHalf02[i + 2],
					&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
				{
					if (f < minF)
					{
						minF = f;
						//m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
						//m_dwDest = i + 0;
						isSmall = true;
						searchSmall = i + 0;

				
					}
				}
			}
		}
		else if (searchBig < 18) // (4/3)�б�
		{
			for (int i = 0; i < m_fastVertexHalf03.size(); i += 3)
			{
				float f;
				if (D3DXIntersectTri(&m_fastVertexHalf03[i + 0],
					&m_fastVertexHalf03[i + 1], &m_fastVertexHalf03[i + 2],
					&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
				{
					if (f < minF)
					{
						minF = f;
						//m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
						//m_dwDest = i + 0;
						isSmall = true;
						searchSmall = i + 0;

				
					}
				}
			}
		}
		else if (searchBig < 24) // (4/4)�б�
		{
			for (int i = 0; i < m_fastVertexHalf04.size(); i += 3)
			{
				float f;
				if (D3DXIntersectTri(&m_fastVertexHalf04[i + 0],
					&m_fastVertexHalf04[i + 1], &m_fastVertexHalf04[i + 2],
					&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
				{
					if (f < minF)
					{
						minF = f;
						//m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
						//m_dwDest = i + 0;
						isSmall = true;
						searchSmall = i + 0;

				
					}
				}
			}
		}
	}

	//�ʱ�ȭ
	minF = 100000;
	//16��� �߿��� �˻�
	if (isSmall)
	{
		if (searchBig < 6) // (4/1) -> (16/1)��� ~ (16/4)���
		{
			if (searchSmall < 6) // (16/1)���
			{
				for (int i = 0; i < m_vecfast01Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast01Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast01Index[i + 1]].p, &m_vecPNTVertex[m_vecfast01Index[i + 2]].p,
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast01Index[i + 0];
							m_isPicked = true;

						
						}
					}
				}
			}
			else if (searchSmall < 12) // (16/2)���
			{
				for (int i = 0; i < m_vecfast02Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast02Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast02Index[i + 1]].p, &m_vecPNTVertex[m_vecfast02Index[i + 2]].p,
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast02Index[i + 0];
							m_isPicked = true;

				
						}
					}
				}
			}
			else if (searchSmall < 18) // (16/3)���
			{
				for (int i = 0; i < m_vecfast03Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast03Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast03Index[i + 1]].p, &m_vecPNTVertex[m_vecfast03Index[i + 2]].p,
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast03Index[i + 0];
							m_isPicked = true;

					
						}
					}
				}
			}
			else if (searchSmall < 24) // (16/4)���
			{
				for (int i = 0; i < m_vecfast04Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast04Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast04Index[i + 1]].p, &m_vecPNTVertex[m_vecfast04Index[i + 2]].p,
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast04Index[i + 0];
							m_isPicked = true;

					
						}
					}
				}
			}
		}
		else if (searchBig < 12) // (4/2) -> (16/5)��� ~ (16/8)���
		{
			if (searchSmall < 6) // (16/5)���
			{
				for (int i = 0; i < m_vecfast05Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast05Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast05Index[i + 1]].p, &m_vecPNTVertex[m_vecfast05Index[i + 2]].p,
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast05Index[i + 0];
							m_isPicked = true;

					
						}
					}
				}
			}
			else if (searchSmall < 12) // (16/6)���
			{
				for (int i = 0; i < m_vecfast06Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast06Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast06Index[i + 1]].p, &m_vecPNTVertex[m_vecfast06Index[i + 2]].p,
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast06Index[i + 0];
							m_isPicked = true;

					
						}
					}
				}
			}
			else if (searchSmall < 18) // (16/7)���
			{
				for (int i = 0; i < m_vecfast07Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast07Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast07Index[i + 1]].p, &m_vecPNTVertex[m_vecfast07Index[i + 2]].p,
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast07Index[i + 0];
							m_isPicked = true;

					
						}
					}
				}
			}
			else if (searchSmall < 24) // (16/8)���
			{
				for (int i = 0; i < m_vecfast08Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast08Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast08Index[i + 1]].p, &m_vecPNTVertex[m_vecfast08Index[i + 2]].p,
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast08Index[i + 0];
							m_isPicked = true;

				
						}
					}
				}
			}
		}
		else if (searchBig < 18) // (4/3) -> (16/9)��� ~ (16/12)���
		{
			if (searchSmall < 6)
			{
				for (int i = 0; i < m_vecfast09Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast09Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast09Index[i + 1]].p, &m_vecPNTVertex[m_vecfast09Index[i + 2]].p,
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast09Index[i + 0];
							m_isPicked = true;

				
						}
					}
				}
			}
			else if (searchSmall < 12)
			{
				for (int i = 0; i < m_vecfast10Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast10Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast10Index[i + 1]].p, &m_vecPNTVertex[m_vecfast10Index[i + 2]].p,
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast10Index[i + 0];
							m_isPicked = true;

			
						}
					}
				}
			}
			else if (searchSmall < 18)
			{
				for (int i = 0; i < m_vecfast11Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast11Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast11Index[i + 1]].p, &m_vecPNTVertex[m_vecfast11Index[i + 2]].p,
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast11Index[i + 0];
							m_isPicked = true;

					
						}
					}
				}
			}
			else if (searchSmall < 24)
			{
				for (int i = 0; i < m_vecfast12Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast12Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast12Index[i + 1]].p, &m_vecPNTVertex[m_vecfast12Index[i + 2]].p,
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast12Index[i + 0];
							m_isPicked = true;

						
						}
					}
				}
			}
		}
		else if (searchBig < 24) // (4/4) -> (16/13)��� ~ (16/16)���
		{
			if (searchSmall < 6)
			{
				for (int i = 0; i < m_vecfast13Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast13Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast13Index[i + 1]].p, &m_vecPNTVertex[m_vecfast13Index[i + 2]].p,
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast13Index[i + 0];
							m_isPicked = true;

					
						}
					}
				}
			}
			else if (searchSmall < 12)
			{
				for (int i = 0; i < m_vecfast14Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast14Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast14Index[i + 1]].p, &m_vecPNTVertex[m_vecfast14Index[i + 2]].p,
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast14Index[i + 0];
							m_isPicked = true;

					
						}
					}
				}
			}
			else if (searchSmall < 18)
			{
				for (int i = 0; i < m_vecfast15Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast15Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast15Index[i + 1]].p, &m_vecPNTVertex[m_vecfast15Index[i + 2]].p,
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast15Index[i + 0];
							m_isPicked = true;

					
						}
					}
				}
			}
			else if (searchSmall < 24)
			{
				for (int i = 0; i < m_vecfast16Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast16Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast16Index[i + 1]].p, &m_vecPNTVertex[m_vecfast16Index[i + 2]].p,
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast16Index[i + 0];
							m_isPicked = true;

						
						}
					}
				}
			}
		}
	}
}

void cTerrain::RealtimeMousePositionCalculation()
{
	float minF = 100000;

	bool isBig = false;
	bool isSmall = false;

	int searchBig = -100000; //������ ������ ���� -ū�� (�б� Ȯ�ο�)
	int searchSmall = -100000; //������ ������ ���� -ū�� (����ȭ �б� Ȯ�ο�)

	//���÷� �˻� (4�б�� �˻�)
	for (int i = 0; i < m_fastVertex.size(); i += 3)
	{
		float f;
		if (D3DXIntersectTri(&m_fastVertex[i + 0],
			&m_fastVertex[i + 1], &m_fastVertex[i + 2],
			&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
		{
			if (f < minF)
			{
				minF = f;
				isBig = true;
				searchBig = i + 0;

				
			}
		}
	}

	//�ʱ�ȭ
	minF = 100000;
	//���÷� �˻� (4�б⿡�� 4�б�� �˻�) //16���
	if (isBig && !isSmall)
	{
		if (searchBig < 6) // (4/1)�б�
		{
			for (int i = 0; i < m_fastVertexHalf01.size(); i += 3)
			{
				float f;
				if (D3DXIntersectTri(&m_fastVertexHalf01[i + 0],
					&m_fastVertexHalf01[i + 1], &m_fastVertexHalf01[i + 2],
					&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
				{
					if (f < minF)
					{
						minF = f;
						isSmall = true;
						searchSmall = i + 0;

					
					}
				}
			}
		}
		else if (searchBig < 12) // (4/2)�б�
		{
			for (int i = 0; i < m_fastVertexHalf02.size(); i += 3)
			{
				float f;
				if (D3DXIntersectTri(&m_fastVertexHalf02[i + 0],
					&m_fastVertexHalf02[i + 1], &m_fastVertexHalf02[i + 2],
					&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
				{
					if (f < minF)
					{
						minF = f;
						isSmall = true;
						searchSmall = i + 0;

				
					}
				}
			}
		}
		else if (searchBig < 18) // (4/3)�б�
		{
			for (int i = 0; i < m_fastVertexHalf03.size(); i += 3)
			{
				float f;
				if (D3DXIntersectTri(&m_fastVertexHalf03[i + 0],
					&m_fastVertexHalf03[i + 1], &m_fastVertexHalf03[i + 2],
					&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
				{
					if (f < minF)
					{
						minF = f;
						isSmall = true;
						searchSmall = i + 0;

					
					}
				}
			}
		}
		else if (searchBig < 24) // (4/4)�б�
		{
			for (int i = 0; i < m_fastVertexHalf04.size(); i += 3)
			{
				float f;
				if (D3DXIntersectTri(&m_fastVertexHalf04[i + 0],
					&m_fastVertexHalf04[i + 1], &m_fastVertexHalf04[i + 2],
					&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
				{
					if (f < minF)
					{
						minF = f;
						isSmall = true;
						searchSmall = i + 0;

					
					}
				}
			}
		}
	}

	//�ʱ�ȭ
	minF = 100000;
	//16��� �߿��� �˻�
	if (isSmall)
	{
		if (searchBig < 6) // (4/1) -> (16/1)��� ~ (16/4)���
		{
			if (searchSmall < 6) // (16/1)���
			{
				for (int i = 0; i < m_vecfast01Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast01Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast01Index[i + 1]].p, &m_vecPNTVertex[m_vecfast01Index[i + 2]].p,
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast01Index[i + 0];

						
						}
					}
				}
			}
			else if (searchSmall < 12) // (16/2)���
			{
				for (int i = 0; i < m_vecfast02Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast02Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast02Index[i + 1]].p, &m_vecPNTVertex[m_vecfast02Index[i + 2]].p,
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast02Index[i + 0];

						
						}
					}
				}
			}
			else if (searchSmall < 18) // (16/3)���
			{
				for (int i = 0; i < m_vecfast03Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast03Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast03Index[i + 1]].p, &m_vecPNTVertex[m_vecfast03Index[i + 2]].p,
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast03Index[i + 0];

						
						}
					}
				}
			}
			else if (searchSmall < 24) // (16/4)���
			{
				for (int i = 0; i < m_vecfast04Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast04Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast04Index[i + 1]].p, &m_vecPNTVertex[m_vecfast04Index[i + 2]].p,
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast04Index[i + 0];

					
						}
					}
				}
			}
		}
		else if (searchBig < 12) // (4/2) -> (16/5)��� ~ (16/8)���
		{
			if (searchSmall < 6) // (16/5)���
			{
				for (int i = 0; i < m_vecfast05Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast05Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast05Index[i + 1]].p, &m_vecPNTVertex[m_vecfast05Index[i + 2]].p,
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast05Index[i + 0];

						
						}
					}
				}
			}
			else if (searchSmall < 12) // (16/6)���
			{
				for (int i = 0; i < m_vecfast06Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast06Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast06Index[i + 1]].p, &m_vecPNTVertex[m_vecfast06Index[i + 2]].p,
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast06Index[i + 0];

				
						}
					}
				}
			}
			else if (searchSmall < 18) // (16/7)���
			{
				for (int i = 0; i < m_vecfast07Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast07Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast07Index[i + 1]].p, &m_vecPNTVertex[m_vecfast07Index[i + 2]].p,
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast07Index[i + 0];

					
						}
					}
				}
			}
			else if (searchSmall < 24) // (16/8)���
			{
				for (int i = 0; i < m_vecfast08Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast08Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast08Index[i + 1]].p, &m_vecPNTVertex[m_vecfast08Index[i + 2]].p,
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast08Index[i + 0];

					
						}
					}
				}
			}
		}
		else if (searchBig < 18) // (4/3) -> (16/9)��� ~ (16/12)���
		{
			if (searchSmall < 6)
			{
				for (int i = 0; i < m_vecfast09Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast09Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast09Index[i + 1]].p, &m_vecPNTVertex[m_vecfast09Index[i + 2]].p,
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast09Index[i + 0];

					
						}
					}
				}
			}
			else if (searchSmall < 12)
			{
				for (int i = 0; i < m_vecfast10Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast10Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast10Index[i + 1]].p, &m_vecPNTVertex[m_vecfast10Index[i + 2]].p,
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast10Index[i + 0];

				
						}
					}
				}
			}
			else if (searchSmall < 18)
			{
				for (int i = 0; i < m_vecfast11Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast11Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast11Index[i + 1]].p, &m_vecPNTVertex[m_vecfast11Index[i + 2]].p,
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast11Index[i + 0];

				
						}
					}
				}
			}
			else if (searchSmall < 24)
			{
				for (int i = 0; i < m_vecfast12Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast12Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast12Index[i + 1]].p, &m_vecPNTVertex[m_vecfast12Index[i + 2]].p,
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast12Index[i + 0];

					
						}
					}
				}
			}
		}
		else if (searchBig < 24) // (4/4) -> (16/13)��� ~ (16/16)���
		{
			if (searchSmall < 6)
			{
				for (int i = 0; i < m_vecfast13Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast13Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast13Index[i + 1]].p, &m_vecPNTVertex[m_vecfast13Index[i + 2]].p,
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast13Index[i + 0];

						
						}
					}
				}
			}
			else if (searchSmall < 12)
			{
				for (int i = 0; i < m_vecfast14Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast14Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast14Index[i + 1]].p, &m_vecPNTVertex[m_vecfast14Index[i + 2]].p,
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast14Index[i + 0];

					
						}
					}
				}
			}
			else if (searchSmall < 18)
			{
				for (int i = 0; i < m_vecfast15Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast15Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast15Index[i + 1]].p, &m_vecPNTVertex[m_vecfast15Index[i + 2]].p,
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast15Index[i + 0];

					
						}
					}
				}
			}
			else if (searchSmall < 24)
			{
				for (int i = 0; i < m_vecfast16Index.size(); i += 3)
				{
					float f;
					if (D3DXIntersectTri(&m_vecPNTVertex[m_vecfast16Index[i + 0]].p,
						&m_vecPNTVertex[m_vecfast16Index[i + 1]].p, &m_vecPNTVertex[m_vecfast16Index[i + 2]].p,
						&m_CameraRay.m_vOrg, &m_CameraRay.m_vDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							m_vDest = m_CameraRay.m_vOrg + m_CameraRay.m_vDir * f;
							m_dwDest = m_vecfast16Index[i + 0];

					
						}
					}
				}
			}
		}
	}
}

void cTerrain::Load()
{
	//�ε�(txtLoadD3DXVECTOR3()�Լ� �������)
	/*vector<string> temp;
	temp = g_pTXTDATAManager->txtLoadDialog(m_isLoaded);
	for (int i = 0; i < temp.size(); i++)
	{
	m_vecVertex[i].y = atof(temp[i].c_str());
	}*/

	vector<D3DXVECTOR3> temp;
	temp.clear();
	temp = g_pTXTDATAManager->txtLoadD3DXVECTOR3(m_isLoaded);

	int Discriminant = m_nVertexNum * m_nVertexNum;
	if (temp.size() > 0 && Discriminant == (temp.size() - 1)) //temp�� ������
	{
		//������
		m_vecVertex.clear();
		m_vecVertex.resize(temp.size() - 1); //temp���� -1�������.
		//����
		m_vecPNTVertex.clear();
		m_vecPNTVertex.resize(m_nVertexNum * m_nVertexNum * sizeof(ST_PNT_VERTEX));

		for (int z = 0; z < m_nVertexNum; ++z)
		{
			for (int x = 0; x < m_nVertexNum; ++x)
			{
				// 1�߹迭 ����� �ε��� ��ȣ ����
				int nIndex = z * m_nVertexNum + x;
				float y = 0.0f;

				D3DXVECTOR3 n;
				ComputeNormal(&n, &m_vecPNTVertex[nIndex + 0].p, &m_vecPNTVertex[nIndex + 1].p, &m_vecPNTVertex[nIndex + 2].p);

				// ĳ������ ��ġ�� ���� y �� ��ȯ�� ���ؼ� ���ؽ� ������ ���� ���� �����Ѵ�.
				m_vecVertex[nIndex] = temp[nIndex];

				m_vecPNTVertex[nIndex].p = temp[nIndex];
				m_vecPNTVertex[nIndex].n = n;
				m_vecPNTVertex[nIndex].t = D3DXVECTOR2(x, z);

				if (m_matWorld)
					D3DXVec3TransformCoord(&m_vecVertex[nIndex], &m_vecVertex[nIndex], &m_matWorld);
			}
		}
	}
	else if (temp.size() > 0 && Discriminant != (temp.size() - 1))
	{
		char str[300];
		wsprintf(str, "�׸��� ����� ��ġ���� �ʾ� ������� �ʽ��ϴ�.");
		MessageBox(g_hWnd, str, "���� �ҷ����� ����", MB_OK);
	}
}

void cTerrain::Save()
{
	//����(txtSaveD3DXVECTOR3()�Լ� �������)
	/*//���̺긦 ���� ��Ʈ����ȯ����
	vector<string> tempVertexY;
	for (int i = 0; i < m_vecVertex.size(); i++)
	{
	tempVertexY.push_back(to_string(m_vecVertex[i].y) + "\n");
	}
	g_pTXTDATAManager->txtSaveDialog(tempVertexY);*/

	vector<D3DXVECTOR3> temp;
	for (int i = 0; i < m_vecVertex.size(); i++)
		temp.push_back(m_vecPNTVertex[i].p);

	g_pTXTDATAManager->txtSaveD3DXVECTOR3(temp);

	char str[300];
	sprintf(str, "%d �׸��� ������� ����Ǿ����ϴ�.", (int)sqrtf(m_vecVertex.size() - 1));
	MessageBox(g_hWnd, str, "���� ����", MB_OK);
}

void cTerrain::FindXIndex()
{
	if (g_pKeyManager->isOnceKeyDown(VK_PRIOR))
	{
		m_XIndex++;
	}
	else if (g_pKeyManager->isOnceKeyDown(VK_NEXT))
	{
		m_XIndex--;
	}

	if (m_XIndex < 0)
		m_XIndex = m_vecXLoader.size() - 1;
	else if (m_XIndex > m_vecXLoader.size() - 1)
		m_XIndex = 0;

	int num = m_XIndex;
	num = m_XIndex;
}





////////////////////////////////////////////////////////////////////////////////////////////////////
//�ʹ� ������� �����ϱ� ���ؼ� �ּ�ó����.
//�̰����ʹ� ��� �����͸� ���̺� �ϰ� �ε� �ϴ� �Լ�
////////////////////////////////////////////////////////////////////////////////////////////////////

void cTerrain::LoadAll()
{
	FILE* fp;

	//���ؽ� ���� �ε�
	fp = fopen("./save/VertexNum", "r");
	char Temp[1024]; //���� �ӽ� �����.
	fgets(Temp, sizeof(Temp), fp);
	int nVertexNum = atoi(Temp);
	fclose(fp);

	//PNT �ε�
	vector<D3DXVECTOR3> PNTtemp;
	PNTtemp.clear();
	fp = fopen("./save/PNTVertex", "rb");
	while (!feof(fp))
	{
		D3DXVECTOR3 i;
		fread(&i, sizeof(D3DXVECTOR3), 1, fp);
		if (i != '\0')
			PNTtemp.push_back(i);
	}
	fclose(fp);

	int Discriminant = nVertexNum * nVertexNum;
	if (PNTtemp.size() > 0 && Discriminant == (PNTtemp.size() - 1)) //temp�� �ְ�, �׸��� ����� ������
	{
		m_vecPNTVertex.clear();
		m_vecPNTVertex.resize(nVertexNum * nVertexNum * sizeof(ST_PNT_VERTEX));

		for (int z = 0; z < nVertexNum; ++z)
		{
			for (int x = 0; x < nVertexNum; ++x)
			{
				// 1�߹迭 ����� �ε��� ��ȣ ����
				int nIndex = z * nVertexNum + x;
				float y = 0.0f;

				D3DXVECTOR3 n;
				ComputeNormal(&n, &m_vecPNTVertex[nIndex + 0].p, &m_vecPNTVertex[nIndex + 1].p, &m_vecPNTVertex[nIndex + 2].p);

				// ĳ������ ��ġ�� ���� y �� ��ȯ�� ���ؼ� ���ؽ� ������ ���� ���� �����Ѵ�.
				m_vecPNTVertex[nIndex].p = PNTtemp[nIndex];
				m_vecPNTVertex[nIndex].n = n;
				m_vecPNTVertex[nIndex].t = D3DXVECTOR2(x, z);

				if (m_matWorld)
					D3DXVec3TransformCoord(&m_vecVertex[nIndex], &m_vecVertex[nIndex], &m_matWorld);
			}
		}
	}
	else if (PNTtemp.size() > 0 && Discriminant != (PNTtemp.size() - 1))
	{
		char str[300];
		wsprintf(str, "�׸��� ����� ��ġ���� �ʾ� ������� �ʽ��ϴ�.");
		MessageBox(g_hWnd, str, "���� �ҷ����� ����", MB_OK);
	}

	//�ؽ�ó �ε�
	string tName; //���� ������
	//char Temp[1024]; //�ӽ÷� ������ //������ ������.
	fp = fopen("./save/TextureName", "r");
	//fread(&i, sizeof(string), 1, fp);
	fgets(Temp, sizeof(Temp), fp);
	tName = Temp; //char�迭�� �׳� ������ �ڿ� �ΰ��� ������ string�� ����ش�.(string�� �˾Ƽ� ������ֱ� ������)
	fclose(fp);
	g_pTextureManager->AddTexture(tName, tName, &m_pTexture);
	m_pTexture = g_pTextureManager->GetTexture(tName);
}

void cTerrain::SaveAll()
{
	//���� ������
	FILE *fp;

	//���ؽ� ���� ����
	fp = fopen("./save/VertexNum", "w");
	char VertexNum_ch[1024];
	itoa(m_nVertexNum, VertexNum_ch, 10);
	fputs(VertexNum_ch, fp);
	fclose(fp);

	//PNT ����
	vector<D3DXVECTOR3> PNTtemp;
	for (int i = 0; i < m_vecVertex.size(); i++)
		PNTtemp.push_back(m_vecPNTVertex[i].p);
	fp = fopen("./save/PNTVertex", "wb");
	fwrite(&PNTtemp[0], PNTtemp.size() * sizeof(D3DXVECTOR3), 1, fp); //�߿�!! ������ * sizeof(Ÿ��)
	fclose(fp);

	//�ؽ��� ����
	fp = fopen("./save/TextureName", "w");
	//fwrite(&m_TextureName, m_TextureName.size() * sizeof(string), 1, fp); //�߿�!! ������ * sizeof(Ÿ��)
	fputs(m_TextureName.c_str(), fp);
	fclose(fp);

	//�� �������� �׸��尡 ��� ����Ǿ����� Ȯ�ο�
	char Gridstr[300];
	sprintf(Gridstr, "%d �׸��� ������� ����Ǿ����ϴ�.", (int)sqrtf(PNTtemp.size() - 1));
	MessageBox(g_hWnd, Gridstr, "���� ����", MB_OK);
}