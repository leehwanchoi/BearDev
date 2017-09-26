#include "stdafx.h"
#include "cAseLoader.h"
#include "Asciitok.h"
#include "cMtlTex.h"


cAseLoader::cAseLoader()
	: m_fp(NULL)
	, m_pRoot(NULL)
{
}


cAseLoader::~cAseLoader()
{
}

char* cAseLoader::GetToken()
{
	bool isQuote = false;
	int nReadCount = 0;

	while (true)
	{
		unsigned char c = fgetc(m_fp);

		if (feof(m_fp)) break;

		if (!isQuote && IsWhiteSpace(c))
		{
			if (nReadCount == 0)
				continue;
			else
				break;
		}

		if (c == '\"')
		{
			if (isQuote)
				break;

			isQuote = true;
			continue;
		}

		m_szToken[nReadCount++] = c;
	}

	if (nReadCount == 0)
		return NULL;

	m_szToken[nReadCount] = '\0';

	return m_szToken;
}

int cAseLoader::GetInteger()
{
	return atoi(GetToken());
}

float cAseLoader::GetFloat()
{
	return (float)atof(GetToken());
}

bool cAseLoader::IsWhiteSpace(char c)
{
	return c < 33;
}

bool cAseLoader::IsEqual(char* str1, char* str2)
{
	return strcmp(str1, str2) == 0;
}

cFrame* cAseLoader::Load(IN char* szFullPath, OUT ST_ASE_SCENE* pScene)
{
	fopen_s(&m_fp, szFullPath, "r");

	while (true)
	{
		char* szToken = GetToken();
		if (szToken == NULL) break;

		if (IsEqual(szToken, ID_SCENE))
		{
			ProcessSCENE(pScene);
		}
		else if (IsEqual(szToken, ID_MATERIAL_LIST))
		{
			ProcessMATERIAL_LIST();
		}
		else if (IsEqual(szToken, ID_GEOMETRY))
		{
			cFrame* pFrame = ProcessGEOMOBJECT();
			if (m_pRoot == NULL)
			{
				m_pRoot = pFrame;
			}
		}
	}

	fclose(m_fp);

	/*
	모든 NODE_TM 을 각각의 프레임에 대한 로컬TM으로 계산한다. (추가 수정 x)
	*/
	m_pRoot->CalcLocalTM(NULL);

	return m_pRoot;
}

void cAseLoader::ProcessMATERIAL_LIST()
{
	int nLevel = 0;
	do 
	{
		char* szToken = GetToken();

		if (IsEqual(szToken, "{"))
			++nLevel;
		else if (IsEqual(szToken, "}"))
			--nLevel;
		else if (IsEqual(szToken, ID_MATERIAL_COUNT))
		{
			if (!m_vecMtlTex.empty())
			{
				for each (auto p in m_vecMtlTex)
					SAFE_RELEASE(p);
				m_vecMtlTex.clear();
			}
			m_vecMtlTex.resize(GetInteger());
		}
		else if (IsEqual(szToken, ID_MATERIAL))
		{
			int nMtlRef = GetInteger();
			SAFE_RELEASE(m_vecMtlTex[nMtlRef]);
			m_vecMtlTex[nMtlRef] = new cMtlTex;
			m_vecMtlTex[nMtlRef]->AutoRelease();
			ProcessMATERIAL(m_vecMtlTex[nMtlRef]);
		}
	} while (nLevel > 0);
}

