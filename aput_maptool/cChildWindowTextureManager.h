#pragma once

#define g_pTextureManager2 cChildWindowTextureManager::GetInstance()

class cChildWindowTextureManager
{
	SINGLETON(cChildWindowTextureManager);

private:
	map<string, LPDIRECT3DTEXTURE9> m_mapTexture;
	map<string, D3DXIMAGE_INFO>		m_mapImageInfo;

public:
	void AddTexture(string key, string fileName, OUT LPDIRECT3DTEXTURE9* pTexture);
	void AddTexture(string key, string fileName, OUT LPDIRECT3DTEXTURE9* pTexture, OUT D3DXIMAGE_INFO* pImageInfo);
	void Destroy();

	LPDIRECT3DTEXTURE9& GetTexture(string key) { return m_mapTexture[key]; }
};

