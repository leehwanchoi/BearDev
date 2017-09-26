#pragma once
#include "iMap.h"

class cMtlTex;

#define VERTEX_DIM (257)
#define TILE_N (VERTEX_DIM - 1)


class cHeightMap : public iMap
{
private:
	D3DXMATRIXA16			m_matWorld;
	LPD3DXMESH				m_pMesh;
	cMtlTex*				m_pMtlTex;

	SYNTHESIZE_PASS_BY_REF(vector<D3DXVECTOR3>, m_vecVertex, Vertex);
	SYNTHESIZE_PASS_BY_REF(vector<DWORD>, m_vecIndex, Index);

	// 정상적인 출력 이후에 사이즈 변경을 시도 한다.
	float					m_fSizeX;
	float					m_fSizeZ;

public:
	cHeightMap();
	~cHeightMap();

	virtual void Load(char* szFilePath, D3DXMATRIXA16* pMat) override;
	virtual bool GetHeight(IN const float x, OUT float& y, IN const float z) override;
	void Render();
};

/*
fopen_s(..., ..., "rb")			// 파일을 바이트 단위로 읽는다. 1byte = 8bit = 2^8 = 256
y = (unsinged char)fgetc(fp)	// char = 1byte => -128 ~ 127, unsigned char=> 0 ~ 256

1. 버텍스 설정(노말은 임의로 0, 1, 0으로 넣는다.)
인덱스 번호 = z * VERTEX_DIM + x
y /= 10.0f;

2. 인덱스 구성(시계 방향)
1--3	0 = (z + 0) * VERTEX_DIM + (x + 0)
|\ |	1 = (z + 1) * VERTEX_DIM + (x + 0)
| \|	2 = (z + 0) * VERTEX_DIM + (x + 1)
0--2	3 = (z + 1) * VERTEX_DIM + (x + 1)

3. 노말값 셋팅
---u---
|\ |\ |
| \| \|
L--n--r
|\ |\ |
| \| \|
---d---

du 벡터와 lr 벡터를 외적해서
n위치의 노말 값을 구한다.

4. 메쉬를 생성 (정보 넣고 최적화 까지)

5. MtlTex 정보 셋팅(화이트 매터리얼, 텍스쳐 1장)
*/

/*
GetHeight 함수 - 캐릭터 높이 조정

1. 기준 페이스 선택
1--3	0의 x z 좌표 찾기
|\ |	x = (int)위치x
| \|	z = (int)위치z
0--2	deltaX = 위치x - x
		deltaZ = 위치z - z

deltaX + deltaZ <  1 : 아래쪽 삼각형
deltaX + deltaZ	>= 1 : 위쪽 삼각형

1--3	두 점 사이의 높이 차이 계산
|\ |	아래쪽 삼각형일 경우 : 0 + 높이차이 * 델타값
| \|	위쪽 삼각형일 경우 : 3 + 높이차이 * (1 - 델타값)
0--2
*/