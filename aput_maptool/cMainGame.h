#pragma once
#include "cGameNode.h"
#include "cMainScene.h"
#include "cMapTool.h"

class cMainGame : public cGameNode
{
private:

public:
	cMainGame();
	virtual ~cMainGame();

	virtual HRESULT Init() override;
	virtual void Update() override;
	virtual void Render() override;
	virtual void Release() override;
	virtual void WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
};