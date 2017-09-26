#include "stdafx.h"
#include "cObjMap.h"


cObjMap::cObjMap()
{
}


cObjMap::~cObjMap()
{
}

void cObjMap::Load(char* szFilePath, D3DXMATRIXA16* pMat)
{
	vector<D3DXVECTOR3> vecV;

	FILE* fp = NULL;
	fopen_s(&fp, szFilePath, "r");

	while (true)
	{
		if (feof(fp)) break;

		char szTemp[1024];
		fgets(szTemp, 1024, fp);

		if (szTemp[0] == 'v')
		{
			if (szTemp[1] == 't') // vt
			{
			}
			else if (szTemp[1] == 'n') // vn
			{
			}
			else // v
			{
				float x, y, z;
				sscanf(szTemp, "%*s %f %f %f", &x, &z, &y);
				vecV.push_back(D3DXVECTOR3(x, y, -z));
			}
		}
		else if (szTemp[0] == 'f')
		{
			int aIndex[3];
			sscanf(szTemp, "%*s %d/%*d/%*d %d/%*d/%*d %d/%*d/%*d",
				&aIndex[0], &aIndex[1], &aIndex[2]);

			for (int i = 0; i < 3; ++i)
			{
				D3DXVECTOR3 p;
				//m_vecIndex.push_back(aIndex[i] - 1);	//Ãß°¡
				p = vecV[aIndex[i] - 1];

				if (pMat)
				{
					D3DXVec3TransformCoord(&p, &p, pMat);
				}
				m_vecVertex.push_back(p);
			}
		}
	}

	fclose(fp);
}

bool cObjMap::GetHeight(IN const float x, OUT float& y, IN const float z)
{
	cRay ray;
	ray.m_vOrg = D3DXVECTOR3(x, y + 1, z);
	ray.m_vDir = D3DXVECTOR3(0, -1, 0);

	float d;

	for (int i = 0; i < m_vecVertex.size(); i += 3)
	{
		if (D3DXIntersectTri(&m_vecVertex[i + 0], &m_vecVertex[i + 1],
			&m_vecVertex[i + 2], &ray.m_vOrg, &ray.m_vDir, NULL, NULL, &d))
		{
			y = ray.m_vOrg.y - d;
			return true;
		}
	}

	return false;
}

void cObjMap::Render()
{
	D3DXMATRIXA16 mat;
	D3DXMatrixIdentity(&mat);

	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, false);
	g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	g_pD3DDevice->SetTexture(0, NULL);
	g_pD3DDevice->SetTransform(D3DTS_WORLD, &mat);
	g_pD3DDevice->SetFVF(D3DFVF_XYZ);
	g_pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_vecVertex.size() / 3,
		&m_vecVertex[0], sizeof(D3DXVECTOR3));

	g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, true);
}
