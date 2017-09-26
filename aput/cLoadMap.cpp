#include "stdafx.h"
#include "cLoadMap.h"
#include "cXobjectLoader.h"

cLoadMap::cLoadMap()
	: m_fSizeX(1.0f)
	, m_fSizeZ(1.0f)
	, m_nVertexSize(0)
	, m_pTexture(NULL)
{
	D3DXMatrixIdentity(&m_matWorld); //Terrain(����)�� ��Ʈ���� ��ġ�� �ʱ�ȭ ��Ű�� ���� �۾�
}


cLoadMap::~cLoadMap()
{
	for (int i = 0; i < m_vecXLoader.size(); i++) // ������ ���� ������ ��� X���� ����
		m_vecXLoader[i]->Destroy();
}

void cLoadMap::Load(char* szFilePath, D3DXMATRIXA16* pMat)
{
	assert(pMat && "Load�� ����Ҽ� �����ϴ�. RealLoad�� ����ϼ��� !!!");
	assert(pMat || "Load�� ����Ҽ� �����ϴ�. RealLoad�� ����ϼ��� !!!");
}


bool cLoadMap::GetHeight(IN const float x, OUT float & y, IN const float z)
{
	if (x < 0 || x / m_fSizeX > (m_nVertexSize - 1) || z < 0 || z / m_fSizeZ > (m_nVertexSize - 1))
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
	int _0 = (nZ + 0) * m_nVertexSize + nX + 0;
	int _1 = (nZ + 1) * m_nVertexSize + nX + 0;
	int _2 = (nZ + 0) * m_nVertexSize + nX + 1;
	int _3 = (nZ + 1) * m_nVertexSize + nX + 1;

	// 1--3
	// |\ |
	// | \|
	// 0--2
	if (fDeltaX + fDeltaZ < 1.0f) // �Ʒ��� �ﰢ�� ���� ��ġ�ϴ� ���
	{
		// 1 - 0
		float zY = m_vecPNTVertex[_1].p.y - m_vecPNTVertex[_0].p.y;
		// 2 - 0
		float xY = m_vecPNTVertex[_2].p.y - m_vecPNTVertex[_0].p.y;
		// 0�� �ε����� y���� �������� ĳ���� ��ġ y�� ���
		y = m_vecPNTVertex[_0].p.y + zY * fDeltaZ + xY * fDeltaX;
	}
	else // ���� �ﰢ��
	{
		// 2 - 3
		float zY = m_vecPNTVertex[_2].p.y - m_vecPNTVertex[_3].p.y;
		// 1 - 3
		float xY = m_vecPNTVertex[_1].p.y - m_vecPNTVertex[_3].p.y;
		// 3�� �ε����� y���� �������� ĳ���� ��ġ y�� ���
		// ��Ÿ ���� 0�� �ε��� �������� ����� �߱� ������
		// 3�� �ε��� �������� ��� �� ���� ������ �ʿ� �ϴ�.
		y = m_vecPNTVertex[_3].p.y + zY * (1.0f - fDeltaZ) + xY * (1.0f - fDeltaX);
	}

	return true;
}

void cLoadMap::Render()
{
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, false);  //Terrain(����)�� ����Ʈ �̻��
	g_pD3DDevice->SetTransform(D3DTS_WORLD, &m_matWorld);
	g_pD3DDevice->SetTexture(0, m_pTexture);
	g_pD3DDevice->SetMaterial(&WHITE_MTRL);
	g_pD3DDevice->SetFVF(ST_PNT_VERTEX::FVF);
	g_pD3DDevice->DrawIndexedPrimitiveUP(
		D3DPT_TRIANGLELIST,				// ���� �׸��� Ÿ��
		0,								// �Ǿ�(����) ���ؽ��� �ε���
		m_vecPNTVertex.size(),			// ���Ǵ� ���ؽ��� ��
		m_vecIndex.size() / 3,			// ������ ��
		&m_vecIndex[0],					// �ε��� ������ ������ �迭
		D3DFMT_INDEX32,					// �ε��� ���� ( 16��Ʈ���� 32��Ʈ���� )
		&m_vecPNTVertex[0],				// ���ؽ� ������ ������ �迭
		sizeof(ST_PNT_VERTEX));			// ���ؽ��� ������

	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, true);  //������ ������ X������ ����Ʈ ���
	for (int i = 0; i < m_vecXLoader.size(); i++)
		m_vecXLoader[i]->Render();
}

