#pragma once
#include "iMap.h"

#define VERTEX_DIM (128)//257
#define TILE_N (VERTEX_DIM - 1)

class cXobjectLoader;

class cLoadMap : public iMap
{
private:
	D3DXMATRIXA16			m_matWorld;

	// �������� ��� ���Ŀ� ������ ������ �õ� �Ѵ�.
	float					m_fSizeX;
	float					m_fSizeZ;

	//�ε��
	vector<cXobjectLoader*> m_vecXLoader;
	LPDIRECT3DTEXTURE9		m_pTexture;

	int						m_nVertexSize;

	//����ȭ �۾� GetHeight��
	//����ȭ Ȯ�ο� ����(4/4�б�) (4���)
	int					m_fastSearch[9];
	vector<D3DXVECTOR3> m_fastVertex;

	//Ȯ�ο� ���� ���� ������ ���� ����(4/4�б⿡�� 4/4�б�� ����) (16���)
	int					m_fastSearchHalf01[9];
	vector<D3DXVECTOR3> m_fastVertexHalf01;

	int					m_fastSearchHalf02[9];
	vector<D3DXVECTOR3> m_fastVertexHalf02;

	int					m_fastSearchHalf03[9];
	vector<D3DXVECTOR3> m_fastVertexHalf03;

	int					m_fastSearchHalf04[9];
	vector<D3DXVECTOR3> m_fastVertexHalf04;

	//���� ã�� ��ġ �ε��� (16���)
	vector<DWORD>		m_vecfast01Index;
	vector<DWORD>		m_vecfast02Index;
	vector<DWORD>		m_vecfast03Index;
	vector<DWORD>		m_vecfast04Index;

	vector<DWORD>		m_vecfast05Index;
	vector<DWORD>		m_vecfast06Index;
	vector<DWORD>		m_vecfast07Index;
	vector<DWORD>		m_vecfast08Index;

	vector<DWORD>		m_vecfast09Index;
	vector<DWORD>		m_vecfast10Index;
	vector<DWORD>		m_vecfast11Index;
	vector<DWORD>		m_vecfast12Index;

	vector<DWORD>		m_vecfast13Index;
	vector<DWORD>		m_vecfast14Index;
	vector<DWORD>		m_vecfast15Index;
	vector<DWORD>		m_vecfast16Index;

	/*===================================================================================*/
	/*                             private New Func, This Area                           */
	/*===================================================================================*/
	//MainScene
	void LoadAll_MainSceneTerrain();
	void LoadAll_MainSceneX();

	//Village
	void LoadAll_VillageTerrain();
	void LoadAll_VillageX();

	//Stage1
	void LoadAll_Stage1Terrain();
	void LoadAll_Stage1X();

	//Stage2
	void LoadAll_Stage2Terrain();
	void LoadAll_Stage2X();

	//Stage3
	void LoadAll_Stage3Terrain();
	void LoadAll_Stage3X();

	//BossMap
	void LoadAll_BossMapTerrain();
	void LoadAll_BossMapX();

	//����ȭ �Լ�
	void SliceQuarterSetup(int VertexNum);
	/*===================================================================================*/

	SYNTHESIZE_PASS_BY_REF(vector<ST_PNT_VERTEX>, m_vecPNTVertex, PNTVertex);
	SYNTHESIZE_PASS_BY_REF(vector<D3DXVECTOR3>, m_vecVertex, Vertex);
	SYNTHESIZE_PASS_BY_REF(vector<DWORD>, m_vecIndex, Index);

public:
	cLoadMap();
	~cLoadMap();

	virtual void Load(char* szFilePath, D3DXMATRIXA16* pMat) override;	//��� ����.
	virtual bool GetHeight(IN const float x, OUT float& y, IN const float z) override;
	void SliceQuarterCalculation(D3DXVECTOR3& vDest);	//GetHeight�� for�� ����(����� ���� ��������, Ȥ�ó� �ϰ� ����)

	void Render();

	//���� �ε�
	void RealLoad_MainScene();
	void RealLoad_Village();
	void RealLoad_Stage1();
	void RealLoad_Stage2();
	void RealLoad_Stage3();
	void RealLoad_BossMap();
};

