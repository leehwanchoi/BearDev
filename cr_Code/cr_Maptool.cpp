#include "stdafx.h"
#include "cr_Maptool.h"

cr_Maptool::cr_Maptool(){}
cr_Maptool::~cr_Maptool(){}

HRESULT cr_Maptool::init()
{
	/*gameStudy���� �ʱ�ȭ�ؾ� �ϴ� �� �̹���*/
	//IMAGEMANAGER->addFrameImage("mapTiles", "image/mapTiles.bmp", 0, 0, 1248, 96, SAMPLETILEX, SAMPLETILEY, true, RGB(255, 0, 255));

	/*cr_Maptool������ �ʱ�ȭ�ϴ� �� �̹���*/
	IMAGEMANAGER->addImage("save", "image/save.bmp", 100, 100, true, RGB(255, 0, 255));
	IMAGEMANAGER->addImage("load", "image/load.bmp", 100, 100, true, RGB(255, 0, 255));
	IMAGEMANAGER->addImage("play", "image/play.bmp", 200, 100, true, RGB(255, 0, 255));

	//���� ��ư �̹���
	button_save = RectMake(WINSIZEX - 200, WINSIZEY - 200, 100, 100);
	button_load = RectMake(WINSIZEX - 100, WINSIZEY - 200, 100, 100);
	button_play = RectMake(WINSIZEX - 400, WINSIZEY - 200, 100, 100);

	//���콺 Ÿ�� �ʱ�ȭ
	_currentTile.x = 26;
	_currentTile.y = 2;

	//ĳ�� Ÿ�� �̹��� �Ѹ� ��Ʈ
	castle_rc = RectMake(0, (WINSIZEY - 100) - IMAGEMANAGER->findImage("Castle")->getHeight(), IMAGEMANAGER->findImage("Castle")->getWidth(), IMAGEMANAGER->findImage("Castle")->getHeight());

	//�÷��̾� �� �� �ǹ� ��Ʈ
	for (int i = 0; i < buildsMAX; i++)
	{
		if (i == 0)
		{
			builds_player_rc[i] = RectMake(160, 70, IMAGEMANAGER->findImage("Castle")->getFrameWidth(), IMAGEMANAGER->findImage("Castle")->getFrameHeight());
			builds_enemy_rc[i] = RectMake(WINSIZEX - 300, 70, IMAGEMANAGER->findImage("Castle")->getFrameWidth(), IMAGEMANAGER->findImage("Castle")->getFrameHeight());

			builds_player_coor[i].x = 6;
			builds_player_coor[i].y = 0;

			builds_enemy_coor[i].x = 6;
			builds_enemy_coor[i].y = 0;
		}

		if (i == 1)
		{
			builds_player_rc[i] = RectMake(160, 260, IMAGEMANAGER->findImage("Castle")->getFrameWidth(), IMAGEMANAGER->findImage("Castle")->getFrameHeight());
			builds_enemy_rc[i] = RectMake(WINSIZEX - 300, 260, IMAGEMANAGER->findImage("Castle")->getFrameWidth(), IMAGEMANAGER->findImage("Castle")->getFrameHeight());

			builds_player_coor[i].x = 6;
			builds_player_coor[i].y = 0;

			builds_enemy_coor[i].x = 6;
			builds_enemy_coor[i].y = 0;
		}

		if (i == 2)
		{
			builds_player_rc[i] = RectMake(40, 165, IMAGEMANAGER->findImage("Castle")->getFrameWidth(), IMAGEMANAGER->findImage("Castle")->getFrameHeight());
			builds_enemy_rc[i] = RectMake(WINSIZEX - 170, 165, IMAGEMANAGER->findImage("Castle")->getFrameWidth(), IMAGEMANAGER->findImage("Castle")->getFrameHeight());

			builds_player_coor[i].x = 2;
			builds_player_coor[i].y = 0;

			builds_enemy_coor[i].x = 2;
			builds_enemy_coor[i].y = 0;
		}
	}

	//������Ʈ���� Ȯ���ϴ� �Ұ�
	_isObject = false;

	//�ʱ⼳��
	setUp();

	//���� ���� ���� ��Ʈ ����
	allow_player_unit = RectMake(_tiles[0].rc.left, _tiles[0].rc.top, TILESIZE * TILEX / 2, TILESIZE * TILEY);
	allow_enemy_unit = RectMake(_tiles[TILEX / 2].rc.left, _tiles[TILEX / 2].rc.top, TILESIZE * TILEX / 2, TILESIZE * TILEY);

	//���� ������ �� �ҷ�����
	load();
	build_player_load();
	build_player_load();

	return S_OK;
}