void cAseLoader::ProcessMATERIAL(OUT cMtlTex * pMtlTex)
{
	int nLevel = 0;
	do
	{
		char* szToken = GetToken();

		if (IsEqual(szToken, "{"))
			++nLevel;
		else if (IsEqual(szToken, "}"))
			--nLevel;
		else if (IsEqual(szToken, ID_AMBIENT))
		{
			D3DMATERIAL9& stMtl = pMtlTex->GetMtl();
			stMtl.Ambient.r = GetFloat();
			stMtl.Ambient.g = GetFloat();
			stMtl.Ambient.b = GetFloat();
			stMtl.Ambient.a = 1.0f;
		}
		else if (IsEqual(szToken, ID_DIFFUSE))
		{
			D3DMATERIAL9& stMtl = pMtlTex->GetMtl();
			stMtl.Diffuse.r = GetFloat();
			stMtl.Diffuse.g = GetFloat();
			stMtl.Diffuse.b = GetFloat();
			stMtl.Diffuse.a = 1.0f;
		}
		else if (IsEqual(szToken, ID_SPECULAR))
		{
			D3DMATERIAL9& stMtl = pMtlTex->GetMtl();
			stMtl.Specular.r = GetFloat();
			stMtl.Specular.g = GetFloat();
			stMtl.Specular.b = GetFloat();
			stMtl.Specular.a = 1.0f;
		}
		else if (IsEqual(szToken, ID_MAP_DIFFUSE))
		{
			ProcessMAP_DIFFUSE(pMtlTex);
		}
	} while (nLevel > 0);
}

void cAseLoader::ProcessMAP_DIFFUSE(OUT cMtlTex * pMtlTex)
{
	int nLevel = 0;
	do
	{
		char* szToken = GetToken();

		if (IsEqual(szToken, "{"))
			++nLevel;
		else if (IsEqual(szToken, "}"))
			--nLevel;
		else if (IsEqual(szToken, ID_BITMAP))
		{
			char szFullPath[1024];
			sprintf(szFullPath, "%s", GetToken());

			g_pTextureManager->AddTexture(szFullPath, szFullPath, NULL, NULL);
			pMtlTex->SetTexture(g_pTextureManager->GetTexture(szFullPath));
		}
	} while (nLevel > 0);
}

cFrame* cAseLoader::ProcessGEOMOBJECT()
{
	cFrame* pFrame = new cFrame;
	pFrame->AutoRelease();

	int nLevel = 0;
	do
	{
		char* szToken = GetToken();

		if (IsEqual(szToken, "{"))
			++nLevel;
		else if (IsEqual(szToken, "}"))
			--nLevel;
		else if (IsEqual(szToken, ID_NODE_NAME))
		{
			char* szNodeName = GetToken();
			m_mapFrame[szNodeName] = pFrame;
		}
		else if (IsEqual(szToken, ID_NODE_PARENT))
		{
			char* szParentName = GetToken();
			m_mapFrame[szParentName]->AddChild(pFrame);
		}
		else if (IsEqual(szToken, ID_NODE_TM))
		{
			ProcessNODE_TM(pFrame);
		}
		else if (IsEqual(szToken, ID_MESH))
		{
			ProcessMESH(pFrame);
		}
		else if (IsEqual(szToken, ID_TM_ANIMATION))
		{
			ProcessTM_ANIMATION(pFrame);
		}
		else if (IsEqual(szToken, ID_MATERIAL_REF))
		{
			pFrame->SetMtlTex(m_vecMtlTex[GetInteger()]);
		}
	} while (nLevel > 0);

	return pFrame;
}

void cAseLoader::ProcessNODE_TM(OUT cFrame * pFrame)
{
	D3DXMATRIXA16 transMatrix;
	D3DXMatrixIdentity(&transMatrix);

	int nLevel = 0;
	do
	{
		char* szToken = GetToken();

		if (IsEqual(szToken, "{"))
			++nLevel;
		else if (IsEqual(szToken, "}"))
			--nLevel;
		else if (IsEqual(szToken, ID_TM_ROW0))
		{
			transMatrix._11 = GetFloat();
			transMatrix._13 = GetFloat();
			transMatrix._12 = GetFloat();
			transMatrix._14 = 0.0f;
		}
		else if (IsEqual(szToken, ID_TM_ROW1))
		{
			transMatrix._31 = GetFloat();
			transMatrix._33 = GetFloat();
			transMatrix._32 = GetFloat();
			transMatrix._34 = 0.0f;
		}
		else if (IsEqual(szToken, ID_TM_ROW2))
		{
			transMatrix._21 = GetFloat();
			transMatrix._23 = GetFloat();
			transMatrix._22 = GetFloat();
			transMatrix._24 = 0.0f;
		}
		else if (IsEqual(szToken, ID_TM_ROW3))
		{
			transMatrix._41 = GetFloat();
			transMatrix._43 = GetFloat();
			transMatrix._42 = GetFloat();
			transMatrix._44 = 1.0f;
		}
	} while (nLevel > 0);

	pFrame->SetWorldTM(transMatrix);
}