void cLoadMap::SliceQuarterSetup(int VertexNum)
{
	//�ʱ� ���� �˻� �ε��� ����
	m_fastSearch[0] = 0;
	m_fastSearch[1] = (VertexNum - 1) / 2;
	m_fastSearch[2] = (VertexNum - 1);
	m_fastSearch[3] = (VertexNum - 1) * VertexNum / 2;
	m_fastSearch[4] = ((VertexNum - 1) * VertexNum / 2) + ((VertexNum - 1) / 2);
	m_fastSearch[5] = ((VertexNum - 1) * VertexNum / 2) + (VertexNum - 1);
	m_fastSearch[6] = (VertexNum - 1) * VertexNum;
	m_fastSearch[7] = ((VertexNum * VertexNum) - 1) - ((VertexNum - 1) / 2);
	m_fastSearch[8] = VertexNum * VertexNum - 1;

	/*
	6 7 8
	3 4 5
	0 1 2
	*/

	// ũ�� �ѹ� �˻��ϱ� ����, �б⺰�� ����.(1~4�б�)
	if (m_fastSearch[8] > 0)
	{
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[0]].p);//1~3
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[3]].p);
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[1]].p);
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[1]].p);//4~6 //1�б� (x < m_fastSearch[1]) (z < m_fastSearch[3])
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[3]].p);
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[4]].p);
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[1]].p);//7~9
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[4]].p);
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[2]].p);
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[2]].p);//10~12 //2�б�
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[4]].p);
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[5]].p);
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[3]].p);//13~15
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[6]].p);
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[4]].p);
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[4]].p);//16~18 //3�б�
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[6]].p);
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[7]].p);
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[4]].p);//19~21
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[7]].p);
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[5]].p);
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[5]].p);//22~24 //4�б�
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[7]].p);
		m_fastVertex.push_back(m_vecPNTVertex[m_fastSearch[8]].p);
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
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[0]].p);//1~3
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[3]].p);
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[1]].p);
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[1]].p);//4~6 //1�б� (x < m_fastSearchHalf[1]) (z < m_fastSearchHalf[3])
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[3]].p);
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[4]].p);
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[1]].p);//7~9
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[4]].p);
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[2]].p);
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[2]].p);//10~12 //2�б�
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[4]].p);
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[5]].p);
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[3]].p);//13~15
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[6]].p);
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[4]].p);
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[4]].p);//16~18 //3�б�
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[6]].p);
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[7]].p);
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[4]].p);//19~21
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[7]].p);
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[5]].p);
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[5]].p);//22~24 //4�б�
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[7]].p);
		m_fastVertexHalf01.push_back(m_vecPNTVertex[m_fastSearchHalf01[8]].p);
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
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[0]].p);//1~3
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[3]].p);
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[1]].p);
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[1]].p);//4~6 //1�б� (x < m_fastSearchHalf[1]) (z < m_fastSearchHalf[3])
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[3]].p);
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[4]].p);
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[1]].p);//7~9
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[4]].p);
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[2]].p);
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[2]].p);//10~12 //2�б�
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[4]].p);
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[5]].p);
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[3]].p);//13~15
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[6]].p);
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[4]].p);
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[4]].p);//16~18 //3�б�
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[6]].p);
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[7]].p);
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[4]].p);//19~21
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[7]].p);
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[5]].p);
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[5]].p);//22~24 //4�б�
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[7]].p);
		m_fastVertexHalf02.push_back(m_vecPNTVertex[m_fastSearchHalf02[8]].p);
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
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[0]].p);//1~3
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[3]].p);
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[1]].p);
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[1]].p);//4~6 //1�б� (x < m_fastSearchHalf[1]) (z < m_fastSearchHalf[3])
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[3]].p);
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[4]].p);
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[1]].p);//7~9
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[4]].p);
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[2]].p);
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[2]].p);//10~12 //2�б�
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[4]].p);
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[5]].p);
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[3]].p);//13~15
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[6]].p);
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[4]].p);
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[4]].p);//16~18 //3�б�
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[6]].p);
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[7]].p);
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[4]].p);//19~21
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[7]].p);
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[5]].p);
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[5]].p);//22~24 //4�б�
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[7]].p);
		m_fastVertexHalf03.push_back(m_vecPNTVertex[m_fastSearchHalf03[8]].p);
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
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[0]].p);//1~3
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[3]].p);
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[1]].p);
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[1]].p);//4~6 //1�б� (x < m_fastSearchHalf[1]) (z < m_fastSearchHalf[3])
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[3]].p);
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[4]].p);
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[1]].p);//7~9
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[4]].p);
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[2]].p);
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[2]].p);//10~12 //2�б�
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[4]].p);
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[5]].p);
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[3]].p);//13~15
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[6]].p);
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[4]].p);
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[4]].p);//16~18 //3�б�
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[6]].p);
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[7]].p);
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[4]].p);//19~21
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[7]].p);
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[5]].p);
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[5]].p);//22~24 //4�б�
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[7]].p);
		m_fastVertexHalf04.push_back(m_vecPNTVertex[m_fastSearchHalf04[8]].p);
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
			int _0 = (z + 0) * VertexNum + x + 0;
			int _1 = (z + 1) * VertexNum + x + 0;
			int _2 = (z + 0) * VertexNum + x + 1;
			int _3 = (z + 1) * VertexNum + x + 1;

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
			int _0 = (z + 0) * VertexNum + x + 0;
			int _1 = (z + 1) * VertexNum + x + 0;
			int _2 = (z + 0) * VertexNum + x + 1;
			int _3 = (z + 1) * VertexNum + x + 1;

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
			int _0 = (z + 0) * VertexNum + x + 0;
			int _1 = (z + 1) * VertexNum + x + 0;
			int _2 = (z + 0) * VertexNum + x + 1;
			int _3 = (z + 1) * VertexNum + x + 1;

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
			int _0 = (z + 0) * VertexNum + x + 0;
			int _1 = (z + 1) * VertexNum + x + 0;
			int _2 = (z + 0) * VertexNum + x + 1;
			int _3 = (z + 1) * VertexNum + x + 1;

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
			int _0 = (z + 0) * VertexNum + x + 0;
			int _1 = (z + 1) * VertexNum + x + 0;
			int _2 = (z + 0) * VertexNum + x + 1;
			int _3 = (z + 1) * VertexNum + x + 1;

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
			int _0 = (z + 0) * VertexNum + x + 0;
			int _1 = (z + 1) * VertexNum + x + 0;
			int _2 = (z + 0) * VertexNum + x + 1;
			int _3 = (z + 1) * VertexNum + x + 1;

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
			int _0 = (z + 0) * VertexNum + x + 0;
			int _1 = (z + 1) * VertexNum + x + 0;
			int _2 = (z + 0) * VertexNum + x + 1;
			int _3 = (z + 1) * VertexNum + x + 1;

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
			int _0 = (z + 0) * VertexNum + x + 0;
			int _1 = (z + 1) * VertexNum + x + 0;
			int _2 = (z + 0) * VertexNum + x + 1;
			int _3 = (z + 1) * VertexNum + x + 1;

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
			int _0 = (z + 0) * VertexNum + x + 0;
			int _1 = (z + 1) * VertexNum + x + 0;
			int _2 = (z + 0) * VertexNum + x + 1;
			int _3 = (z + 1) * VertexNum + x + 1;

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
			int _0 = (z + 0) * VertexNum + x + 0;
			int _1 = (z + 1) * VertexNum + x + 0;
			int _2 = (z + 0) * VertexNum + x + 1;
			int _3 = (z + 1) * VertexNum + x + 1;

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
			int _0 = (z + 0) * VertexNum + x + 0;
			int _1 = (z + 1) * VertexNum + x + 0;
			int _2 = (z + 0) * VertexNum + x + 1;
			int _3 = (z + 1) * VertexNum + x + 1;

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
			int _0 = (z + 0) * VertexNum + x + 0;
			int _1 = (z + 1) * VertexNum + x + 0;
			int _2 = (z + 0) * VertexNum + x + 1;
			int _3 = (z + 1) * VertexNum + x + 1;

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
			int _0 = (z + 0) * VertexNum + x + 0;
			int _1 = (z + 1) * VertexNum + x + 0;
			int _2 = (z + 0) * VertexNum + x + 1;
			int _3 = (z + 1) * VertexNum + x + 1;

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
			int _0 = (z + 0) * VertexNum + x + 0;
			int _1 = (z + 1) * VertexNum + x + 0;
			int _2 = (z + 0) * VertexNum + x + 1;
			int _3 = (z + 1) * VertexNum + x + 1;

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
			int _0 = (z + 0) * VertexNum + x + 0;
			int _1 = (z + 1) * VertexNum + x + 0;
			int _2 = (z + 0) * VertexNum + x + 1;
			int _3 = (z + 1) * VertexNum + x + 1;

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
			int _0 = (z + 0) * VertexNum + x + 0;
			int _1 = (z + 1) * VertexNum + x + 0;
			int _2 = (z + 0) * VertexNum + x + 1;
			int _3 = (z + 1) * VertexNum + x + 1;

			m_vecfast16Index.push_back(_0);
			m_vecfast16Index.push_back(_1);
			m_vecfast16Index.push_back(_2);
			m_vecfast16Index.push_back(_2);
			m_vecfast16Index.push_back(_1);
			m_vecfast16Index.push_back(_3);
		}
	}
}

