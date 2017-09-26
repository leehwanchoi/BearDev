#include "stdafx.h"
#include "cTextureManager.h"


cTextureManager::cTextureManager()
{
}


cTextureManager::~cTextureManager()
{
}

void cTextureManager::AddTexture(string key, string fileName,
	OUT LPDIRECT3DTEXTURE9* pTexture)
{
	if (!m_mapTexture[key])
	{
		_bstr_t wText(fileName.c_str());

		D3DXCreateTextureFromFile(g_pD3DDevice, wText, &m_mapTexture[key]);
	}

	if (pTexture)
	{
		*pTexture = m_mapTexture[key];
	}
}

void cTextureManager::AddTexture(string key, string fileName,
	OUT LPDIRECT3DTEXTURE9* pTexture, OUT D3DXIMAGE_INFO* pImageInfo)
{
	if (!m_mapTexture[key])
	{
		_bstr_t wText(fileName.c_str());

		D3DXCreateTextureFromFileEx(g_pD3DDevice, wText, D3DX_DEFAULT_NONPOW2,
			D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
			D3DX_FILTER_NONE, D3DX_DEFAULT, 0, &m_mapImageInfo[key], NULL, &m_mapTexture[key]);
	}

	if (pTexture)
	{
		*pTexture = m_mapTexture[key];
	}

	if (pImageInfo)
	{
		*pImageInfo = m_mapImageInfo[key];
	}
}

void cTextureManager::Destroy()
{
	map<string, LPDIRECT3DTEXTURE9>::iterator iter;

	for (iter = m_mapTexture.begin(); iter != m_mapTexture.end(); ++iter)
		SAFE_RELEASE(iter->second);

	m_mapTexture.clear();
}