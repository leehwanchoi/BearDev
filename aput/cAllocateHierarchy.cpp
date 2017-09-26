#include "stdafx.h"
#include "cAllocateHierarchy.h"


cAllocateHierarchy::cAllocateHierarchy()
	//�߰�
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
		int nLen = strlen(Name) + 1; //�߰�
		pBone->Name = new char[strlen(Name) + 1];
		strcpy(pBone->Name, Name);
	}
	else
		pBone->Name = NULL;

	//�߰�
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
	//�߰�
	assert(pMeshData && pMeshData->Type == D3DXMESHTYPE_MESH);

	ST_BONE_MESH* pBoneMesh = new ST_BONE_MESH;

	//����
	if (Name)
	{
		int nLen = strlen(Name) + 1; //�߰�
		pBoneMesh->Name = new char[strlen(Name) + 1];
		strcpy(pBoneMesh->Name, Name);
	}
	else
		pBoneMesh->Name = NULL;

	//1. �޽� ������(pMesh) ���� : �������� �޽�, ��ũ �޽�
	// ��ũ �޽� = ��������->Ŭ�и޽�
	SAFE_ADDREF(pMeshData->pMesh);
	pBoneMesh->pOrigMesh = pMeshData->pMesh;	// ���� ����(��ü ��뿡 ���ؼ� ���۷��� ī��Ʈ�� ���� ������� �Ѵ�.)
	pBoneMesh->pOrigMesh->CloneMeshFVF(pBoneMesh->pOrigMesh->GetOptions(),		// ���� ����(���ο� ������ ��� ���� �����ؼ� ����ϴ� ���̱�
		pBoneMesh->pOrigMesh->GetFVF(), g_pD3DDevice, &pBoneMesh->pWorkMesh);	// ������ ���۷��� ī��Ʈ�� ���� ��ų �ʿ䰡 ����.

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

	//2. ��Ų���� ����;
	if (pSkinInfo)	//����
	{
		SAFE_ADDREF(pSkinInfo);
		pBoneMesh->pSkinInfo = pSkinInfo;

		//3. pSkinInfo->GetNumBones() : ���� �޽��� ������ �ִ� ���� ���� ����
		DWORD dwNumBones = pSkinInfo->GetNumBones();
		// ppBoneMatrixPtrs, pBoneOffsetMatrices, pCurrentBoneMatrices ���� �Ҵ�
		pBoneMesh->ppBoneMatrixPtrs = new D3DXMATRIX*[dwNumBones];
		pBoneMesh->pBoneOffsetMatrix = new D3DXMATRIX[dwNumBones];
		pBoneMesh->pCurrentBoneMatrices = new D3DXMATRIX[dwNumBones];

		//4. ���� �Ҵ� �� pBoneOffsetMatrices ��Ʈ���� �� ����
		// pSkinInfo->GetBoneOffsetMatrix(i)
		// ppBoneMatrixPtrs, pCurrentBoneMatrices �� �迭�� �ε��߿� ä�� �� ����.
		for (DWORD i = 0; i < dwNumBones; ++i)
		{
			pBoneMesh->pBoneOffsetMatrix[i] = *(pSkinInfo->GetBoneOffsetMatrix(i));
		}
	}
	else pBoneMesh->pSkinInfo = NULL;	//����

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
	// �߰��� ���� �Ҵ� �� ������ �ִٸ� ������ �߰��� ���.
	ST_BONE_MESH* pBoneMesh = (ST_BONE_MESH*)pMeshContainerToFree;

	SAFE_RELEASE(pBoneMesh->pOrigMesh);
	SAFE_RELEASE(pBoneMesh->pWorkMesh);

	for (int i = 0; i < pBoneMesh->NumMaterials; ++i)
		SAFE_DELETE_ARRAY(pBoneMesh->pMaterials[i].pTextureFilename);

	SAFE_DELETE_ARRAY(pBoneMesh->pMaterials);

	if (pBoneMesh->pSkinInfo)	//����
	{
		SAFE_DELETE_ARRAY(pBoneMesh->ppBoneMatrixPtrs);
		SAFE_DELETE_ARRAY(pBoneMesh->pBoneOffsetMatrix);
		SAFE_DELETE_ARRAY(pBoneMesh->pCurrentBoneMatrices);

		SAFE_RELEASE(pBoneMesh->pSkinInfo);
	}

	SAFE_DELETE(pBoneMesh);

	return S_OK;
}