void cLoadMap::SliceQuarterCalculation(D3DXVECTOR3 & vDest)
{
	D3DXVECTOR3 vRayOrig(vDest.x, vDest.y + 10, vDest.z);
	D3DXVECTOR3 vRayDir(0, -1, 0);

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
			&vRayOrig, &vRayDir, NULL, NULL, &f))
		{
			if (f < minF)
			{
				minF = f;
				isBig = true;
				searchBig = i + 0;

				break;
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
					&vRayOrig, &vRayDir, NULL, NULL, &f))
				{
					if (f < minF)
					{
						minF = f;
						isSmall = true;
						searchSmall = i + 0;

						break;
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
					&vRayOrig, &vRayDir, NULL, NULL, &f))
				{
					if (f < minF)
					{
						minF = f;
						isSmall = true;
						searchSmall = i + 0;

						break;
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
					&vRayOrig, &vRayDir, NULL, NULL, &f))
				{
					if (f < minF)
					{
						minF = f;
						isSmall = true;
						searchSmall = i + 0;

						break;
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
					&vRayOrig, &vRayDir, NULL, NULL, &f))
				{
					if (f < minF)
					{
						minF = f;
						isSmall = true;
						searchSmall = i + 0;

						break;
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
						&vRayOrig, &vRayDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							vDest = vRayOrig + vRayDir * f;
							m_vecfast01Index[i + 0];

							break;
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
						&vRayOrig, &vRayDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							vDest = vRayOrig + vRayDir * f;
							m_vecfast02Index[i + 0];

							break;
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
						&vRayOrig, &vRayDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							vDest = vRayOrig + vRayDir * f;
							m_vecfast03Index[i + 0];

							break;
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
						&vRayOrig, &vRayDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							vDest = vRayOrig + vRayDir * f;
							m_vecfast04Index[i + 0];

							break;
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
						&vRayOrig, &vRayDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							vDest = vRayOrig + vRayDir * f;
							m_vecfast05Index[i + 0];

							break;
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
						&vRayOrig, &vRayDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							vDest = vRayOrig + vRayDir * f;
							m_vecfast06Index[i + 0];

							break;
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
						&vRayOrig, &vRayDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							vDest = vRayOrig + vRayDir * f;
							m_vecfast07Index[i + 0];

							break;
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
						&vRayOrig, &vRayDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							vDest = vRayOrig + vRayDir * f;
							m_vecfast08Index[i + 0];

							break;
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
						&vRayOrig, &vRayDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							vDest = vRayOrig + vRayDir * f;
							m_vecfast09Index[i + 0];

							break;
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
						&vRayOrig, &vRayDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							vDest = vRayOrig + vRayDir * f;
							m_vecfast10Index[i + 0];

							break;
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
						&vRayOrig, &vRayDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							vDest = vRayOrig + vRayDir * f;
							m_vecfast11Index[i + 0];

							break;
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
						&vRayOrig, &vRayDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							vDest = vRayOrig + vRayDir * f;
							m_vecfast12Index[i + 0];

							break;
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
						&vRayOrig, &vRayDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							vDest = vRayOrig + vRayDir * f;
							m_vecfast13Index[i + 0];

							break;
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
						&vRayOrig, &vRayDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							vDest = vRayOrig + vRayDir * f;
							m_vecfast14Index[i + 0];

							break;
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
						&vRayOrig, &vRayDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							vDest = vRayOrig + vRayDir * f;
							m_vecfast15Index[i + 0];

							break;
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
						&vRayOrig, &vRayDir, NULL, NULL, &f))
					{
						if (f < minF)
						{
							minF = f;
							vDest = vRayOrig + vRayDir * f;
							m_vecfast16Index[i + 0];

							break;
						}
					}
				}
			}
		}
	}
}

