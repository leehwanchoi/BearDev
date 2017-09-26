#include "StdAfx.h"
#include "cSkinnedMeshManager.h"


cSkinnedMeshManager::cSkinnedMeshManager(void)
{
}


cSkinnedMeshManager::~cSkinnedMeshManager(void)
{
}

cSkinnedMesh* cSkinnedMeshManager::GetSkinnedMesh( char* szFolder, char* szFile )
{
	if (m_mapSkinnedMesh.find(szFile) == m_mapSkinnedMesh.end())
	{
		cSkinnedMesh* pSkinnedMesh = new cSkinnedMesh();
		pSkinnedMesh->Load(szFolder, szFile);
		m_mapSkinnedMesh[szFile] = pSkinnedMesh;
	}

	return m_mapSkinnedMesh[szFile];
}

void cSkinnedMeshManager::Destroy()
{
	for each(auto p in m_mapSkinnedMesh)
	{
		p.second->Destroy();
		SAFE_RELEASE(p.second);
	}
}
