#include "stdafx.h"
#include "cDeltaTimeManager.h"


cDeltaTimeManager::cDeltaTimeManager()
{
	m_dwLastUpdatedTime = GetTickCount();
}


cDeltaTimeManager::~cDeltaTimeManager()
{
}

void cDeltaTimeManager::Update()
{
	DWORD dwCurrUpdateTime = GetTickCount();
	m_dwDeltaTime = dwCurrUpdateTime - m_dwLastUpdatedTime;
	m_dwLastUpdatedTime = dwCurrUpdateTime;
}

float cDeltaTimeManager::GetDeltaTime()
{
	return m_dwDeltaTime / 1000.0f;
}