void cr_Maptool::release()
{

}

void cr_Maptool::update()
{
	//�Է°� ó�� (��ư �� Ÿ��&������Ʈ)
	input();

	//�ǹ� ó��
	buildsCollision();

	//Ÿ�ϸ� �׸���
	if (_leftButtonDown && !_isObject) setMap();

	RECT temp;
	if (IntersectRect(&temp,&button_play,&mouse_rc)&&KEYMANAGER->isOnceKeyDown(VK_RBUTTON))
	{
		SCENEMANAGER->changeScene("main Game");
	}

	//���콺 �̹��� �׷��ֱ�
	if (!_isObject)
		mouse_rc = RectMake(_ptMouse.x - (IMAGEMANAGER->findImage("mapTiles")->getFrameWidth() / 2), _ptMouse.y - (IMAGEMANAGER->findImage("mapTiles")->getFrameHeight() / 2), IMAGEMANAGER->findImage("mapTiles")->getFrameWidth(), IMAGEMANAGER->findImage("mapTiles")->getFrameHeight());
	if (_isObject)
		mouse_rc = RectMake(_ptMouse.x - (IMAGEMANAGER->findImage("Castle")->getFrameWidth() / 2), _ptMouse.y - (IMAGEMANAGER->findImage("Castle")->getFrameHeight() / 2), IMAGEMANAGER->findImage("Castle")->getFrameWidth(), IMAGEMANAGER->findImage("Castle")->getFrameHeight());
	if (IntersectRect(&temp,&button_play,&mouse_rc)&&KEYMANAGER->isOnceKeyDown(VK_LBUTTON))
	{
		SCENEMANAGER->changeScene("main Game");
	}
}