void cAseLoader::ProcessMESH(OUT cFrame* pFrame)
{
	vector<D3DXVECTOR3>		vecV;
	vector<D3DXVECTOR2>		vecTV;
	vector<ST_PNT_VERTEX>	vecPNTVertex;

	int nLevel = 0;
	do
	{
		char* szToken = GetToken();

		if (IsEqual(szToken, "{"))
			++nLevel;
		else if (IsEqual(szToken, "}"))
			--nLevel;
		else if (IsEqual(szToken, ID_MESH_NUMVERTEX))
		{
			vecV.resize(GetInteger());
		}
		else if (IsEqual(szToken, ID_MESH_NUMFACES))
		{
			vecPNTVertex.resize(GetInteger() * 3);
		}
		else if (IsEqual(szToken, ID_MESH_VERTEX_LIST))
		{
			ProcessMESH_VERTEX_LIST(vecV);
		}
		else if (IsEqual(szToken, ID_MESH_FACE_LIST))
		{
			ProcessMESH_FACE_LIST(vecV, vecPNTVertex);
		}
		else if (IsEqual(szToken, ID_MESH_NUMTVERTEX))
		{
			vecTV.resize(GetInteger());
		}
		else if (IsEqual(szToken, ID_MESH_TVERTLIST))
		{
			ProcessMESH_TVERTLIST(vecTV);
		}
		else if (IsEqual(szToken, ID_MESH_TFACELIST))
		{
			ProcessMESH_TFACELIST(vecTV, vecPNTVertex);
		}
		else if (IsEqual(szToken, ID_MESH_NORMALS))
		{
			ProcessMESH_NORMALS(vecPNTVertex);
		}
	} while (nLevel > 0);

	/*
	NODE_TM 에서 얻은 매트릭스의 인벌스 매트릭스로
	모든 정점 정보를 각각의 로컬 영역(애니메이션을 적용 시킬 위치)으로 돌려 보낸다.
	*/
	D3DXMATRIXA16 matInvNodeTM;
	D3DXMatrixInverse(&matInvNodeTM, NULL, &pFrame->GetWorldTM());
	for (int i = 0; i < vecPNTVertex.size(); ++i)
	{
		D3DXVec3TransformCoord(&vecPNTVertex[i].p, &vecPNTVertex[i].p, &matInvNodeTM);
		D3DXVec3TransformNormal(&vecPNTVertex[i].n, &vecPNTVertex[i].n, &matInvNodeTM);
	}

	pFrame->SetVertex(vecPNTVertex);
}

void cAseLoader::ProcessMESH_VERTEX_LIST(OUT vector<D3DXVECTOR3>& vecV)
{
	int nLevel = 0;
	do
	{
		char* szToken = GetToken();

		if (IsEqual(szToken, "{"))
			++nLevel;
		else if (IsEqual(szToken, "}"))
			--nLevel;
		else if (IsEqual(szToken, ID_MESH_VERTEX))
		{
			int nIndex = GetInteger();
			vecV[nIndex].x = GetFloat();
			vecV[nIndex].z = GetFloat();
			vecV[nIndex].y = GetFloat();
		}
	} while (nLevel > 0);
}

