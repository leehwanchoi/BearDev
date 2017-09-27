#pragma once

#include "gameNode.h"

class GameSound : public gameNode
{
private:

public:
	GameSound();
	~GameSound();

	HRESULT init();
	void release();
	void update();
	void render();

	void BackGroundSound();
	void MeleeHitSound();
	void RangeHitSound();
	void BuildHitSound();
	void ClickSound();
	void DestroyedSound();
	void WinSound();
	void LoseSound();
};

