#include "stdafx.h"
#include "video.h"



video::video()
	:hWndVideo(NULL)
{
}


video::~video()
{
}
//                   파일이름     시작x, 시작y, 가로크기, 세로크기 (렉트랑같음)
void video::play(char *fileName, int x, int y, int width, int height)
{
	if (hWndVideo)
	{
		MCIWndClose(hWndVideo);
		MCIWndDestroy(hWndVideo);
		hWndVideo = NULL;
	}

	hWndVideo = MCIWndCreate(_hWnd, _hInstance,
		MCIWNDF_NOTIFYANSI | MCIWNDF_NOMENU |
		MCIWNDF_NOTIFYALL | MCIWNDF_NOPLAYBAR, fileName);

	if (hWndVideo)
	{
		SetWindowPos(hWndVideo, NULL, x, y, width, height, SWP_NOZORDER | SWP_NOMOVE);
		MCIWndPlay(hWndVideo);
		MCIWndSetVolume(hWndVideo, 0);
	}
}

void video::stop()
{
	if (!hWndVideo) return;

	if (hWndVideo)
	{
		MCIWndPlay(hWndVideo);
	}
}
void video::pause()
{
	if (!hWndVideo) return;

	if (hWndVideo)
	{
		MCIWndPause(hWndVideo);
	}
}
void video::resume()
{
	if (!hWndVideo) return;

	if (hWndVideo)
	{
		MCIWndResume(hWndVideo);
	}
}
bool video::videoState()
{
	char a[10];
	switch (MCIWndGetMode(hWndVideo, a, sizeof(a)))
	{
	case MCI_MODE_STOP:
		return false;
	case MCI_MODE_PLAY:
		return true;
	}
}

void video::release(void)
{
	MCIWndClose(hWndVideo);
	MCIWndDestroy(hWndVideo);
	hWndVideo = NULL;
}

void video::replay(void)
{
	if (!videoState())
		MCIWndPlay(hWndVideo);
}

void video::setVolume(int volume)
{
	MCIWndSetVolume(hWndVideo, volume);
}