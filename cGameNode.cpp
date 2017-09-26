#pragma once
class cGameNode
{
public:
	cGameNode();
	virtual ~cGameNode();

	virtual HRESULT init();
	virtual void Release();
	virtual void update();
	virtual void render();
	virtual void WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