/*============================================================================*/
//MainScene
void cLoadMap::RealLoad_MainScene()
{
	LoadAll_MainSceneTerrain();
	LoadAll_MainSceneX();
}

void cLoadMap::LoadAll_MainSceneTerrain()
{
	FILE* fp;

	//���ؽ� ���� �ε�
	fp = fopen("./save/MainScene/VertexNum", "r");
	char Temp[1024]; //���� �ӽ� �����.
	fgets(Temp, sizeof(Temp), fp);
	int nVertexNum = atoi(Temp);
	fclose(fp);

	m_nVertexSize = nVertexNum;

	//�ε��� ����
	for (int z = 0; z < nVertexNum - 1; ++z)
	{
		for (int x = 0; x < nVertexNum - 1; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * nVertexNum + x + 0;
			int _1 = (z + 1) * nVertexNum + x + 0;
			int _2 = (z + 0) * nVertexNum + x + 1;
			int _3 = (z + 1) * nVertexNum + x + 1;

			m_vecIndex.push_back(_0);
			m_vecIndex.push_back(_1);
			m_vecIndex.push_back(_2);
			m_vecIndex.push_back(_2);
			m_vecIndex.push_back(_1);
			m_vecIndex.push_back(_3);
		}
	}

	//PNT �ε�
	vector<D3DXVECTOR3> PNTtemp;
	PNTtemp.clear();
	fp = fopen("./save/MainScene/PNTVertex", "rb");
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
	fp = fopen("./save/MainScene/TextureName", "r");
	//fread(&i, sizeof(string), 1, fp);
	fgets(Temp, sizeof(Temp), fp);
	tName = Temp; //char�迭�� �׳� ������ �ڿ� �ΰ��� ������ string�� ����ش�.(string�� �˾Ƽ� ������ֱ� ������)
	fclose(fp);
	g_pTextureManager->AddTexture(tName, tName, &m_pTexture);
	m_pTexture = g_pTextureManager->GetTexture(tName);


	//����ȭ �ʱ� ����
	SliceQuarterSetup(nVertexNum);
}

void cLoadMap::LoadAll_MainSceneX()
{
	FILE* fp;
	string fileName;
	//X ���� �ε�

	//X ���� �� ����(��ü ���� �ľ��� ���� ���̱⿡, �ѹ��� ���ָ��)
	fp = fopen("./save/MainScene/XTotalNum", "r");
	char Temp[1024]; //���� �ӽ� �����.
	fgets(Temp, sizeof(Temp), fp);
	int nXTotalNum = atoi(Temp);
	fclose(fp);

	fp = fopen("./save/MainScene/XTotalSize", "r");
	fgets(Temp, sizeof(Temp), fp);
	int nXTotalSize = atoi(Temp);
	fclose(fp);

	if (nXTotalNum > 0) // �����ϴ� ��쿡��
	{
		for (int i = 0; i < nXTotalNum; i++)
		{
			//�浹ü ����
			fileName = "./save/MainScene/XisSphere";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "r");
			fgets(Temp, sizeof(Temp), fp);
			bool bisSphere = atoi(Temp);
			fclose(fp);

			//���� �̸�
			fileName = "./save/MainScene/XFileName";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "r");
			fgets(Temp, sizeof(Temp), fp);
			fclose(fp);

			cXobjectLoader* temp = new cXobjectLoader;
			temp->Load(Temp, bisSphere);
			m_vecXLoader.push_back(temp);

			//�Ž� ��Ʈ����
			fileName = "./save/MainScene/XMatW";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "rb");
			while (!feof(fp))
			{
				D3DXMATRIXA16 MatW;
				fread(&MatW, sizeof(D3DXMATRIXA16), 1, fp);

				if (MatW != '\0')
					m_vecXLoader[i]->SetMeshMatrix(MatW);
			}
			fclose(fp);

			//�Ž� �浹ü ��Ʈ����
			fileName = "./save/MainScene/XCollisionMatW";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "rb");
			while (!feof(fp))
			{
				D3DXMATRIXA16 CollisionMatW;
				fread(&CollisionMatW, sizeof(D3DXMATRIXA16), 1, fp);

				if (CollisionMatW != '\0')
					m_vecXLoader[i]->SetCollisionMatrix(CollisionMatW);
			}
			fclose(fp);
		}
	}
}

