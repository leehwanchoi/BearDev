#include "stdafx.h"
#include "cDeviceManager.h"


cDeviceManager::cDeviceManager()
{
	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	D3DCAPS9 stCaps;		// ��ġ ������ ���� ����ü
	int nVertexProcessing;	// ���ؽ��� ó���ϴ� ���

	m_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &stCaps);

	if (stCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		nVertexProcessing = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else
	{
		nVertexProcessing = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

	D3DPRESENT_PARAMETERS stD3DPP;
	ZeroMemory(&stD3DPP, sizeof(D3DPRESENT_PARAMETERS));
	stD3DPP.Windowed = FALSE;	//TRUE FALSE
	stD3DPP.SwapEffect = D3DSWAPEFFECT_DISCARD;
	stD3DPP.BackBufferFormat = D3DFMT_UNKNOWN; //D3DFMT_UNKNOWN D3DFMT_X8R8G8B8		// �ȼ� ���� p79
	stD3DPP.EnableAutoDepthStencil = TRUE;			// ���� ���� �ڵ� ����
	stD3DPP.AutoDepthStencilFormat = D3DFMT_D16;	// ���� ���� p82
	stD3DPP.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE; //���������� ����

	//�߰�
	stD3DPP.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	stD3DPP.BackBufferFormat = D3DFMT_A8R8G8B8;
	stD3DPP.BackBufferWidth = WINSIZEX;
	stD3DPP.BackBufferHeight = WINSIZEY;
	stD3DPP.hDeviceWindow = g_hWnd;

	HRESULT hr = m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		g_hWnd,
		nVertexProcessing,
		&stD3DPP,
		&m_pD3DDevice);
}


cDeviceManager::~cDeviceManager()
{
}

void cDeviceManager::Destroy()
{
	SAFE_RELEASE(m_pD3D);

	if (m_pD3DDevice)
	{
		ULONG ul = m_pD3DDevice->Release();
		assert(ul == 0 && "����̽��� �̿��ؼ� ������ ��ü �� �Ҹ���� ���� ��ü�� ����");
	}
}
