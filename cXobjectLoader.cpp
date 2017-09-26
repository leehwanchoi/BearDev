#include "stdafx.h"
#include "cXobjectLoader.h"


cXobjectLoader::cXobjectLoader()
	: m_isSphere(false)
{
	D3DXMatrixIdentity(&m_matMeshW);
	D3DXMatrixIdentity(&m_matColW);
}

cXobjectLoader::~cXobjectLoader()
{
}

void cXobjectLoader::Load(char* lpstrFile, bool isSphere)
{
	ID3DXBuffer* adjBuffer = 0;
	ID3DXBuffer* mtrlBuffer = 0;
	DWORD		numMtrls = 0;
	LPD3DXMESH	pMesh = 0;
	LPD3DXMESH	SphereMesh = 0;
	LPD3DXMESH	BoxMesh = 0;
	m_isSphere = isSphere;

	D3DXLoadMeshFromX(
		lpstrFile,
		D3DXMESH_MANAGED,
		g_pD3DDevice,
		&adjBuffer,
		&mtrlBuffer,
		0,
		&numMtrls,
		&pMesh);

	//X파일 이름을 담는다.(로드용)
	m_vecFileName.push_back(lpstrFile);

	//X파일로 부터 얻은 메쉬 값을 담는다.
	m_vecMesh.push_back(pMesh);

	//x파일 내에 오브젝트가 여러개인경우 다 그리기 위해서 갯수를 받는다.
	m_vecMtlNum.push_back(numMtrls);

	//텍스쳐 이름을 담고, 텍스쳐매니저에 추가한다.
	D3DXMATERIAL* mtrls = (D3DXMATERIAL*)mtrlBuffer->GetBufferPointer();

	for (int i = 0; i < numMtrls; i++)
	{
		string temp (mtrls[i].pTextureFilename);
		string FullPath("./MapToolX/"); //이곳에서 폴더명 변경
		FullPath += mtrls[i].pTextureFilename;
		
		//텍스쳐 추가
		g_pTextureManager->AddTexture(FullPath, FullPath, NULL);

		//텍스쳐 이름 담기
		m_vecTextureName.push_back(FullPath);
		//매터리얼 담기
		m_vecMaterial.push_back(mtrls[i].MatD3D);
	}

	//각각의 매트리스 담기.
	m_vecMatW.push_back(m_matMeshW);

	m_matColW = m_matMeshW;

	//충돌체 담기.
	//원인 경우
	if (m_isSphere)
	{
		BoundingSphere boundingSphere;
		ComputeBoundingSphere(pMesh, &boundingSphere);
		D3DXCreateSphere(
			g_pD3DDevice,
			boundingSphere._radius,
			10,
			10,
			&SphereMesh,
			0);

		m_vecCollision.push_back(SphereMesh);

		//충돌체 중점 계산후 담기.
		m_matColW._42 = m_matMeshW._42 + (boundingSphere._radius / 2) - boundingSphere._center.y;
		m_vecCollisionMatW.push_back(m_matColW);
	}
	//박스인 경우
	else
	{
		BoundingBox    boundingBox;
		ComputeBoundingBox(pMesh, &boundingBox);

		D3DXCreateBox(
			g_pD3DDevice,
			boundingBox._max.x - boundingBox._min.x,
			boundingBox._max.y - boundingBox._min.y,
			boundingBox._max.z - boundingBox._min.z,
			&BoxMesh,
			0);

		m_vecCollision.push_back(BoxMesh);

		//충돌체 중점 계산후 담기.
		m_matColW._42 = m_matMeshW._42 + ((boundingBox._max.y + boundingBox._min.y) / 2);
		m_vecCollisionMatW.push_back(m_matColW);
	}
}

void cXobjectLoader::Update()
{

}