void cr_Maptool::render()
{
	//Ÿ�ϸ� �̹��� �׷���
	IMAGEMANAGER->render("mapTiles", getMemDC(), 0, WINSIZEY - IMAGEMANAGER->findImage("mapTiles")->getHeight());

	//������Ʈ(�ǹ�) �̹��� �׷���
	DrawRect(getMemDC(), castle_rc);
	IMAGEMANAGER->render("Castle", getMemDC(), 0, (WINSIZEY - 100) - IMAGEMANAGER->findImage("Castle")->getHeight());

	//save, load �̹���
	IMAGEMANAGER->render("save", getMemDC(), button_save.left, button_save.top);
	IMAGEMANAGER->render("load", getMemDC(), button_load.left, button_load.top);
	IMAGEMANAGER->render("play", getMemDC(), button_play.left, button_play.top);

	//���� �Ѹ���
	for (int i = 0; i < TILEX * TILEY; i++)
	{
		IMAGEMANAGER->frameRender("mapTiles", getMemDC(), _tiles[i].rc.left, _tiles[i].rc.top, _tiles[i].terrainFrameX, _tiles[i].terrainFrameY);
	}

	for (int i = 0; i < TILEX * TILEY; i++)
	{
		if (_tiles[i].obj == OBJ_NONE) continue;
		IMAGEMANAGER->frameRender("mapTiles", getMemDC(), _tiles[i].rc.left,
			_tiles[i].rc.top, _tiles[i].objFrameX, _tiles[i].objFrameY);
	}

	if (!_isObject)
	{
		//���콺�� Ÿ�� �Ѹ���
		if (_currentTile.y == 2 && _currentTile.x == 26);
		else
		{
			//DrawRect(getMemDC(), mouse_rc);
			IMAGEMANAGER->frameRender("mapTiles", getMemDC(), mouse_rc.left, mouse_rc.top, _currentTile.x, _currentTile.y);
		}
	}
	if (_isObject)
	{
		//���콺�� �ǹ� �Ѹ���
		//DrawRect(getMemDC(), mouse_rc);
		IMAGEMANAGER->frameRender("Castle", getMemDC(), mouse_rc.left, mouse_rc.top, _currentObject.x, _currentObject.y);
	}

	char str[128];
	sprintf_s(str, "cr_Maptool Ver 0.0001  ����Ʈ������[2��]");
	TextOut(getMemDC(), WINSIZEX - 400, WINSIZEY - 25, str, strlen(str));

	for (int i = 0; i < buildsMAX; i++)
	{
		//DrawRect(getMemDC(), builds_player_rc[i]);
		//DrawRect(getMemDC(), builds_enemy_rc[i]);

		//�÷��̾� ĳ�� �̹��� �׷���
		IMAGEMANAGER->frameRender("Castle", getMemDC(), builds_player_rc[i].left, builds_player_rc[i].top, builds_player_coor[i].x, builds_player_coor[i].y);

		//�� ĳ�� �̹��� �׷���
		IMAGEMANAGER->frameRender("Castle", getMemDC(), builds_enemy_rc[i].left, builds_enemy_rc[i].top, builds_enemy_coor[i].x, builds_player_coor[i].y);
	}

	if (KEYMANAGER->isStayKeyDown(VK_TAB))
	{
		for (int i = 0; i < TILEX * TILEY; i++)
		{
			DrawRect(getMemDC(), _tiles[i].rc);

			if (_tiles[i].terrain == 2)
				SetTextColor(getMemDC(), RGB(128, 128, 128));
			else if (_tiles[i].terrain == 3)
				SetTextColor(getMemDC(), RGB(0, 0, 255));
			else if (_tiles[i].terrain == 4)
				SetTextColor(getMemDC(), RGB(255, 0, 0));
			else if (_tiles[i].terrain == 1)
				SetTextColor(getMemDC(), RGB(0, 128, 0));
			else
				SetTextColor(getMemDC(), RGB(0, 0, 0));

			char str[128];
			sprintf_s(str, "%d,%d", _tiles[i].terrain, _tiles[i].obj);
			TextOut(getMemDC(), _tiles[i].rc.left, _tiles[i].rc.top, str, strlen(str));
		}
	}

	if (KEYMANAGER->isStayKeyDown(VK_CAPITAL))
	{
		//���� ���� ���� ��ġ �׸���
		DrawRect(getMemDC(), allow_player_unit);
		DrawRect(getMemDC(), allow_enemy_unit);

		char str[128];
		sprintf_s(str, "%d", UnitCreate_Check());
		TextOut(getMemDC(), allow_player_unit.left, allow_player_unit.top, str, strlen(str));
	}
}

void cr_Maptool::setMap()
{
	for (int i = 0; i < SAMPLETILEX * SAMPLETILEY; i++)
	{
		if (PtInRect(&_sampleTiles[i].rcTile, _ptMouse))
		{
			_currentTile.x = _sampleTiles[i].terrainFrameX;
			_currentTile.y = _sampleTiles[i].terrainFrameY;
			break;
		}
	}

	for (int i = 0; i < TILEX * TILEY; i++)
	{
		if (PtInRect(&_tiles[i].rc, _ptMouse))
		{
			//���� ��ư�� ������ üũ�Ǿ� ������
			if (_ctrlSelect == CTRL_TERRAINDRAW)
			{
				_tiles[i].terrainFrameX = _currentTile.x;
				_tiles[i].terrainFrameY = _currentTile.y;

				_tiles[i].terrain = terrainSelect(_currentTile.x, _currentTile.y);
			}

			//���� ��ư�� ������Ʈ�� üũ�Ǿ� ������
			if (_ctrlSelect == CTRL_OBJDRAW)
			{
				_tiles[i].objFrameX = _currentTile.x;
				_tiles[i].objFrameY = _currentTile.y;

				_tiles[i].obj = objSelect(_currentTile.x, _currentTile.y);
			}

			//���� ��ư�� ���찳�� üũ�Ǿ� ������
			if (_ctrlSelect == CTRL_ERASER)
			{
				_tiles[i].objFrameX = 0;
				_tiles[i].objFrameY = 8;

				_tiles[i].obj = OBJ_NONE;
			}

			//InvalidateRect(_hWnd, NULL, false);
			break;
		}
	}
}

