#include "stdafx.h"
#include "mainGame.h"


mainGame::mainGame()
{
}


mainGame::~mainGame()
{
}


HRESULT mainGame::init()
{
	_clashPlayer = new clashPlayer;
	_clashPlayer->init();

	_clashEnemy = new clashEnemy;
	_clashEnemy->init();


	_mapInGame = new mapInGame;
	_mapInGame->init();

	//링크
	_clashPlayer->linkClashPlayerEnemyClass(_clashEnemy);
	_clashEnemy->linkClashPlayerEnemyClass(_clashPlayer);

	_clashPlayer->linkMapClass(_mapInGame);
	_clashEnemy->linkMapClass(_mapInGame);

	//배경음악 재생
	SOUNDMANAGER->play("MainGameMusic", 0.5f);

	//이미지 초기화
	IMAGEMANAGER->addImage("Win", "image/Win.bmp", WINSIZEX, WINSIZEY, false, false);
	IMAGEMANAGER->addImage("Lose", "image/Lose.bmp", WINSIZEX, WINSIZEY, false, false);



	//캐릭터 카드 배치
	//for (int i = 0; i < 5; i++)
	//{
	//	_characterCard[i] = RectMakeCenter(300 + i * 100, 600, 50, 50);
	//}

	//_selectedCharacterClass = CC_NONESELECTED;


	//성 배치
	//_playerCastle[0] = RectMake(260, 70, 120, 120);
	//_enemyCastle[0] = RectMake(WINSIZEX - 400, 70, 120, 120);

	//_playerCastle[1] = RectMake(260, 260, 120, 120);
	//_enemyCastle[1] = RectMake(WINSIZEX - 400, 260, 120, 120);

	//_playerCastle[2] = RectMake(100, 165, 120, 120);
	//_enemyCastle[2] = RectMake(WINSIZEX - 240, 165, 120, 120);



	RECT rc = _mapInGame->getPlayerCastleRect(0);
	_clashPlayer->addStruct(rc, TRUE, FALSE, 0);
	rc = _mapInGame->getEnemyCastleRect(0);
	_clashEnemy->addStruct(rc, FALSE, FALSE, 0);

	rc = _mapInGame->getPlayerCastleRect(1);
	_clashPlayer->addStruct(rc, TRUE, FALSE, 1);
	rc = _mapInGame->getEnemyCastleRect(1);
	_clashEnemy->addStruct(rc, FALSE, FALSE, 1);

	rc = _mapInGame->getPlayerCastleRect(2);
	_clashPlayer->addStruct(rc, TRUE, TRUE, 2);
	rc = _mapInGame->getEnemyCastleRect(2);
	_clashEnemy->addStruct(rc, FALSE, TRUE, 2);
	//for (int i = 0; i < MAX_CASTLE_NUM; i++)
	//{
	//	RECT rc = _mapInGame->getPlayerCastleRect(i);
	//	_clashPlayer->addStruct(rc, TRUE, i = 1 ? TRUE : FALSE);
	//	rc = _mapInGame->getEnemyCastleRect(i);
	//	_clashEnemy->addStruct(rc, TRUE, i = 1 ? TRUE : FALSE);

	//}
	return S_OK;
}
void mainGame::release()
{
	_clashPlayer->release();
	SAFE_DELETE(_clashPlayer);
	_clashEnemy->release();
	SAFE_DELETE(_clashEnemy);
	_mapInGame->release();
	SAFE_DELETE(_mapInGame);
}
void mainGame::update()
{
	//_clashPlayer->destroyUnitnCastle();
	//_clashEnemy->destroyUnitnCastle();

	//버그로 인해 임시방편
	if (_mapInGame->get_playercoor(0) == 0 && _mapInGame->get_playercoor(1) == 0)
	{
		
	}
	if (_mapInGame->get_enemycoor(0) == 0 && _mapInGame->get_enemycoor(1) == 0)
	{
		
	}
	else
	{
		_clashPlayer->update();
		_clashEnemy->update();
		_mapInGame->update();
	}

	//if (KEYMANAGER->isOnceKeyDown('1'))
	//{
	//	if (_isCardSelected == TRUE)
	//	{
	//		switch (_selectedCharacterClass)
	//		{
	//		case CC_NONESELECTED:
	//			break;
	//		case CC_ASSASSIN:
	//			_clashPlayer->addUnit(CC_ASSASSIN, _ptMouse.x, _ptMouse.y, TRUE);
	//			break;
	//		case CC_ARCHER:
	//			_clashPlayer->addUnit(CC_ARCHER, _ptMouse.x, _ptMouse.y, TRUE);
	//			break;
	//		case CC_MAGICIAN:
	//			_clashPlayer->addUnit(CC_MAGICIAN, _ptMouse.x, _ptMouse.y, TRUE);
	//			break;
	//		case CC_DEFENCER:
	//			break;;
	//		case CC_GOLEN:
	//			break;
	//		case CC_PUMPKIN:
	//			_clashPlayer->addUnit(CC_PUMPKIN, _ptMouse.x, _ptMouse.y, TRUE);
	//			break;
	//		}
	//		_isCardSelected = FALSE;
	//	}


	//	if (PtInRect(&_characterCard[0], _ptMouse))
	//	{
	//		_selectedCharacterClass = CC_ASSASSIN;
	//		_isCardSelected = TRUE;
	//		//return;
	//	}
	//	else if (PtInRect(&_characterCard[1], _ptMouse))
	//	{
	//		_selectedCharacterClass = CC_ARCHER;
	//		_isCardSelected = TRUE;
	//		//return;
	//	}
	//	else if (PtInRect(&_characterCard[2], _ptMouse))
	//	{
	//		_selectedCharacterClass = CC_MAGICIAN;
	//		_isCardSelected = TRUE;
	//		//return;
	//	}
	//	else if (PtInRect(&_characterCard[3], _ptMouse))
	//	{
	//		_selectedCharacterClass = CC_PUMPKIN;
	//		_isCardSelected = TRUE;
	//		//return;
	//	}
	//	else if (PtInRect(&_characterCard[4], _ptMouse))
	//	{
	//		_selectedCharacterClass = CC_GOLEN;
	//		_isCardSelected = TRUE;
	//		//return;
	//	}
	//	//디펜서 없애고 렉드 없앴음 나중에 하려면 만들어줘야함
	//	
	//}



	//if (KEYMANAGER->isOnceKeyDown('2'))
	//{
	//	if (_isCardSelected == TRUE)
	//	{
	//		switch (_selectedCharacterClass)
	//		{
	//		case CC_NONESELECTED:
	//			break;
	//		case CC_ASSASSIN:
	//			_clashEnemy->addUnit(CC_ASSASSIN, _ptMouse.x, _ptMouse.y, FALSE);
	//			break;
	//		case CC_ARCHER:
	//			_clashEnemy->addUnit(CC_ARCHER, _ptMouse.x, _ptMouse.y, FALSE);
	//			break;
	//		case CC_MAGICIAN:
	//			_clashEnemy->addUnit(CC_MAGICIAN, _ptMouse.x, _ptMouse.y, FALSE);
	//			break;
	//		case CC_DEFENCER:
	//			break;;
	//		case CC_GOLEN:
	//			break;
	//		case CC_PUMPKIN:
	//			_clashEnemy->addUnit(CC_PUMPKIN, _ptMouse.x, _ptMouse.y, FALSE);
	//			break;
	//		}
	//		_isCardSelected = FALSE;

	//	}


	//	if (PtInRect(&_characterCard[0], _ptMouse))
	//	{
	//		_selectedCharacterClass = CC_ASSASSIN;
	//		_isCardSelected = TRUE;
	//		//return;
	//	}
	//	else if (PtInRect(&_characterCard[1], _ptMouse))
	//	{
	//		_selectedCharacterClass = CC_ARCHER;
	//		_isCardSelected = TRUE;
	//		//return;
	//	}
	//	else if (PtInRect(&_characterCard[2], _ptMouse))
	//	{
	//		_selectedCharacterClass = CC_MAGICIAN;
	//		_isCardSelected = TRUE;
	//		//return;
	//	}
	//	else if (PtInRect(&_characterCard[3], _ptMouse))
	//	{
	//		_selectedCharacterClass = CC_PUMPKIN;
	//		_isCardSelected = TRUE;
	//		//return;
	//	}
	//	else if (PtInRect(&_characterCard[4], _ptMouse))
	//	{
	//		_selectedCharacterClass = CC_GOLEN;
	//		_isCardSelected = TRUE;
	//		//return;
	//	}


	//}
}
void mainGame::render()
{
	//버그로 인해 임시방편
	if (_mapInGame->get_playercoor(0) == 0 && _mapInGame->get_playercoor(1) == 0)
	{
		IMAGEMANAGER->render("Lose", getMemDC());
	}
	if (_mapInGame->get_enemycoor(0) == 0 && _mapInGame->get_enemycoor(1) == 0)
	{
		IMAGEMANAGER->render("Win", getMemDC());
	}
	else
	{
		_mapInGame->render();
		_clashPlayer->render();
		_clashEnemy->render();
	}

	//캐릭터 카드 렉트 출력
	//for (int i = 0; i < 5; i++)
	//{
	//	Rectangle(getMemDC(), _characterCard[i].left, _characterCard[i].top, _characterCard[i].right, _characterCard[i].bottom);
	//}

	
	//성 랙트 출력
	//for (int i = 2; i < 3; i++)	//TEST용으로 최종건물만 출력되게 해놨음
	//{
	//	Rectangle(getMemDC(), _playerCastle[i].left, _playerCastle[i].top, _playerCastle[i].right, _playerCastle[i].bottom);
	//	Rectangle(getMemDC(), _enemyCastle[i].left, _enemyCastle[i].top, _enemyCastle[i].right, _enemyCastle[i].bottom);
	//
	//}
}