void cXobjectLoader::Render()
{
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, true);

	//메쉬가 있는 경우에만 그린다.
	if (m_vecMesh.size() > 0 && !m_vecMesh.empty())
	{
		//메쉬 사이즈만큼
		for (int i = 0; i < m_vecMesh.size(); i++)
		{
			//매터리얼 갯수가 1개인 경우 (0부터 그려야 하기 때문에 매터리얼갯수-1을 해준다.)
			if (m_vecMtlNum[i] - 1 == 0) 
			{
				//조정된 매트리스로 적용
				g_pD3DDevice->SetTransform(D3DTS_WORLD, &m_vecMatW[i]);
				g_pD3DDevice->SetMaterial(&m_vecMaterial[i]);
				g_pD3DDevice->SetTexture(0, g_pTextureManager->GetTexture(m_vecTextureName[i]));
				m_vecMesh[i]->DrawSubset(0);
			}

			//매터리얼 갯수가 여러개인 경우
			else
			{
				//매터리얼 갯수만큼
				for (int j = 0; j < m_vecMtlNum[i]; j++)
				{
					//조정된 매트리스로 적용
					g_pD3DDevice->SetTransform(D3DTS_WORLD, &m_vecMatW[i]);
					g_pD3DDevice->SetMaterial(&m_vecMaterial[j]);
					g_pD3DDevice->SetTexture(0, g_pTextureManager->GetTexture(m_vecTextureName[j]));
					m_vecMesh[i]->DrawSubset(j);
				}
			}
		}
	}

	//와이어로 그리기
	g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, false);

	if (g_pKeyManager->isToggleKey(VK_NUMLOCK))
	{
		//충돌체 메쉬가 있는 경우에만 그린다.
		if (m_vecCollision.size() > 0 && !m_vecCollision.empty())
		{
			//충돌체 메쉬 사이즈 만큼
			for (int i = 0; i < m_vecCollision.size(); i++)
			{
				g_pD3DDevice->SetMaterial(&WHITE_MTRL);
				g_pD3DDevice->SetTransform(D3DTS_WORLD, &m_vecCollisionMatW[i]);
				m_vecCollision[i]->DrawSubset(0);
			}
		}
	}

	//원상태로 그리기(와이어->원래로 복구)
	g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, false);
}

void cXobjectLoader::Destroy()
{
	//메쉬 제거
	if (!m_vecMesh.empty())
	{
		for (int i = 0; i < m_vecMesh.size(); i++)
			SAFE_RELEASE(m_vecMesh[i]);
	}

	//충돌체 메쉬 제거
	if (!m_vecCollision.empty())
	{
		for (int i = 0; i < m_vecCollision.size(); i++)
			SAFE_RELEASE(m_vecCollision[i]);
	}
}

void cXobjectLoader::SelectedCollisionRender()
{
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, true);
	//와이어로 그리기
	g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	//충돌체 메쉬가 있는 경우에만 그린다.
	if (m_vecCollision.size() > 0 && !m_vecCollision.empty())
	{
		g_pD3DDevice->SetMaterial(&BLACK_MTRL);
		g_pD3DDevice->SetTransform(D3DTS_WORLD, &m_vecCollisionMatW[0]);
		m_vecCollision[0]->DrawSubset(0);
	}

	//원상태로 그리기(와이어->원래로 복구)
	g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, false);
}

bool cXobjectLoader::ComputeBoundingBox(ID3DXMesh* mesh, BoundingBox* box)
{
	HRESULT hr = 0;

	BYTE* v = 0;
	mesh->LockVertexBuffer(0, (void**)&v);

	hr = D3DXComputeBoundingBox(
		(D3DXVECTOR3*)v,
		mesh->GetNumVertices(),
		D3DXGetFVFVertexSize(mesh->GetFVF()),
		&box->_min,
		&box->_max);

	mesh->UnlockVertexBuffer();

	if (FAILED(hr))
		return false;

	return true;
}

bool cXobjectLoader::ComputeBoundingSphere(ID3DXMesh* mesh, BoundingSphere* sphere)
{
	HRESULT hr = 0;

	BYTE* v = 0;
	mesh->LockVertexBuffer(0, (void**)&v);

	hr = D3DXComputeBoundingSphere(
		(D3DXVECTOR3*)v,
		mesh->GetNumVertices(),
		D3DXGetFVFVertexSize(mesh->GetFVF()),
		&sphere->_center,
		&sphere->_radius);

	mesh->UnlockVertexBuffer();

	if (FAILED(hr))
		return false;

	return true;
}
