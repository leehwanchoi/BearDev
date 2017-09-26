#pragma once
#include "cGameNode.h"
#include "cMainScene.h"
#include "cVillage.h"
#include "cStage1.h"
#include "cStage2.h"
#include "cStage3.h"
#include "cBossMap.h"

#include "cTitleScene.h"
#include "cCreditScene.h"
#include "cEndingScene.h"

class cMainGame : public cGameNode
{
private:

public:
	cMainGame();
	virtual ~cMainGame();

	virtual HRESULT init() override;
	virtual void update() override;
	virtual void render() override;
	virtual void Release() override;
	virtual void WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
};