void cr_Maptool::setUp()
{
	//�⺻���� ����Ʈ ���¸� �������� �д�
	_ctrlSelect = CTRL_TERRAINDRAW;

	//������ ���� Ÿ�� ��Ʈ ����
	for (int i = 0; i < SAMPLETILEY; i++)
	{
		for (int j = 0; j < SAMPLETILEX; j++)
		{
			_sampleTiles[i * SAMPLETILEX + j].terrainFrameX = j;
			_sampleTiles[i * SAMPLETILEX + j].terrainFrameY = i;

			SetRect(&_sampleTiles[i*SAMPLETILEX + j].rcTile, j * TILESIZE, (WINSIZEY - IMAGEMANAGER->findImage("mapTiles")->getHeight()) + i * TILESIZE,
				j * TILESIZE + TILESIZE, (WINSIZEY - IMAGEMANAGER->findImage("mapTiles")->getHeight()) + i * TILESIZE + TILESIZE);
		}
	}

	//���ʺκ� Ÿ�� ��Ʈ�� ���� ����
	for (int i = 0; i < TILEY; i++)
	{
		for (int j = 0; j < TILEX; j++)
		{
			SetRect(&_tiles[i *TILEX + j].rc,
				j * TILESIZE,
				i * TILESIZE,
				j * TILESIZE + TILESIZE,
				i * TILESIZE + TILESIZE);
		}
	}

	//�⺻������ Ÿ���� ��� �ܵ�� 
	for (int i = 0; i < TILEX * TILEY; i++)
	{
		_tiles[i].terrainFrameX = 1;
		_tiles[i].terrainFrameY = 1;
		_tiles[i].objFrameX = 0;
		_tiles[i].objFrameY = 0;
		_tiles[i].terrain = terrainSelect(_tiles[i].terrainFrameX,
			_tiles[i].terrainFrameY);
		_tiles[i].obj = objSelect(_tiles[i].terrainFrameX,
			_tiles[i].terrainFrameY);
	}
}

