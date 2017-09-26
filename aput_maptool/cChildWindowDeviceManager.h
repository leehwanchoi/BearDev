#pragma once

#define g_pDeviceManager2 cChildWindowDeviceManager::GetInstance()
#define g_pD3DDevice2 cChildWindowDeviceManager::GetInstance()->GetDevice2()

class cChildWindowDeviceManager
{
	SINGLETON(cChildWindowDeviceManager);

private:
	LPDIRECT3D9			m_pD3D2;			// D3D9 �������̽����� �����͸� ��´�.
	LPDIRECT3DDEVICE9	m_pD3DDevice2;	// ��ġ�� ���� D3D9 �������̽��� ����µ� ���ȴ�.

public:
	LPDIRECT3DDEVICE9 GetDevice2() { return m_pD3DDevice2; }
	void Destroy();
};

