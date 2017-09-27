#pragma once
#include "gameNode.h"
#include <Vfw.h>

class video : public gameNode
{
private:
	HWND hWndVideo;
	RECT _rc;
public:
	video();
	~video();

	bool videoState();
	void release(void);

	void play(char *fileName, int x, int y, int width, int height);
	void stop();
	void pause();
	void resume();
	void replay();
	void setVolume(int volume);

};