/*============================================================================*/
//Village
void cLoadMap::RealLoad_Village()
{
	LoadAll_VillageTerrain();
	LoadAll_VillageX();
}

void cLoadMap::LoadAll_VillageTerrain()
{
	FILE* fp;

	//���ؽ� ���� �ε�
	fp = fopen("./save/Village/VertexNum", "r");
	char Temp[1024]; //���� �ӽ� �����.
	fgets(Temp, sizeof(Temp), fp);
	int nVertexNum = atoi(Temp);
	fclose(fp);

	m_nVertexSize = nVertexNum;

	//�ε��� ����
	for (int z = 0; z < nVertexNum - 1; ++z)
	{
		for (int x = 0; x < nVertexNum - 1; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * nVertexNum + x + 0;
			int _1 = (z + 1) * nVertexNum + x + 0;
			int _2 = (z + 0) * nVertexNum + x + 1;
			int _3 = (z + 1) * nVertexNum + x + 1;

			m_vecIndex.push_back(_0);
			m_vecIndex.push_back(_1);
			m_vecIndex.push_back(_2);
			m_vecIndex.push_back(_2);
			m_vecIndex.push_back(_1);
			m_vecIndex.push_back(_3);
		}
	}

	//PNT �ε�
	vector<D3DXVECTOR3> PNTtemp;
	PNTtemp.clear();
	fp = fopen("./save/Village/PNTVertex", "rb");
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
	fp = fopen("./save/Village/TextureName", "r");
	//fread(&i, sizeof(string), 1, fp);
	fgets(Temp, sizeof(Temp), fp);
	tName = Temp; //char�迭�� �׳� ������ �ڿ� �ΰ��� ������ string�� ����ش�.(string�� �˾Ƽ� ������ֱ� ������)
	fclose(fp);
	g_pTextureManager->AddTexture(tName, tName, &m_pTexture);
	m_pTexture = g_pTextureManager->GetTexture(tName);


	//����ȭ �ʱ� ����
	SliceQuarterSetup(nVertexNum);
}

void cLoadMap::LoadAll_VillageX()
{
	FILE* fp;
	string fileName;
	//X ���� �ε�

	//X ���� �� ����(��ü ���� �ľ��� ���� ���̱⿡, �ѹ��� ���ָ��)
	fp = fopen("./save/Village/XTotalNum", "r");
	char Temp[1024]; //���� �ӽ� �����.
	fgets(Temp, sizeof(Temp), fp);
	int nXTotalNum = atoi(Temp);
	fclose(fp);

	fp = fopen("./save/Village/XTotalSize", "r");
	fgets(Temp, sizeof(Temp), fp);
	int nXTotalSize = atoi(Temp);
	fclose(fp);

	if (nXTotalNum > 0) // �����ϴ� ��쿡��
	{
		for (int i = 0; i < nXTotalNum; i++)
		{
			//�浹ü ����
			fileName = "./save/Village/XisSphere";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "r");
			fgets(Temp, sizeof(Temp), fp);
			bool bisSphere = atoi(Temp);
			fclose(fp);

			//���� �̸�
			fileName = "./save/Village/XFileName";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "r");
			fgets(Temp, sizeof(Temp), fp);
			fclose(fp);

			cXobjectLoader* temp = new cXobjectLoader;
			temp->Load(Temp, bisSphere);
			m_vecXLoader.push_back(temp);

			//�Ž� ��Ʈ����
			fileName = "./save/Village/XMatW";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "rb");
			while (!feof(fp))
			{
				D3DXMATRIXA16 MatW;
				fread(&MatW, sizeof(D3DXMATRIXA16), 1, fp);

				if (MatW != '\0')
					m_vecXLoader[i]->SetMeshMatrix(MatW);
			}
			fclose(fp);

			//�Ž� �浹ü ��Ʈ����
			fileName = "./save/Village/XCollisionMatW";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "rb");
			while (!feof(fp))
			{
				D3DXMATRIXA16 CollisionMatW;
				fread(&CollisionMatW, sizeof(D3DXMATRIXA16), 1, fp);

				if (CollisionMatW != '\0')
					m_vecXLoader[i]->SetCollisionMatrix(CollisionMatW);
			}
			fclose(fp);
		}
	}
}

/*============================================================================*/
//Stage1
void cLoadMap::RealLoad_Stage1()
{
	LoadAll_Stage1Terrain();
	LoadAll_Stage1X();
}

void cLoadMap::LoadAll_Stage1Terrain()
{
	FILE* fp;

	//���ؽ� ���� �ε�
	fp = fopen("./save/Stage1/VertexNum", "r");
	char Temp[1024]; //���� �ӽ� �����.
	fgets(Temp, sizeof(Temp), fp);
	int nVertexNum = atoi(Temp);
	fclose(fp);

	m_nVertexSize = nVertexNum;

	//�ε��� ����
	for (int z = 0; z < nVertexNum - 1; ++z)
	{
		for (int x = 0; x < nVertexNum - 1; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * nVertexNum + x + 0;
			int _1 = (z + 1) * nVertexNum + x + 0;
			int _2 = (z + 0) * nVertexNum + x + 1;
			int _3 = (z + 1) * nVertexNum + x + 1;

			m_vecIndex.push_back(_0);
			m_vecIndex.push_back(_1);
			m_vecIndex.push_back(_2);
			m_vecIndex.push_back(_2);
			m_vecIndex.push_back(_1);
			m_vecIndex.push_back(_3);
		}
	}

	//PNT �ε�
	vector<D3DXVECTOR3> PNTtemp;
	PNTtemp.clear();
	fp = fopen("./save/Stage1/PNTVertex", "rb");
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
	fp = fopen("./save/Stage1/TextureName", "r");
	//fread(&i, sizeof(string), 1, fp);
	fgets(Temp, sizeof(Temp), fp);
	tName = Temp; //char�迭�� �׳� ������ �ڿ� �ΰ��� ������ string�� ����ش�.(string�� �˾Ƽ� ������ֱ� ������)
	fclose(fp);
	g_pTextureManager->AddTexture(tName, tName, &m_pTexture);
	m_pTexture = g_pTextureManager->GetTexture(tName);


	//����ȭ �ʱ� ����
	SliceQuarterSetup(nVertexNum);
}

