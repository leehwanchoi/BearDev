#include "stdafx.h"
#include "cAllocateHierarchy.h"


cAllocateHierarchy::cAllocateHierarchy()
	//추가
	: m_sDirectory("")
	, m_dwDefaultPaletteSize(0)
	, m_dwMaxPaletteSize(0)
	, m_vMin(0, 0, 0)
	, m_vMax(0, 0, 0)
{
}


cAllocateHierarchy::~cAllocateHierarchy()
{
}

STDMETHODIMP cAllocateHierarchy::CreateFrame(
	THIS_ LPCSTR Name,
	OUT LPD3DXFRAME *ppNewFrame)
{
	ST_BONE* pBone = new ST_BONE;

	if (Name)
	{
		int nLen = strlen(Name) + 1; //추가
		pBone->Name = new char[strlen(Name) + 1];
		strcpy(pBone->Name, Name);
	}
	else
		pBone->Name = NULL;

	//추가
	pBone->pFrameFirstChild = NULL;
	pBone->pFrameSibling = NULL;
	pBone->pMeshContainer = NULL;


	D3DXMatrixIdentity(&pBone->TransformationMatrix);
	D3DXMatrixIdentity(&pBone->CombinedTransformationMatrix);

	*ppNewFrame = pBone;

	return S_OK;
}

STDMETHODIMP cAllocateHierarchy::CreateMeshContainer(
	THIS_ LPCSTR Name,
	CONST D3DXMESHDATA *pMeshData,
	CONST D3DXMATERIAL *pMaterials,
	CONST D3DXEFFECTINSTANCE *pEffectInstances,
	DWORD NumMaterials,
	CONST DWORD *pAdjacency,
	LPD3DXSKININFO pSkinInfo,
	OUT LPD3DXMESHCONTAINER *ppNewMeshContainer)
{
	//추가
	assert(pMeshData && pMeshData->Type == D3DXMESHTYPE_MESH);

	ST_BONE_MESH* pBoneMesh = new ST_BONE_MESH;

	//수정
	if (Name)
	{
		int nLen = strlen(Name) + 1; //추가
		pBoneMesh->Name = new char[strlen(Name) + 1];
		strcpy(pBoneMesh->Name, Name);
	}
	else
		pBoneMesh->Name = NULL;

	//1. 메쉬 데이터(pMesh) 설정 : 오리지널 메쉬, 워크 메쉬
	// 워크 메쉬 = 오리지널->클론메쉬
	SAFE_ADDREF(pMeshData->pMesh);
	pBoneMesh->pOrigMesh = pMeshData->pMesh;	// 얕은 복사(객체 사용에 대해서 레퍼런스 카운트를 증가 시켜줘야 한다.)
	pBoneMesh->pOrigMesh->CloneMeshFVF(pBoneMesh->pOrigMesh->GetOptions(),		// 깊은 복사(새로운 공간에 모든 것을 복사해서 사용하는 것이기
		pBoneMesh->pOrigMesh->GetFVF(), g_pD3DDevice, &pBoneMesh->pWorkMesh);	// 때문에 레퍼런스 카운트를 증가 시킬 필요가 없다.

	pBoneMesh->pMaterials = new D3DXMATERIAL[NumMaterials];
	for (int i = 0; i < NumMaterials; ++i)
	{
		string filePath(pMaterials[i].pTextureFilename);
		string fullPath = m_sFolder + "\\" + filePath;
		g_pTextureManager->AddTexture(filePath, fullPath, NULL, NULL);

		pBoneMesh->pMaterials[i].MatD3D = pMaterials[i].MatD3D;
		pBoneMesh->pMaterials[i].pTextureFilename =
			new char[strlen(pMaterials[i].pTextureFilename) + 1];
		strcpy(pBoneMesh->pMaterials[i].pTextureFilename, pMaterials[i].pTextureFilename);
	}

	pBoneMesh->pEffects = NULL;

	pBoneMesh->NumMaterials = NumMaterials;

	pBoneMesh->pAdjacency = NULL;

	//2. 스킨인포 저장;
	if (pSkinInfo)	//수정
	{
		SAFE_ADDREF(pSkinInfo);
		pBoneMesh->pSkinInfo = pSkinInfo;

		//3. pSkinInfo->GetNumBones() : 현재 메쉬에 영향을 주는 본의 갯수 정보
		DWORD dwNumBones = pSkinInfo->GetNumBones();
		// ppBoneMatrixPtrs, pBoneOffsetMatrices, pCurrentBoneMatrices 들을 할당
		pBoneMesh->ppBoneMatrixPtrs = new D3DXMATRIX*[dwNumBones];
		pBoneMesh->pBoneOffsetMatrix = new D3DXMATRIX[dwNumBones];
		pBoneMesh->pCurrentBoneMatrices = new D3DXMATRIX[dwNumBones];

		//4. 동적 할당 된 pBoneOffsetMatrices 매트릭스 값 저장
		// pSkinInfo->GetBoneOffsetMatrix(i)
		// ppBoneMatrixPtrs, pCurrentBoneMatrices 두 배열은 로딩중에 채울 수 없음.
		for (DWORD i = 0; i < dwNumBones; ++i)
		{
			pBoneMesh->pBoneOffsetMatrix[i] = *(pSkinInfo->GetBoneOffsetMatrix(i));
		}
	}
	else pBoneMesh->pSkinInfo = NULL;	//수정

	*ppNewMeshContainer = pBoneMesh;

	return S_OK;
}

STDMETHODIMP cAllocateHierarchy::DestroyFrame(
	THIS_ LPD3DXFRAME pFrameToFree)
{
	SAFE_DELETE_ARRAY(pFrameToFree->Name);

	SAFE_DELETE(pFrameToFree);

	return S_OK;
}

STDMETHODIMP cAllocateHierarchy::DestroyMeshContainer(
	THIS_ LPD3DXMESHCONTAINER pMeshContainerToFree)
{
	// 추가로 동적 할당 된 내용이 있다면 해제도 추가해 줘라.
	ST_BONE_MESH* pBoneMesh = (ST_BONE_MESH*)pMeshContainerToFree;

	SAFE_RELEASE(pBoneMesh->pOrigMesh);
	SAFE_RELEASE(pBoneMesh->pWorkMesh);

	for (int i = 0; i < pBoneMesh->NumMaterials; ++i)
		SAFE_DELETE_ARRAY(pBoneMesh->pMaterials[i].pTextureFilename);

	SAFE_DELETE_ARRAY(pBoneMesh->pMaterials);

	if (pBoneMesh->pSkinInfo)	//수정
	{
		SAFE_DELETE_ARRAY(pBoneMesh->ppBoneMatrixPtrs);
		SAFE_DELETE_ARRAY(pBoneMesh->pBoneOffsetMatrix);
		SAFE_DELETE_ARRAY(pBoneMesh->pCurrentBoneMatrices);

		SAFE_RELEASE(pBoneMesh->pSkinInfo);
	}

	SAFE_DELETE(pBoneMesh);

	return S_OK;
}