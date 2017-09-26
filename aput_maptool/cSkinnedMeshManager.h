#pragma once
#include "cSkinnedMesh.h"

#define g_pSkinnedMeshManager cSkinnedMeshManager::GetInstance()

class cSkinnedMeshManager
{
	SINGLETON(cSkinnedMeshManager);

private:
	std::map<std::string, cSkinnedMesh*> m_mapSkinnedMesh;

public:
	cSkinnedMesh* GetSkinnedMesh(char* szFolder, char* szFile);
	void Destroy();
};