void cLoadMap::LoadAll_Stage1X()
{
	FILE* fp;
	string fileName;
	//X ���� �ε�

	//X ���� �� ����(��ü ���� �ľ��� ���� ���̱⿡, �ѹ��� ���ָ��)
	fp = fopen("./save/Stage1/XTotalNum", "r");
	char Temp[1024]; //���� �ӽ� �����.
	fgets(Temp, sizeof(Temp), fp);
	int nXTotalNum = atoi(Temp);
	fclose(fp);

	fp = fopen("./save/Stage1/XTotalSize", "r");
	fgets(Temp, sizeof(Temp), fp);
	int nXTotalSize = atoi(Temp);
	fclose(fp);

	if (nXTotalNum > 0) // �����ϴ� ��쿡��
	{
		for (int i = 0; i < nXTotalNum; i++)
		{
			//�浹ü ����
			fileName = "./save/Stage1/XisSphere";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "r");
			fgets(Temp, sizeof(Temp), fp);
			bool bisSphere = atoi(Temp);
			fclose(fp);

			//���� �̸�
			fileName = "./save/Stage1/XFileName";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "r");
			fgets(Temp, sizeof(Temp), fp);
			fclose(fp);

			cXobjectLoader* temp = new cXobjectLoader;
			temp->Load(Temp, bisSphere);
			m_vecXLoader.push_back(temp);

			//�Ž� ��Ʈ����
			fileName = "./save/Stage1/XMatW";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "rb");
			while (!feof(fp))
			{
				D3DXMATRIXA16 MatW;
				fread(&MatW, sizeof(D3DXMATRIXA16), 1, fp);

				if (MatW != '\0')
					m_vecXLoader[i]->SetMeshMatrix(MatW);
			}
			fclose(fp);

			//�Ž� �浹ü ��Ʈ����
			fileName = "./save/Stage1/XCollisionMatW";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "rb");
			while (!feof(fp))
			{
				D3DXMATRIXA16 CollisionMatW;
				fread(&CollisionMatW, sizeof(D3DXMATRIXA16), 1, fp);

				if (CollisionMatW != '\0')
					m_vecXLoader[i]->SetCollisionMatrix(CollisionMatW);
			}
			fclose(fp);
		}
	}
}

/*============================================================================*/
//Stage2
void cLoadMap::RealLoad_Stage2()
{
	LoadAll_Stage2Terrain();
	LoadAll_Stage2X();
}

void cLoadMap::LoadAll_Stage2Terrain()
{
	FILE* fp;

	//���ؽ� ���� �ε�
	fp = fopen("./save/Stage2/VertexNum", "r");
	char Temp[1024]; //���� �ӽ� �����.
	fgets(Temp, sizeof(Temp), fp);
	int nVertexNum = atoi(Temp);
	fclose(fp);

	m_nVertexSize = nVertexNum;

	//�ε��� ����
	for (int z = 0; z < nVertexNum - 1; ++z)
	{
		for (int x = 0; x < nVertexNum - 1; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * nVertexNum + x + 0;
			int _1 = (z + 1) * nVertexNum + x + 0;
			int _2 = (z + 0) * nVertexNum + x + 1;
			int _3 = (z + 1) * nVertexNum + x + 1;

			m_vecIndex.push_back(_0);
			m_vecIndex.push_back(_1);
			m_vecIndex.push_back(_2);
			m_vecIndex.push_back(_2);
			m_vecIndex.push_back(_1);
			m_vecIndex.push_back(_3);
		}
	}

	//PNT �ε�
	vector<D3DXVECTOR3> PNTtemp;
	PNTtemp.clear();
	fp = fopen("./save/Stage2/PNTVertex", "rb");
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
	fp = fopen("./save/Stage2/TextureName", "r");
	//fread(&i, sizeof(string), 1, fp);
	fgets(Temp, sizeof(Temp), fp);
	tName = Temp; //char�迭�� �׳� ������ �ڿ� �ΰ��� ������ string�� ����ش�.(string�� �˾Ƽ� ������ֱ� ������)
	fclose(fp);
	g_pTextureManager->AddTexture(tName, tName, &m_pTexture);
	m_pTexture = g_pTextureManager->GetTexture(tName);


	//����ȭ �ʱ� ����
	SliceQuarterSetup(nVertexNum);
}

