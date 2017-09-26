#pragma once

#define g_pDeltaTimeManager cDeltaTimeManager::GetInstance()

class cDeltaTimeManager
{
	SINGLETON(cDeltaTimeManager);

private:
	DWORD	m_dwLastUpdatedTime;
	DWORD	m_dwDeltaTime;

public:
	void Update();
	float GetDeltaTime();
};

