#include "stdafx.h"
#include "cObjectManager.h"


cObjectManager::cObjectManager()
{
}


cObjectManager::~cObjectManager()
{
}

void cObjectManager::AddObject(cObject* pObject)
{
	m_setObject.insert(pObject);
}

void cObjectManager::RemoveObject(cObject* pObject)
{
	m_setObject.erase(pObject);
}

void cObjectManager::Destroy()
{
	assert(m_setObject.empty() && "오브젝트 매니져: 커스텀 객체 중 삭제 되지 않은 객체가 있음.");
}