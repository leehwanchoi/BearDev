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
	

	int _gameTime;	//���� ���� �� �ð�

	
	//TEST�� ����
	//CHARACTER_CLASS _selectedCharacterClass;
	//BOOL			_isCardSelected;
	//RECT _characterCard[5];		//ĳ���� ī�� ��Ʈ
	//RECT _playerCastle[3];		//player ��
	//RECT _enemyCastle[3];		//enemy ��
public:
	mainGame();
	~mainGame();

	virtual HRESULT init();
	virtual void release();
	virtual void update();
	virtual void render();
};

