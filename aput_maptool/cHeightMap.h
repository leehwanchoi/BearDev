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

	// �������� ��� ���Ŀ� ������ ������ �õ� �Ѵ�.
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
fopen_s(..., ..., "rb")			// ������ ����Ʈ ������ �д´�. 1byte = 8bit = 2^8 = 256
y = (unsinged char)fgetc(fp)	// char = 1byte => -128 ~ 127, unsigned char=> 0 ~ 256

1. ���ؽ� ����(�븻�� ���Ƿ� 0, 1, 0���� �ִ´�.)
�ε��� ��ȣ = z * VERTEX_DIM + x
y /= 10.0f;

2. �ε��� ����(�ð� ����)
1--3	0 = (z + 0) * VERTEX_DIM + (x + 0)
|\ |	1 = (z + 1) * VERTEX_DIM + (x + 0)
| \|	2 = (z + 0) * VERTEX_DIM + (x + 1)
0--2	3 = (z + 1) * VERTEX_DIM + (x + 1)

3. �븻�� ����
---u---
|\ |\ |
| \| \|
L--n--r
|\ |\ |
| \| \|
---d---

du ���Ϳ� lr ���͸� �����ؼ�
n��ġ�� �븻 ���� ���Ѵ�.

4. �޽��� ���� (���� �ְ� ����ȭ ����)

5. MtlTex ���� ����(ȭ��Ʈ ���͸���, �ؽ��� 1��)
*/

/*
GetHeight �Լ� - ĳ���� ���� ����

1. ���� ���̽� ����
1--3	0�� x z ��ǥ ã��
|\ |	x = (int)��ġx
| \|	z = (int)��ġz
0--2	deltaX = ��ġx - x
		deltaZ = ��ġz - z

deltaX + deltaZ <  1 : �Ʒ��� �ﰢ��
deltaX + deltaZ	>= 1 : ���� �ﰢ��

1--3	�� �� ������ ���� ���� ���
|\ |	�Ʒ��� �ﰢ���� ��� : 0 + �������� * ��Ÿ��
| \|	���� �ﰢ���� ��� : 3 + �������� * (1 - ��Ÿ��)
0--2
*/