#pragma once

struct ST_BONE : public D3DXFRAME
{
	D3DXMATRIX CombinedTransformationMatrix;
};

struct ST_BONE_MESH : public D3DXMESHCONTAINER
{
	//�߰�
	vector<LPDIRECT3DTEXTURE9>  vecTexture;
	vector<D3DMATERIAL9>		vecMaterial;

	LPD3DXMESH			pOrigMesh;				// ���� �޽� (Ʈ������ ��� �ܼ�ȭ�� ���ؼ� ����)
	LPD3DXMESH			pWorkMesh;				// �۾� �޽� (���� ����): �ִϸ��̼��� ó�� �� �޽�
	D3DXMATRIX**		ppBoneMatrixPtrs;		// �ش� �޽��� ������ �ִ� �����ӵ��� �����Ʈ���� ������ �迭
	D3DXMATRIX*			pBoneOffsetMatrix;		// ���� �޽��� ���ý����̽��� ������ ��Ʈ������
	D3DXMATRIX*			pCurrentBoneMatrices;	// �� ���� ��� �� �����Ʈ����

	//�߰�
	DWORD				dwNumPaletteEntries;
	DWORD				dwMaxNumFaceInfls;
	DWORD				dwNumAttrGroups;
	LPD3DXBUFFER		pBufBoneCombos;
};

class cAllocateHierarchy : public ID3DXAllocateHierarchy
{
	SYNTHESIZE_PASS_BY_REF(string, m_sFolder, Folder);

public:
	cAllocateHierarchy();
	~cAllocateHierarchy();

	//�߰�
	SYNTHESIZE(string, m_sDirectory, Directory);
	SYNTHESIZE(D3DXVECTOR3, m_vMin, Min);
	SYNTHESIZE(D3DXVECTOR3, m_vMax, Max);
	SYNTHESIZE(DWORD, m_dwDefaultPaletteSize, DefaultPaletteSize);
	SYNTHESIZE(DWORD, m_dwMaxPaletteSize, MaxPaletteSize);


	STDMETHOD(CreateFrame)(THIS_ LPCSTR Name,
		OUT LPD3DXFRAME *ppNewFrame) override;

	STDMETHOD(CreateMeshContainer)(THIS_
		LPCSTR Name,
		CONST D3DXMESHDATA *pMeshData,
		CONST D3DXMATERIAL *pMaterials,
		CONST D3DXEFFECTINSTANCE *pEffectInstances,
		DWORD NumMaterials,
		CONST DWORD *pAdjacency,
		LPD3DXSKININFO pSkinInfo,
		OUT LPD3DXMESHCONTAINER *ppNewMeshContainer) override;

	STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree) override;

	STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerToFree) override;
};

