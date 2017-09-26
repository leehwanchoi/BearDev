#pragma once

class cMtlTex;
class cGroup;

class cObjLoader : public cObject
{
private:
	map<string, cMtlTex*> m_mapMtlTex;

private:
	void LoadMtlLib(char* szFullPath);

public:
	cObjLoader();
	~cObjLoader();

	void Load(IN char* szFilePath, IN D3DXMATRIXA16* pMat,
		OUT vector<cGroup*>& vecGroup);
	LPD3DXMESH LoadMesh(IN char* szFilePath, IN D3DXMATRIXA16* pMat,
		OUT vector<cMtlTex*>& vecMtlTex);
};

