#pragma once
#include "gameNode.h"
#include "clashPlayer.h"
#include "clashEnemy.h"
#include "mapInGame.h"


#define MAX_CASTLE_NUM 3



class mainGame :
	public gameNode
{
private:
	clashPlayer* _clashPlayer;
	clashEnemy* _clashEnemy;
	mapInGame* _mapInGame;
	

	int _gameTime;	//게임 한판 당 시간

	
	//TEST용 변수
	//CHARACTER_CLASS _selectedCharacterClass;
	//BOOL			_isCardSelected;
	//RECT _characterCard[5];		//캐릭터 카드 랙트
	//RECT _playerCastle[3];		//player 성
	//RECT _enemyCastle[3];		//enemy 성
public:
	mainGame();
	~mainGame();

	virtual HRESULT init();
	virtual void release();
	virtual void update();
	virtual void render();
};