void cAseLoader::ProcessMESH_FACE_LIST(IN vector<D3DXVECTOR3>& vecV, OUT vector<ST_PNT_VERTEX>& vecPNTVertex)
{
	int nLevel = 0;
	do
	{
		char* szToken = GetToken();

		if (IsEqual(szToken, "{"))
			++nLevel;
		else if (IsEqual(szToken, "}"))
			--nLevel;
		else if (IsEqual(szToken, ID_MESH_FACE))
		{
			int nFaceIndex = GetInteger();

			GetToken(); // A:
			int nA = GetInteger();

			GetToken(); // B:
			int nB = GetInteger();

			GetToken(); // C:
			int nC = GetInteger();

			vecPNTVertex[nFaceIndex * 3 + 0].p = vecV[nA];
			vecPNTVertex[nFaceIndex * 3 + 1].p = vecV[nC];
			vecPNTVertex[nFaceIndex * 3 + 2].p = vecV[nB];
		}
	} while (nLevel > 0);
}

void cAseLoader::ProcessMESH_TVERTLIST(OUT vector<D3DXVECTOR2>& vecTV)
{
	int nLevel = 0;
	do
	{
		char* szToken = GetToken();

		if (IsEqual(szToken, "{"))
			++nLevel;
		else if (IsEqual(szToken, "}"))
			--nLevel;
		else if (IsEqual(szToken, ID_MESH_TVERT))
		{
			int nIndex = GetInteger();
			vecTV[nIndex].x = GetFloat();
			vecTV[nIndex].y = 1.0f - GetFloat();
		}
	} while (nLevel > 0);
}

void cAseLoader::ProcessMESH_TFACELIST(IN vector<D3DXVECTOR2>& vecTV, OUT vector<ST_PNT_VERTEX>& vecPNTVertex)
{
	int nLevel = 0;
	do
	{
		char* szToken = GetToken();

		if (IsEqual(szToken, "{"))
			++nLevel;
		else if (IsEqual(szToken, "}"))
			--nLevel;
		else if (IsEqual(szToken, ID_MESH_TFACE))
		{
			int nFaceIndex = GetInteger();
			int nA = GetInteger();
			int nB = GetInteger();
			int nC = GetInteger();

			vecPNTVertex[nFaceIndex * 3 + 0].t = vecTV[nA];
			vecPNTVertex[nFaceIndex * 3 + 1].t = vecTV[nC];
			vecPNTVertex[nFaceIndex * 3 + 2].t = vecTV[nB];
		}
	} while (nLevel > 0);
}

void cAseLoader::ProcessMESH_NORMALS(OUT vector<ST_PNT_VERTEX>& vecPNTVertex)
{
	int nFaceIndex = 0;
	int nFaceCount = 0;
	int aModCount[] = { 0, 2, 1 };

	int nLevel = 0;
	do
	{
		char* szToken = GetToken();

		if (IsEqual(szToken, "{"))
			++nLevel;
		else if (IsEqual(szToken, "}"))
			--nLevel;
		else if (IsEqual(szToken, ID_MESH_FACENORMAL))
		{
			nFaceIndex = GetInteger();
			nFaceCount = 0;
		}
		else if (IsEqual(szToken, ID_MESH_VERTEXNORMAL))
		{
			GetToken();
			vecPNTVertex[nFaceIndex * 3 + aModCount[nFaceCount]].n.x = GetFloat();
			vecPNTVertex[nFaceIndex * 3 + aModCount[nFaceCount]].n.z = GetFloat();
			vecPNTVertex[nFaceIndex * 3 + aModCount[nFaceCount]].n.y = GetFloat();
			++nFaceCount;
		}
	} while (nLevel > 0);
}

