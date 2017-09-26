#include "stdafx.h"
#include "cChildWindowFontManager.h"


cChildWindowFontManager::cChildWindowFontManager()
{
}


cChildWindowFontManager::~cChildWindowFontManager()
{
}

LPD3DXFONT cChildWindowFontManager::GetFont(eFontType e)
{
	if (m_mapFont.find(e) == m_mapFont.end())
	{
		if (e == E_childNORMAL)
		{

		}
		else if (e == E_childCHAT)
		{

		}
		else if (e == E_childQUEST)
		{
			D3DXFONT_DESC fd;
			ZeroMemory(&fd, sizeof(D3DXFONT_DESC));

			fd.Height = 24;
			fd.Width = 12;
			fd.Weight = FW_NORMAL;
			fd.Italic = false;
			fd.CharSet = DEFAULT_CHARSET;
			fd.OutputPrecision = OUT_DEFAULT_PRECIS;
			fd.PitchAndFamily = FF_DONTCARE;
			strcpy_s(fd.FaceName, "±Ã¼­Ã¼");

			D3DXCreateFontIndirect(g_pD3DDevice2, &fd, &m_mapFont[e]);
		}
		else if (e == E_childDEBUG)
		{
			D3DXFONT_DESC fd;
			ZeroMemory(&fd, sizeof(D3DXFONT_DESC));

			fd.Height = 20;
			fd.Width = 10;
			fd.Weight = FW_NORMAL;
			fd.Italic = false;
			fd.CharSet = DEFAULT_CHARSET;
			fd.OutputPrecision = OUT_DEFAULT_PRECIS;
			fd.PitchAndFamily = FF_DONTCARE;
			strcpy_s(fd.FaceName, "±Ã¼­Ã¼");

			D3DXCreateFontIndirect(g_pD3DDevice2, &fd, &m_mapFont[e]);
		}
	}

	return m_mapFont[e];
}

void cChildWindowFontManager::Destroy()
{
	for each (auto p in m_mapFont)
		SAFE_RELEASE(p.second);

	//RemoveFontResource("umberto.ttf");
}
