#pragma once

#define g_pDeviceManager2 cChildWindowDeviceManager::GetInstance()
#define g_pD3DDevice2 cChildWindowDeviceManager::GetInstance()->GetDevice2()

class cChildWindowDeviceManager
{
	SINGLETON(cChildWindowDeviceManager);

private:
	LPDIRECT3D9			m_pD3D2;			// D3D9 인터페이스로의 포인터를 담는다.
	LPDIRECT3DDEVICE9	m_pD3DDevice2;	// 장치로 부터 D3D9 인터페이스를 만드는데 사용된다.

public:
	LPDIRECT3DDEVICE9 GetDevice2() { return m_pD3DDevice2; }
	void Destroy();
};

