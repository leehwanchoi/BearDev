#include "stdafx.h"
#include "cObjLoader.h"
#include "cGroup.h"

cObjLoader::cObjLoader()
{
}


cObjLoader::~cObjLoader()
{
}

void cObjLoader::Load(IN char* szFilePath, IN D3DXMATRIXA16* pMat, OUT vector<cGroup*>& vecGroup)
{
	vector<D3DXVECTOR3> vecP;
	vector<D3DXVECTOR2> vecT;
	vector<D3DXVECTOR3> vecN;
	vector<ST_PNT_VERTEX> vecPNTVertex;

	string sMtlName;

	FILE* fp = NULL;
	fopen_s(&fp, szFilePath, "r");

	while (true)
	{
		if (feof(fp)) break;

		char szTemp[1024];
		fgets(szTemp, 1024, fp);

		if (szTemp[0] == '#' || szTemp[0] == 's' || szTemp[0] == '\n')
		{
			continue;
		}
		else if (szTemp[0] == 'm')
		{
			char szMtlPath[1024];
			char szFullPath[1024];
			sscanf(szTemp, "%*s ./%s", szMtlPath);
			sprintf(szFullPath, "obj/%s", szMtlPath);
			LoadMtlLib(szFullPath);
		}
		else if (szTemp[0] == 'g')
		{
			if (vecPNTVertex.empty())
				continue;

			cGroup* pGroup = new cGroup;
			pGroup->AutoRelease();
			pGroup->SetVertex(vecPNTVertex);
			pGroup->SetMtlTex(m_mapMtlTex[sMtlName]);
			vecGroup.push_back(pGroup);
			vecPNTVertex.clear();
		}
		else if (szTemp[0] == 'v')
		{
			if (szTemp[1] == 't') // vt
			{
				float u, v;
				sscanf(szTemp, "%*s %f %f", &u, &v);
				vecT.push_back(D3DXVECTOR2(u, v));
			}
			else if (szTemp[1] == 'n') // vn
			{
				float x, y, z;
				sscanf(szTemp, "%*s %f %f %f", &x, &z, &y);
				vecN.push_back(D3DXVECTOR3(x, y, -z));
			}
			else // v
			{
				float x, y, z;
				sscanf(szTemp, "%*s %f %f %f", &x, &z, &y);
				vecP.push_back(D3DXVECTOR3(x, y, -z));
			}
		}
		else if (szTemp[0] == 'u')
		{
			char szMtlName[1024];
			sscanf(szTemp, "%*s %s", szMtlName);
			sMtlName = szMtlName;
		}
		else if (szTemp[0] == 'f')
		{
			int aIndex[3][3];
			sscanf(szTemp, "%*s %d/%d/%d %d/%d/%d %d/%d/%d",
				&aIndex[0][0], &aIndex[0][1], &aIndex[0][2],
				&aIndex[1][0], &aIndex[1][1], &aIndex[1][2],
				&aIndex[2][0], &aIndex[2][1], &aIndex[2][2]);

			for (int i = 0; i < 3; ++i)
			{
				ST_PNT_VERTEX pnt;
				pnt.p = vecP[aIndex[i][0] - 1];
				pnt.t = vecT[aIndex[i][1] - 1];
				pnt.n = vecN[aIndex[i][2] - 1];

				if (pMat)
				{
					D3DXVec3TransformCoord(&pnt.p, &pnt.p, pMat);
					D3DXVec3TransformNormal(&pnt.n, &pnt.n, pMat);
				}
				vecPNTVertex.push_back(pnt);
			}
		}
	}

	fclose(fp);

	for each (auto p in m_mapMtlTex)
	{
		SAFE_RELEASE(p.second);
	}
	m_mapMtlTex.clear();
}

