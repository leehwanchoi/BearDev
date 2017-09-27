#pragma once

#include "gameNode.h"

//타일 하나의 사이즈가 32 * 32
#define TILESIZE 32

//타일 갯수
#define TILEX 40
#define TILEY 15

//총 타일 사이즈
#define TILESIZEX TILESIZE * TILEX
#define TILESIZEY TILESIZE * TILEY

//맵을 그려올 이미지 타일 종류들이 있는 그림 파일 크기
#define SAMPLETILEX 39
#define SAMPLETILEY 3

#define buildsMAX 3

//터레인
enum TERRAIN
{
	TR_CEMENT, TR_EARTH, TR_GRASS, TR_WATER, TR_WALL, TR_END
};

//각종 사물들
enum OBJECT
{
	OBJ_BLOCK1, OBJ_BLOCK3, OBJ_BLOCKS, OBJ_FLAG1,
	OBJ_FLAG2, OBJ_TANK1, OBJ_TANK2, OBJ_NONE
};

//포지션
enum POS
{
	POS_FLAG1, POS_FLAG2, POS_TANK1, POS_TANK2
};

struct tagTile
{
	TERRAIN terrain;
	OBJECT obj;
	RECT rc;
	int terrainFrameX;
	int terrainFrameY;
	int objFrameX;
	int objFrameY;
};

//가져올 샘플 타일 정보에 관한 구조체
struct tagSampleTile
{
	RECT rcTile;
	int terrainFrameX;
	int terrainFrameY;
};

//현재 타일 가져올 구조체
struct tagCurrentTile
{
	int x;
	int y;
};

class cr_Maptool : public gameNode
{
private:
	tagCurrentTile _currentTile;
	tagCurrentTile _currentObject;
	tagTile _tiles[TILEX * TILEY];
	tagSampleTile _sampleTiles[SAMPLETILEX * SAMPLETILEY];

	int _pos[2];
	int _ctrlSelect;

	RECT button_save;
	RECT button_load;
	RECT button_play;
	RECT mouse_rc;

	RECT castle_rc;
	RECT builds_player_rc[buildsMAX];
	RECT builds_enemy_rc[buildsMAX];

	RECT allow_player_unit;
	RECT allow_enemy_unit;

	tagCurrentTile builds_player_coor[buildsMAX];
	tagCurrentTile builds_enemy_coor[buildsMAX];

	bool _isObject;
public:
	cr_Maptool();
	~cr_Maptool();

	HRESULT init();
	void release();
	void update();
	void render();

	void setMap() override;
	void setUp();

	void save() override;
	void load() override;

	void build_player_save();
	void build_player_load();

	void build_enemy_save();
	void build_enemy_load();

	void input();
	void buildsCollision();
	void pixelCollision();

	bool UnitCreate_Check();

	TERRAIN terrainSelect(int frameX, int frameY);
	OBJECT objSelect(int frameX, int frameY);
	void setCtrlSelect(int num) { _ctrlSelect = num; }

	void SettingMap();

	RECT Tiles(int [TILEX*TILEY]){ return _tiles[TILEX * TILEY].rc; }

};