void cLoadMap::LoadAll_Stage2X()
{
	FILE* fp;
	string fileName;
	//X ���� �ε�

	//X ���� �� ����(��ü ���� �ľ��� ���� ���̱⿡, �ѹ��� ���ָ��)
	fp = fopen("./save/Stage2/XTotalNum", "r");
	char Temp[1024]; //���� �ӽ� �����.
	fgets(Temp, sizeof(Temp), fp);
	int nXTotalNum = atoi(Temp);
	fclose(fp);

	fp = fopen("./save/Stage2/XTotalSize", "r");
	fgets(Temp, sizeof(Temp), fp);
	int nXTotalSize = atoi(Temp);
	fclose(fp);

	if (nXTotalNum > 0) // �����ϴ� ��쿡��
	{
		for (int i = 0; i < nXTotalNum; i++)
		{
			//�浹ü ����
			fileName = "./save/Stage2/XisSphere";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "r");
			fgets(Temp, sizeof(Temp), fp);
			bool bisSphere = atoi(Temp);
			fclose(fp);

			//���� �̸�
			fileName = "./save/Stage2/XFileName";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "r");
			fgets(Temp, sizeof(Temp), fp);
			fclose(fp);

			cXobjectLoader* temp = new cXobjectLoader;
			temp->Load(Temp, bisSphere);
			m_vecXLoader.push_back(temp);

			//�Ž� ��Ʈ����
			fileName = "./save/Stage2/XMatW";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "rb");
			while (!feof(fp))
			{
				D3DXMATRIXA16 MatW;
				fread(&MatW, sizeof(D3DXMATRIXA16), 1, fp);

				if (MatW != '\0')
					m_vecXLoader[i]->SetMeshMatrix(MatW);
			}
			fclose(fp);

			//�Ž� �浹ü ��Ʈ����
			fileName = "./save/Stage2/XCollisionMatW";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "rb");
			while (!feof(fp))
			{
				D3DXMATRIXA16 CollisionMatW;
				fread(&CollisionMatW, sizeof(D3DXMATRIXA16), 1, fp);

				if (CollisionMatW != '\0')
					m_vecXLoader[i]->SetCollisionMatrix(CollisionMatW);
			}
			fclose(fp);
		}
	}
}

/*============================================================================*/
//Stage3
void cLoadMap::RealLoad_Stage3()
{
	LoadAll_Stage3Terrain();
	LoadAll_Stage3X();
}

void cLoadMap::LoadAll_Stage3Terrain()
{
	FILE* fp;

	//���ؽ� ���� �ε�
	fp = fopen("./save/Stage3/VertexNum", "r");
	char Temp[1024]; //���� �ӽ� �����.
	fgets(Temp, sizeof(Temp), fp);
	int nVertexNum = atoi(Temp);
	fclose(fp);

	m_nVertexSize = nVertexNum;

	//�ε��� ����
	for (int z = 0; z < nVertexNum - 1; ++z)
	{
		for (int x = 0; x < nVertexNum - 1; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * nVertexNum + x + 0;
			int _1 = (z + 1) * nVertexNum + x + 0;
			int _2 = (z + 0) * nVertexNum + x + 1;
			int _3 = (z + 1) * nVertexNum + x + 1;

			m_vecIndex.push_back(_0);
			m_vecIndex.push_back(_1);
			m_vecIndex.push_back(_2);
			m_vecIndex.push_back(_2);
			m_vecIndex.push_back(_1);
			m_vecIndex.push_back(_3);
		}
	}

	//PNT �ε�
	vector<D3DXVECTOR3> PNTtemp;
	PNTtemp.clear();
	fp = fopen("./save/Stage3/PNTVertex", "rb");
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
	fp = fopen("./save/Stage3/TextureName", "r");
	//fread(&i, sizeof(string), 1, fp);
	fgets(Temp, sizeof(Temp), fp);
	tName = Temp; //char�迭�� �׳� ������ �ڿ� �ΰ��� ������ string�� ����ش�.(string�� �˾Ƽ� ������ֱ� ������)
	fclose(fp);
	g_pTextureManager->AddTexture(tName, tName, &m_pTexture);
	m_pTexture = g_pTextureManager->GetTexture(tName);


	//����ȭ �ʱ� ����
	SliceQuarterSetup(nVertexNum);
}

void cLoadMap::LoadAll_Stage3X()
{
	FILE* fp;
	string fileName;
	//X ���� �ε�

	//X ���� �� ����(��ü ���� �ľ��� ���� ���̱⿡, �ѹ��� ���ָ��)
	fp = fopen("./save/Stage3/XTotalNum", "r");
	char Temp[1024]; //���� �ӽ� �����.
	fgets(Temp, sizeof(Temp), fp);
	int nXTotalNum = atoi(Temp);
	fclose(fp);

	fp = fopen("./save/Stage3/XTotalSize", "r");
	fgets(Temp, sizeof(Temp), fp);
	int nXTotalSize = atoi(Temp);
	fclose(fp);

	if (nXTotalNum > 0) // �����ϴ� ��쿡��
	{
		for (int i = 0; i < nXTotalNum; i++)
		{
			//�浹ü ����
			fileName = "./save/Stage3/XisSphere";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "r");
			fgets(Temp, sizeof(Temp), fp);
			bool bisSphere = atoi(Temp);
			fclose(fp);

			//���� �̸�
			fileName = "./save/Stage3/XFileName";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "r");
			fgets(Temp, sizeof(Temp), fp);
			fclose(fp);

			cXobjectLoader* temp = new cXobjectLoader;
			temp->Load(Temp, bisSphere);
			m_vecXLoader.push_back(temp);

			//�Ž� ��Ʈ����
			fileName = "./save/Stage3/XMatW";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "rb");
			while (!feof(fp))
			{
				D3DXMATRIXA16 MatW;
				fread(&MatW, sizeof(D3DXMATRIXA16), 1, fp);

				if (MatW != '\0')
					m_vecXLoader[i]->SetMeshMatrix(MatW);
			}
			fclose(fp);

			//�Ž� �浹ü ��Ʈ����
			fileName = "./save/Stage3/XCollisionMatW";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "rb");
			while (!feof(fp))
			{
				D3DXMATRIXA16 CollisionMatW;
				fread(&CollisionMatW, sizeof(D3DXMATRIXA16), 1, fp);

				if (CollisionMatW != '\0')
					m_vecXLoader[i]->SetCollisionMatrix(CollisionMatW);
			}
			fclose(fp);
		}
	}
}