LPD3DXMESH cObjLoader::LoadMesh(IN char* szFilePath, IN D3DXMATRIXA16* pMat,
	OUT vector<cMtlTex*>& vecMtlTex)
{
	vector<D3DXVECTOR3> vecP;
	vector<D3DXVECTOR2> vecT;
	vector<D3DXVECTOR3> vecN;
	vector<ST_PNT_VERTEX> vecPNTVertex;
	vector<DWORD> vecAttbuf;

	string sMtlName;

	FILE* fp = NULL;
	fopen_s(&fp, szFilePath, "r");

	while (true)
	{
		if (feof(fp)) break;

		char szTemp[1024];
		fgets(szTemp, 1024, fp);

		if (szTemp[0] == '#' || szTemp[0] == 's' || szTemp[0] == '\n')
		{
			continue;
		}
		else if (szTemp[0] == 'm')
		{
			char szMtlPath[1024];
			char szFullPath[1024];
			sscanf(szTemp, "%*s ./%s", szMtlPath);
			sprintf(szFullPath, "obj/%s", szMtlPath);
			LoadMtlLib(szFullPath);

			vecMtlTex.resize(m_mapMtlTex.size());
			for each (auto p in m_mapMtlTex)
				vecMtlTex[p.second->GetMtlRef()] = p.second;
		}
		//else if (szTemp[0] == 'g')
		//{
		//	if (vecPNTVertex.empty())
		//		continue;

		//	cGroup* pGroup = new cGroup;
		//	pGroup->AutoRelease();
		//	pGroup->SetVertex(vecPNTVertex);
		//	pGroup->SetMtlTex(m_mapMtlTex[sMtlName]);
		//	vecGroup.push_back(pGroup);
		//	vecPNTVertex.clear();
		//}
		else if (szTemp[0] == 'v')
		{
			if (szTemp[1] == 't') // vt
			{
				float u, v;
				sscanf(szTemp, "%*s %f %f", &u, &v);
				vecT.push_back(D3DXVECTOR2(u, v));
			}
			else if (szTemp[1] == 'n') // vn
			{
				float x, y, z;
				sscanf(szTemp, "%*s %f %f %f", &x, &z, &y);
				vecN.push_back(D3DXVECTOR3(x, y, -z));
			}
			else // v
			{
				float x, y, z;
				sscanf(szTemp, "%*s %f %f %f", &x, &z, &y);
				vecP.push_back(D3DXVECTOR3(x, y, -z));
			}
		}
		else if (szTemp[0] == 'u')
		{
			char szMtlName[1024];
			sscanf(szTemp, "%*s %s", szMtlName);
			sMtlName = szMtlName;
		}
		else if (szTemp[0] == 'f')
		{
			int aIndex[3][3];
			sscanf(szTemp, "%*s %d/%d/%d %d/%d/%d %d/%d/%d",
				&aIndex[0][0], &aIndex[0][1], &aIndex[0][2],
				&aIndex[1][0], &aIndex[1][1], &aIndex[1][2],
				&aIndex[2][0], &aIndex[2][1], &aIndex[2][2]);

			for (int i = 0; i < 3; ++i)
			{
				ST_PNT_VERTEX pnt;
				pnt.p = vecP[aIndex[i][0] - 1];
				pnt.t = vecT[aIndex[i][1] - 1];
				pnt.n = vecN[aIndex[i][2] - 1];

				if (pMat)
				{
					D3DXVec3TransformCoord(&pnt.p, &pnt.p, pMat);
					D3DXVec3TransformNormal(&pnt.n, &pnt.n, pMat);
				}
				vecPNTVertex.push_back(pnt);
			}

			vecAttbuf.push_back(m_mapMtlTex[sMtlName]->GetMtlRef());
		}
	}

	fclose(fp);

	//for each (auto p in m_mapMtlTex)
	//{
	//	SAFE_RELEASE(p.second);
	//}

	m_mapMtlTex.clear();

	LPD3DXMESH pMesh = NULL;

	// 1. 메쉬 생성
	D3DXCreateMeshFVF(vecPNTVertex.size() / 3, vecPNTVertex.size(), D3DXMESH_MANAGED,
		ST_PNT_VERTEX::FVF, g_pD3DDevice, &pMesh);

	// 2. 버텍스 정보 복사
	ST_PNT_VERTEX* pV = NULL;
	pMesh->LockVertexBuffer(NULL, (LPVOID*)&pV); // void**, LPVOID = void*
	memcpy(pV, &vecPNTVertex[0], vecPNTVertex.size() * sizeof(ST_PNT_VERTEX));
	pMesh->UnlockVertexBuffer();

	// 3. 인덱스 정보 복사
	WORD* pI = NULL;
	pMesh->LockIndexBuffer(NULL, (LPVOID*)&pI);
	for (WORD i = 0; i < vecPNTVertex.size(); ++i)
		pI[i] = i;
	pMesh->UnlockIndexBuffer();

	// 4. 속성 정보 복사
	DWORD* pA = NULL;
	pMesh->LockAttributeBuffer(NULL, &pA);
	memcpy(pA, &vecAttbuf[0], vecAttbuf.size() * sizeof(DWORD));
	pMesh->UnlockAttributeBuffer();

	vector<DWORD> vecAdjBuf(pMesh->GetNumVertices());
	pMesh->GenerateAdjacency(D3DX_16F_EPSILON, &vecAdjBuf[0]);

	pMesh->OptimizeInplace(D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE,
		&vecAdjBuf[0], NULL, NULL, NULL);

	return pMesh;
}

void cObjLoader::LoadMtlLib(char* szFullPath)
{
	string sMtlName;
	DWORD nRef = 0;

	FILE* fp = NULL;
	fopen_s(&fp, szFullPath, "r");

	while (true)
	{
		if (feof(fp)) break;

		char szTemp[1024];
		fgets(szTemp, 1024, fp);

		if (szTemp[0] == 'n')
		{
			char szMtlName[1024];
			sscanf(szTemp, "%*s %s", szMtlName);
			sMtlName = szMtlName;
			SAFE_RELEASE(m_mapMtlTex[sMtlName]);
			m_mapMtlTex[sMtlName] = new cMtlTex;
			m_mapMtlTex[sMtlName]->SetMtlRef(nRef++);
		}
		else if (szTemp[0] == 'K')
		{
			float r, g, b;
			sscanf(szTemp, "%*s %f %f %f", &r, &g, &b);
			D3DMATERIAL9& stMtl = m_mapMtlTex[sMtlName]->GetMtl();

			if (szTemp[1] == 'a')
			{
				stMtl.Ambient.r = r;
				stMtl.Ambient.g = g;
				stMtl.Ambient.b = b;
				stMtl.Ambient.a = 1.0f;
			}
			else if (szTemp[1] == 'd')
			{
				stMtl.Diffuse.r = r;
				stMtl.Diffuse.g = g;
				stMtl.Diffuse.b = b;
				stMtl.Diffuse.a = 1.0f;
			}
			else if (szTemp[1] == 's')
			{
				stMtl.Specular.r = r;
				stMtl.Specular.g = g;
				stMtl.Specular.b = b;
				stMtl.Specular.a = 1.0f;
			}
		}
		else if (szTemp[0] == 'm')
		{
			char szFilePath[1024];
			char szFullPath[1024];
			sscanf(szTemp, "%*s %s", szFilePath);
			sprintf(szFullPath, "obj/%s", szFilePath);

			g_pTextureManager->AddTexture(szFilePath, szFullPath, NULL, NULL);
			m_mapMtlTex[sMtlName]->SetTexture(g_pTextureManager->GetTexture(szFilePath));
		}
	}

	fclose(fp);
}