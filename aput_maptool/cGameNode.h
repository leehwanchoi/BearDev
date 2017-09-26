#pragma once
class cGameNode
{
public:
	cGameNode();
	virtual ~cGameNode();

	virtual HRESULT Init();
	virtual void Release();
	virtual void Update();
	virtual void Render();
	virtual void WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