void cr_Maptool::save()
{
	//�� Ÿ�� ����
	HANDLE file;
	DWORD write;

	file = CreateFile("tiles.map", GENERIC_WRITE, 0, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	WriteFile(file, _tiles, sizeof(tagTile) * TILEX * TILEY, &write, NULL);
	CloseHandle(file);
}

void cr_Maptool::load()
{
	//�� Ÿ�� �ε�
	HANDLE file;
	DWORD read;

	file = CreateFile("tiles.map", GENERIC_READ, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	ReadFile(file, _tiles, sizeof(tagTile) * TILEX * TILEY, &read, NULL);
	CloseHandle(file);
}

void cr_Maptool::build_player_save()
{
	//�÷��̾� �ǹ� ����
	HANDLE file;
	DWORD write;

	file = CreateFile("player_build.map", GENERIC_WRITE, 0, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	WriteFile(file, builds_player_coor, sizeof(RECT) * buildsMAX, &write, NULL);
	CloseHandle(file);
}

void cr_Maptool::build_player_load()
{
	//�÷��̾� �ǹ� �ε�
	HANDLE file;
	DWORD read;

	file = CreateFile("player_build.map", GENERIC_READ, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	ReadFile(file, builds_player_coor, sizeof(RECT) * buildsMAX, &read, NULL);
	CloseHandle(file);
}

void cr_Maptool::build_enemy_save()
{
	//�� �ǹ� ����
	HANDLE file;
	DWORD write;

	file = CreateFile("build_enemy.map", GENERIC_WRITE, 0, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	WriteFile(file, builds_player_coor, sizeof(RECT) * buildsMAX, &write, NULL);
	CloseHandle(file);
}

void cr_Maptool::build_enemy_load()
{
	//�� �ǹ� �ε�
	HANDLE file;
	DWORD read;

	file = CreateFile("build_enemy.map", GENERIC_READ, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	ReadFile(file, builds_player_coor, sizeof(RECT) * buildsMAX, &read, NULL);
	CloseHandle(file);
}

TERRAIN cr_Maptool::terrainSelect(int frameX, int frameY)
{
	//�� ���� Ÿ���� �����ϴ� ���� (�浹������ �ƴ�, �浹ü ó���� �ؿ� ����, objSelect()�Լ� ����)

	//����(CEMENT)
	if (
		(frameX == 3 && frameY == 0) || (frameX == 4 && frameY == 0) || (frameX == 5 && frameY == 0) || (frameX == 6 && frameY == 0) ||
		(frameX == 3 && frameY == 1) || (frameX == 4 && frameY == 1) || (frameX == 5 && frameY == 1) || (frameX == 6 && frameY == 1) ||
		(frameX == 3 && frameY == 2) || (frameX == 4 && frameY == 2) || (frameX == 5 && frameY == 2) || (frameX == 6 && frameY == 2) ||
		(frameX == 11 && frameY == 0) || (frameX == 12 && frameY == 0) || (frameX == 16 && frameY == 0) || (frameX == 17 && frameY == 0) ||
		(frameX == 16 && frameY == 1) || (frameX == 17 && frameY == 1) || (frameX == 18 && frameY == 1) || (frameX == 19 && frameY == 1) ||
		(frameX == 16 && frameY == 2) || (frameX == 17 && frameY == 2) || (frameX == 18 && frameY == 2) || (frameX == 19 && frameY == 2) ||
		(frameX == 18 && frameY == 0) || (frameX == 19 && frameY == 0) || (frameX == 35 && frameY == 0) || (frameX == 36 && frameY == 0))
	{
		return TR_CEMENT;
	}

	//��������(EARTH)
	else if (
		(frameX == 10 && frameY == 0) || (frameX == 10 && frameY == 1) || (frameX == 11 && frameY == 1) || (frameX == 12 && frameY == 1) ||
		(frameX == 11 && frameY == 2) || (frameX == 12 && frameY == 2) || (frameX == 24 && frameY == 0) || (frameX == 25 && frameY == 0) ||
		(frameX == 24 && frameY == 1) || (frameX == 25 && frameY == 1) || (frameX == 29 && frameY == 1) || (frameX == 30 && frameY == 1) ||
		(frameX == 31 && frameY == 1) || (frameX == 32 && frameY == 1) || (frameX == 33 && frameY == 1) || (frameX == 37 && frameY == 0) ||
		(frameX == 38 && frameY == 0) || (frameX == 37 && frameY == 1) || (frameX == 38 && frameY == 1))
	{
		return TR_EARTH;
	}

	//�ܵ��(GRASS) (��ǥ���� �ܵ�Ÿ��(����� �ܵ�)�� ����) ������ �ܵ�Ÿ���� �˾Ƽ� ���ϵ�.
	else if (frameX == 1 && frameY == 1)
	{
		return TR_GRASS;
	}

	//��(WATER)
	else if (
		(frameX == 20 && frameY == 0) || (frameX == 21 && frameY == 0) || (frameX == 22 && frameY == 0) || (frameX == 23 && frameY == 0) ||
		(frameX == 20 && frameY == 1) || (frameX == 21 && frameY == 1) || (frameX == 22 && frameY == 1) || (frameX == 23 && frameY == 1) ||
		(frameX == 20 && frameY == 2) || (frameX == 21 && frameY == 2) || (frameX == 22 && frameY == 2) || (frameX == 23 && frameY == 2) ||
		(frameX == 24 && frameY == 2) || (frameX == 25 && frameY == 2) || (frameX == 29 && frameY == 0) || (frameX == 30 && frameY == 0) ||
		(frameX == 31 && frameY == 0) || (frameX == 32 && frameY == 0) || (frameX == 33 && frameY == 0) || (frameX == 34 && frameY == 0) ||
		(frameX == 35 && frameY == 0) || (frameX == 36 && frameY == 0) || (frameX == 34 && frameY == 1) || (frameX == 35 && frameY == 1) || (frameX == 36 && frameY == 1))
	{
		return TR_WATER;
	}

	//�� (��Ÿ�� �� �� object) (WALL)
	else if (
		(frameX == 4 && frameY == 2) || (frameX == 7 && frameY == 0) || (frameX == 8 && frameY == 0) || (frameX == 7 && frameY == 1) ||
		(frameX == 8 && frameY == 1) || (frameX == 9 && frameY == 1) || (frameX == 7 && frameY == 2) || (frameX == 8 && frameY == 2) ||
		(frameX == 9 && frameY == 2) || (frameX == 11 && frameY == 0) || (frameX == 12 && frameY == 0) || (frameX == 15 && frameY == 2) ||
		(frameX == 26 && frameY == 0) || (frameX == 27 && frameY == 0) || (frameX == 28 && frameY == 0) || (frameX == 28 && frameY == 1))
	{
		return TR_WALL;
	}

	return TR_GRASS;
}

OBJECT cr_Maptool::objSelect(int frameX, int frameY)
{
	//	OBJ_BLOCK1, OBJ_BLOCK3, OBJ_BLOCKS, OBJ_FLAG1,
	//  OBJ_FLAG2, OBJ_TANK1, OBJ_TANK2, OBJ_NONE

	//�浹ü�� ó���� ��� ������Ʈ Ÿ�� (OBJ_BLOCK1�� �浹ü)
	if (
		(frameX == 3 && frameY == 0) || (frameX == 4 && frameY == 0) || (frameX == 5 && frameY == 0) || (frameX == 6 && frameY == 0) ||
		(frameX == 3 && frameY == 1) || (frameX == 4 && frameY == 1) || (frameX == 5 && frameY == 1) || (frameX == 6 && frameY == 1) ||
		(frameX == 3 && frameY == 2) || (frameX == 4 && frameY == 2) || (frameX == 5 && frameY == 2) || (frameX == 6 && frameY == 2) ||
		(frameX == 7 && frameY == 0) || (frameX == 8 && frameY == 0) || (frameX == 7 && frameY == 1) || (frameX == 8 && frameY == 1) ||
		(frameX == 9 && frameY == 1) || (frameX == 7 && frameY == 2) || (frameX == 8 && frameY == 2) || (frameX == 9 && frameY == 2) ||
		(frameX == 4 && frameY == 2) || (frameX == 11 && frameY == 0) || (frameX == 12 && frameY == 0) || (frameX == 15 && frameY == 2) ||
		(frameX == 16 && frameY == 0) || (frameX == 17 && frameY == 0) || (frameX == 18 && frameY == 0) || (frameX == 19 && frameY == 0) ||
		(frameX == 20 && frameY == 0) || (frameX == 21 && frameY == 0) || (frameX == 22 && frameY == 0) || (frameX == 23 && frameY == 0) ||
		(frameX == 20 && frameY == 1) || (frameX == 21 && frameY == 1) || (frameX == 22 && frameY == 1) || (frameX == 23 && frameY == 1) ||
		(frameX == 20 && frameY == 2) || (frameX == 21 && frameY == 2) || (frameX == 22 && frameY == 2) || (frameX == 23 && frameY == 2) ||
		(frameX == 24 && frameY == 2) || (frameX == 25 && frameY == 2) || (frameX == 26 && frameY == 0) || (frameX == 27 && frameY == 0) ||
		(frameX == 28 && frameY == 0) || (frameX == 28 && frameY == 1) || (frameX == 29 && frameY == 0) || (frameX == 30 && frameY == 0) ||
		(frameX == 31 && frameY == 0) || (frameX == 32 && frameY == 0) || (frameX == 33 && frameY == 0) || (frameX == 34 && frameY == 0) ||
		(frameX == 35 && frameY == 0) || (frameX == 36 && frameY == 0) || (frameX == 34 && frameY == 1) || (frameX == 35 && frameY == 1) ||
		(frameX == 36 && frameY == 1))
	{
		return OBJ_BLOCK1;
	}

	return OBJ_BLOCK1;
}

void cr_Maptool::input()
{
	//����
	if (_leftButtonDown && PtInRect(&button_save, _ptMouse))
	{
		save();
		build_player_save();
		build_enemy_save();
	}

	//�ҷ�����
	if (_leftButtonDown && PtInRect(&button_load, _ptMouse))
	{
		load();
		build_player_load();
		build_enemy_load();
	}

	//�ǹ� �̹��� ����
	if (KEYMANAGER->isOnceKeyDown(VK_LBUTTON) && PtInRect(&castle_rc, _ptMouse))
	{
		_currentObject.x = _ptMouse.x / IMAGEMANAGER->findImage("Castle")->getFrameWidth();
		_currentObject.y = _ptMouse.y / IMAGEMANAGER->findImage("Castle")->getFrameHeight();

		_isObject = !_isObject;
	}
}

void cr_Maptool::buildsCollision()
{
	//�ǹ� ����

	for (int i = 0; i < buildsMAX; i++)
	{
		if (_leftButtonDown && PtInRect(&builds_player_rc[i], _ptMouse))
		{
			builds_player_coor[i].x = _currentObject.x;
			builds_player_coor[i].y = _currentObject.y;
		}
	}

	for (int i = 0; i < buildsMAX; i++)
	{
		if (_leftButtonDown && PtInRect(&builds_enemy_rc[i], _ptMouse))
		{
			builds_enemy_coor[i].x = _currentObject.x;
			builds_enemy_coor[i].y = _currentObject.y;
		}
	}
}

void cr_Maptool::pixelCollision()
{
	//���� ����?
}

bool cr_Maptool::UnitCreate_Check()
{
	if (PtInRect(&allow_player_unit, _ptMouse))
	{
		for (int i = 0; i < TILEX * TILEY; i++)
		{
			if (PtInRect(&_tiles[i].rc, _ptMouse) && _tiles[i].terrainFrameX == 2)
			{
				return true;
			}
		}
	}
	else
		return false;
}

void cr_Maptool::SettingMap()
{
	
	for (int i = 0; i < TILEX * TILEY; i++)
	{
		IMAGEMANAGER->frameRender("mapTiles", getMemDC(), _tiles[i].rc.left, _tiles[i].rc.top, _tiles[i].terrainFrameX, _tiles[i].terrainFrameY);
	}

	for (int i = 0; i < TILEX * TILEY; i++)
	{
		if (_tiles[i].obj == OBJ_NONE) continue;
		IMAGEMANAGER->frameRender("mapTiles", getMemDC(), _tiles[i].rc.left,
			_tiles[i].rc.top, _tiles[i].objFrameX, _tiles[i].objFrameY);
	}

	for (int i = 0; i < buildsMAX; i++)
	{
		//DrawRect(getMemDC(), builds_player_rc[i]);
		//DrawRect(getMemDC(), builds_enemy_rc[i]);

		//�÷��̾� ĳ�� �̹��� �׷���
		IMAGEMANAGER->frameRender("Castle", getMemDC(), builds_player_rc[i].left, builds_player_rc[i].top, builds_player_coor[i].x, builds_player_coor[i].y);

		//�� ĳ�� �̹��� �׷���
		IMAGEMANAGER->frameRender("Castle", getMemDC(), builds_enemy_rc[i].left, builds_enemy_rc[i].top, builds_enemy_coor[i].x, builds_player_coor[i].y);
	}
}