void cAseLoader::ProcessSCENE(OUT ST_ASE_SCENE * pScene)
{
	int nLevel = 0;
	do
	{
		char* szToken = GetToken();

		if (IsEqual(szToken, "{"))
			++nLevel;
		else if (IsEqual(szToken, "}"))
			--nLevel;
		else if (IsEqual(szToken, ID_FIRSTFRAME))
		{
			pScene->nFirstFrame = GetInteger();
		}
		else if (IsEqual(szToken, ID_LASTFRAME))
		{
			pScene->nLastFrame = GetInteger();
		}
		else if (IsEqual(szToken, ID_FRAMESPEED))
		{
			pScene->nFrameSpeed = GetInteger();
		}
		else if (IsEqual(szToken, ID_TICKSPERFRAME))
		{
			pScene->nTicksPerFrame = GetInteger();
		}
	} while (nLevel > 0);
}

void cAseLoader::ProcessTM_ANIMATION(OUT cFrame * pFrame)
{
	int nLevel = 0;
	do
	{
		char* szToken = GetToken();

		if (IsEqual(szToken, "{"))
			++nLevel;
		else if (IsEqual(szToken, "}"))
			--nLevel;
		else if (IsEqual(szToken, ID_POS_TRACK))
		{
			ProcessCONTROL_POS_TRACK(pFrame);
		}
		else if (IsEqual(szToken, ID_ROT_TRACK))
		{
			ProcessCONTROL_ROT_TRACK(pFrame);
		}
	} while (nLevel > 0);
}

void cAseLoader::ProcessCONTROL_POS_TRACK(OUT cFrame * pFrame)
{
	int nLevel = 0;
	do
	{
		char* szToken = GetToken();

		if (IsEqual(szToken, "{"))
			++nLevel;
		else if (IsEqual(szToken, "}"))
			--nLevel;
		else if (IsEqual(szToken, ID_POS_SAMPLE))
		{
			ST_POS_SAMPLE stPosSample;
			stPosSample.n = GetInteger();
			stPosSample.v.x = GetFloat();
			stPosSample.v.z = GetFloat();
			stPosSample.v.y = GetFloat();
			pFrame->m_vecPosTrack.push_back(stPosSample);
		}
	} while (nLevel > 0);
}

/*
키 값은 그대로 저장

값을 퀀터니온으로 변형하여 저장한다.
stRotSample.q.x = stRotSample.q.x * sinf(stRotSample.q.w / 2.0f)
stRotSample.q.y = stRotSample.q.y * sinf(stRotSample.q.w / 2.0f)
stRotSample.q.z = stRotSample.q.z * sinf(stRotSample.q.w / 2.0f)
stRotSample.q.w = cosf(stRotSample.q.w / 2.0f)

회전 값은 누적 연산 처리 된다.
if (!pFrame->m_vecRotTrack.empty())
{
	stRotSample.q = pFrame->m_vecRotTrack.back().q * stRotSample.q;
}

pFrame->m_vecRotTrack.push_back(stRotSample);
*/
void cAseLoader::ProcessCONTROL_ROT_TRACK(OUT cFrame * pFrame)
{
	int nLevel = 0;
	do
	{
		char* szToken = GetToken();

		if (IsEqual(szToken, "{"))
			++nLevel;
		else if (IsEqual(szToken, "}"))
			--nLevel;
		else if (IsEqual(szToken, ID_ROT_SAMPLE))
		{
			ST_ROT_SAMPLE stRotSample;
			stRotSample.n = GetInteger();
			stRotSample.q.x = GetFloat();
			stRotSample.q.z = GetFloat();
			stRotSample.q.y = GetFloat();
			stRotSample.q.w = GetFloat();

			stRotSample.q.x = stRotSample.q.x * sinf(stRotSample.q.w / 2.0f);
			stRotSample.q.y = stRotSample.q.y * sinf(stRotSample.q.w / 2.0f);
			stRotSample.q.z = stRotSample.q.z * sinf(stRotSample.q.w / 2.0f);
			stRotSample.q.w = cosf(stRotSample.q.w / 2.0f);

			if (!pFrame->m_vecRotTrack.empty())
			{
				stRotSample.q = pFrame->m_vecRotTrack.back().q * stRotSample.q;
			}

			pFrame->m_vecRotTrack.push_back(stRotSample);
		}
	} while (nLevel > 0);
}
