#pragma once

#define g_pFontManager2 cChildWindowFontManager::GetInstance()

class cChildWindowFontManager
{
	SINGLETON(cChildWindowFontManager);

public:
	enum eFontType { E_childNORMAL, E_childCHAT, E_childQUEST, E_childDEBUG };

private:
	map<eFontType, LPD3DXFONT> m_mapFont;

public:
	LPD3DXFONT GetFont(eFontType e);
	void Destroy();
};

