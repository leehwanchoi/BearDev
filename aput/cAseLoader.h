#pragma once
#include "cFrame.h"

class cMtlTex;

class cAseLoader : public cObject
{
private:
	map<string, cFrame*>	m_mapFrame;
	vector<cMtlTex*>		m_vecMtlTex;
	cFrame*					m_pRoot;

	FILE*					m_fp;
	char					m_szToken[1024];

public:
	cAseLoader();
	~cAseLoader();

	char*	GetToken();
	int		GetInteger();
	float	GetFloat();
	bool	IsWhiteSpace(char c);
	bool	IsEqual(char* str1, char* str2);

	cFrame*	Load(IN char* szFullPath, OUT ST_ASE_SCENE* pScene);

	void	ProcessMATERIAL_LIST();
	void	ProcessMATERIAL(OUT cMtlTex* pMtlTex);
	void	ProcessMAP_DIFFUSE(OUT cMtlTex* pMtlTex);

	cFrame*	ProcessGEOMOBJECT();
	void	ProcessMESH(OUT cFrame* pFrame);
	void	ProcessMESH_VERTEX_LIST(OUT vector<D3DXVECTOR3>& vecV);
	void	ProcessMESH_FACE_LIST(IN vector<D3DXVECTOR3>& vecV, OUT vector<ST_PNT_VERTEX>& vecPNTVertex);
	void	ProcessMESH_TVERTLIST(OUT vector<D3DXVECTOR2>& vecTV);
	void	ProcessMESH_TFACELIST(IN vector<D3DXVECTOR2>& vecTV, OUT vector<ST_PNT_VERTEX>& vecPNTVertex);
	void	ProcessMESH_NORMALS(OUT vector<ST_PNT_VERTEX>& vecPNTVertex);

	void	ProcessNODE_TM(OUT cFrame* pFrame);
	void	ProcessSCENE(OUT ST_ASE_SCENE* pScene);
	void	ProcessTM_ANIMATION(OUT cFrame* pFrame);
	void	ProcessCONTROL_POS_TRACK(OUT cFrame* pFrame);
	void	ProcessCONTROL_ROT_TRACK(OUT cFrame* pFrame);
};

