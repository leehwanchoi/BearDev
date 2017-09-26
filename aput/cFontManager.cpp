#include "stdafx.h"
#include "cFontManager.h"


cFontManager::cFontManager()
{
}


cFontManager::~cFontManager()
{
}

LPD3DXFONT cFontManager::GetFont(eFontType e)
{
	if (m_mapFont.find(e) == m_mapFont.end())
	{
		if (e == E_NORMAL)
		{

		}
		else if (e == E_CHAT)
		{

		}
		else if (e == E_QUEST)
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
			strcpy_s(fd.FaceName, "Arial");

			D3DXCreateFontIndirect(g_pD3DDevice, &fd, &m_mapFont[e]);
		}
		else if (e == E_DEBUG)
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
			strcpy_s(fd.FaceName, "Arial");

			D3DXCreateFontIndirect(g_pD3DDevice, &fd, &m_mapFont[e]);
		}
		else if (e == E_INGAMETEXT)
		{
			D3DXFONT_DESC fd;
			ZeroMemory(&fd, sizeof(D3DXFONT_DESC));

			fd.Height = 70;
			fd.Width = 70;
			fd.Weight = FW_NORMAL;
			fd.Italic = false;
			fd.CharSet = DEFAULT_CHARSET;
			fd.OutputPrecision = OUT_DEFAULT_PRECIS;
			fd.PitchAndFamily = FF_DONTCARE;
			strcpy_s(fd.FaceName, "Arial");

			D3DXCreateFontIndirect(g_pD3DDevice, &fd, &m_mapFont[e]);
		}
	}

	return m_mapFont[e];
}

void cFontManager::Destroy()
{
	for each (auto p in m_mapFont)
		SAFE_RELEASE(p.second);

	//RemoveFontResource("umberto.ttf");
}
