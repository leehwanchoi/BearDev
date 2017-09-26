#pragma once

struct ST_BONE : public D3DXFRAME
{
	D3DXMATRIX CombinedTransformationMatrix;
};

struct ST_BONE_MESH : public D3DXMESHCONTAINER
{
	//추가
	vector<LPDIRECT3DTEXTURE9>  vecTexture;
	vector<D3DMATERIAL9>		vecMaterial;

	LPD3DXMESH			pOrigMesh;				// 원본 메쉬 (트리구조 경로 단순화를 위해서 선언)
	LPD3DXMESH			pWorkMesh;				// 작업 메쉬 (깊은 복사): 애니메이션이 처리 된 메쉬
	D3DXMATRIX**		ppBoneMatrixPtrs;		// 해당 메쉬에 영향을 주는 프레임들의 월드매트릭스 포인터 배열
	D3DXMATRIX*			pBoneOffsetMatrix;		// 원본 메쉬를 로컬스페이스로 보내는 매트릭스들
	D3DXMATRIX*			pCurrentBoneMatrices;	// 각 본의 계산 된 월드매트릭스

	//추가
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

	//추가
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