/*============================================================================*/
//BossMap
void cLoadMap::RealLoad_BossMap()
{
	LoadAll_BossMapTerrain();
	LoadAll_BossMapX();
}

void cLoadMap::LoadAll_BossMapTerrain()
{
	FILE* fp;

	//���ؽ� ���� �ε�
	fp = fopen("./save/BossMap/VertexNum", "r");
	char Temp[1024]; //���� �ӽ� �����.
	fgets(Temp, sizeof(Temp), fp);
	int nVertexNum = atoi(Temp);
	fclose(fp);

	m_nVertexSize = nVertexNum;

	//�ε��� ����
	for (int z = 0; z < nVertexNum - 1; ++z)
	{
		for (int x = 0; x < nVertexNum - 1; ++x)
		{
			// 1--3
			// |\ |
			// | \|
			// 0--2
			int _0 = (z + 0) * nVertexNum + x + 0;
			int _1 = (z + 1) * nVertexNum + x + 0;
			int _2 = (z + 0) * nVertexNum + x + 1;
			int _3 = (z + 1) * nVertexNum + x + 1;

			m_vecIndex.push_back(_0);
			m_vecIndex.push_back(_1);
			m_vecIndex.push_back(_2);
			m_vecIndex.push_back(_2);
			m_vecIndex.push_back(_1);
			m_vecIndex.push_back(_3);
		}
	}

	//PNT �ε�
	vector<D3DXVECTOR3> PNTtemp;
	PNTtemp.clear();
	fp = fopen("./save/BossMap/PNTVertex", "rb");
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

		m_vecVertex.clear();	//�߰�
		m_vecVertex.resize(nVertexNum * nVertexNum);	//�߰�

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
				m_vecVertex[nIndex] = m_vecPNTVertex[nIndex].p;	//�߰�
				m_vecPNTVertex[nIndex].n = n;
				m_vecPNTVertex[nIndex].t = D3DXVECTOR2(x, z);
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
	fp = fopen("./save/BossMap/TextureName", "r");
	//fread(&i, sizeof(string), 1, fp);
	fgets(Temp, sizeof(Temp), fp);
	tName = Temp; //char�迭�� �׳� ������ �ڿ� �ΰ��� ������ string�� ����ش�.(string�� �˾Ƽ� ������ֱ� ������)
	fclose(fp);
	g_pTextureManager->AddTexture(tName, tName, &m_pTexture);
	m_pTexture = g_pTextureManager->GetTexture(tName);


	//����ȭ �ʱ� ����
	SliceQuarterSetup(nVertexNum);
}

void cLoadMap::LoadAll_BossMapX()
{
	FILE* fp;
	string fileName;
	//X ���� �ε�

	//X ���� �� ����(��ü ���� �ľ��� ���� ���̱⿡, �ѹ��� ���ָ��)
	fp = fopen("./save/BossMap/XTotalNum", "r");
	char Temp[1024]; //���� �ӽ� �����.
	fgets(Temp, sizeof(Temp), fp);
	int nXTotalNum = atoi(Temp);
	fclose(fp);

	fp = fopen("./save/BossMap/XTotalSize", "r");
	fgets(Temp, sizeof(Temp), fp);
	int nXTotalSize = atoi(Temp);
	fclose(fp);

	if (nXTotalNum > 0) // �����ϴ� ��쿡��
	{
		for (int i = 0; i < nXTotalNum; i++)
		{
			//�浹ü ����
			fileName = "./save/BossMap/XisSphere";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "r");
			fgets(Temp, sizeof(Temp), fp);
			bool bisSphere = atoi(Temp);
			fclose(fp);

			//���� �̸�
			fileName = "./save/BossMap/XFileName";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "r");
			fgets(Temp, sizeof(Temp), fp);
			fclose(fp);

			cXobjectLoader* temp = new cXobjectLoader;
			temp->Load(Temp, bisSphere);
			m_vecXLoader.push_back(temp);

			//�Ž� ��Ʈ����
			fileName = "./save/BossMap/XMatW";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "rb");
			while (!feof(fp))
			{
				D3DXMATRIXA16 MatW;
				fread(&MatW, sizeof(D3DXMATRIXA16), 1, fp);

				if (MatW != '\0')
					m_vecXLoader[i]->SetMeshMatrix(MatW);
			}
			fclose(fp);

			//�Ž� �浹ü ��Ʈ����
			fileName = "./save/BossMap/XCollisionMatW";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "rb");
			while (!feof(fp))
			{
				D3DXMATRIXA16 CollisionMatW;
				fread(&CollisionMatW, sizeof(D3DXMATRIXA16), 1, fp);

				if (CollisionMatW != '\0')
					m_vecXLoader[i]->SetCollisionMatrix(CollisionMatW);
			}
			fclose(fp);
		}
	}
}