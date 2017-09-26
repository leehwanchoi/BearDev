#include "stdafx.h"
#include "cMapToolInUI.h"
#include "cUIImageView.h"
#include "cUITextView.h"
#include "cXobjectLoader.h"


cMapToolInUI::cMapToolInUI()
	: m_pSprite(NULL)
	, m_pUIRoot(NULL)
	, m_XIndex(0)
	, m_IsLClick(false)
	, m_IsRClick(false)
{
	ZeroMemory(&m_stImageInfo, sizeof(D3DXIMAGE_INFO));
}


cMapToolInUI::~cMapToolInUI()
{
	SAFE_RELEASE(m_pSprite);

	for (int i = 0; i < m_vecMesh.size(); i++)
		SAFE_RELEASE(m_vecMesh[i]);
}

void cMapToolInUI::Setup()
{
	D3DXCreateSprite(g_pD3DDevice, &m_pSprite);

	LPDIRECT3DTEXTURE9 pTexture;
	D3DXIMAGE_INFO stImageInfo;

	char str[1024] = "./MapToolUI/XViewAlpha.png";
	g_pTextureManager->AddTexture(str, str, &pTexture, &stImageInfo);

	cUIImageView* pImageView = new cUIImageView;
	pImageView->AutoRelease();
	pImageView->SetTexture(pTexture);
	pImageView->SetSize(ST_SIZE(stImageInfo.Width, stImageInfo.Height));
	pImageView->SetLocalPos(D3DXVECTOR3(WINSIZEX - stImageInfo.Width, WINSIZEY - stImageInfo.Height, 0));
	pImageView->SetDebugRender(false);
	m_pUIRoot = pImageView;

	cUITextView* pTextView = new cUITextView;
	pTextView->AutoRelease();
	pTextView->SetText("XPreView");
	pTextView->SetSize(ST_SIZE(100, 30));
	pTextView->SetLocalPos(D3DXVECTOR3((stImageInfo.Width / 2) - (pTextView->GetSize().fWidth / 2) + 5, 0, 0));
	pTextView->SetFont(g_pFontManager->GetFont(cFontManager::E_QUEST));
	pTextView->SetDebugRender(false);
	pTextView->SetDrawTextFormat(DT_CENTER | DT_VCENTER | DT_WORDBREAK);
	pTextView->SetColor(D3DCOLOR_XRGB(200, 175, 0));
	pTextView->SetTag(E_TEXTVIEW);
	m_pUIRoot->AddChild(pTextView);

	pTextView = new cUITextView;
	pTextView->AutoRelease();
	pTextView->SetText("if you want see Folder+File Name, Press /");
	pTextView->SetSize(ST_SIZE(1000, 30));
	pTextView->SetLocalPos(D3DXVECTOR3(-1000, 170, 0));
	pTextView->SetFont(g_pFontManager->GetFont(cFontManager::E_QUEST));
	pTextView->SetDebugRender(false);
	pTextView->SetDrawTextFormat(DT_CENTER | DT_VCENTER | DT_WORDBREAK);
	pTextView->SetColor(D3DCOLOR_XRGB(200, 175, 0));
	pTextView->SetTag(E_FolderFileTEXTVIEW);
	m_pUIRoot->AddChild(pTextView);

	cUIButton* pButton = NULL;
	pButton = new cUIButton;
	pButton->AutoRelease();
	pButton->SetTexture("./MapToolUI/LeftButton.png", "./MapToolUI/LeftButton.png", "./MapToolUI/LeftButton.png");
	pButton->SetSize(ST_SIZE(35, 35));
	pButton->SetLocalPos(D3DXVECTOR3(10, 85, 0));
	pButton->SetDebugRender(false);
	pButton->SetTag(E_BUTTON1);
	pButton->SetDelegate(this);
	m_pUIRoot->AddChild(pButton);

	pButton = new cUIButton;
	pButton->AutoRelease();
	pButton->SetTexture("./MapToolUI/RightButton.png", "./MapToolUI/RightButton.png", "./MapToolUI/RightButton.png");
	pButton->SetSize(ST_SIZE(35, 35));
	pButton->SetLocalPos(D3DXVECTOR3(stImageInfo.Width - 30, 85, 0));
	pButton->SetDebugRender(false);
	pButton->SetTag(E_BUTTON2);
	pButton->SetDelegate(this);
	m_pUIRoot->AddChild(pButton);

	//미리 X 파일을 로드한다. (따로 열면 전체 폴더명이 뜨기 때문에, 다른 곳에서 작업하는 사람에게 버그가 발생하게 할 우려가 있다.)
	// all-objects.max.x는 추가 할 필요 없다.
	Load("./MapToolX/bench_A_01.max.x");
	m_vecXName.push_back("./MapToolX/bench_A_01.max.x");
	Load("./MapToolX/bench_A_02.max.x");
	m_vecXName.push_back("./MapToolX/bench_A_02.max.x");
	Load("./MapToolX/bench_B_01.max.x");
	m_vecXName.push_back("./MapToolX/bench_B_01.max.x");
	Load("./MapToolX/bench_B_02.max.x");
	m_vecXName.push_back("./MapToolX/bench_B_02.max.x");
	Load("./MapToolX/bridge_Bigg_01.max.x");
	m_vecXName.push_back("./MapToolX/bridge_Bigg_01.max.x");
	Load("./MapToolX/bridge_Bigg_02.max.x");
	m_vecXName.push_back("./MapToolX/bridge_Bigg_02.max.x");
	Load("./MapToolX/bridge_small.max.x");
	m_vecXName.push_back("./MapToolX/bridge_small.max.x");
	Load("./MapToolX/candy_shop_01.max.x");
	m_vecXName.push_back("./MapToolX/candy_shop_01.max.x");
	Load("./MapToolX/Christmas-village-level.x");
	m_vecXName.push_back("./MapToolX/Christmas-village-level.x");
	Load("./MapToolX/church_01.max.x");
	m_vecXName.push_back("./MapToolX/church_01.max.x");
	Load("./MapToolX/donut_shop_01.max.x");
	m_vecXName.push_back("./MapToolX/donut_shop_01.max.x");
	Load("./MapToolX/fence_A_01.max.x");
	m_vecXName.push_back("./MapToolX/fence_A_01.max.x");
	Load("./MapToolX/fence_A_02.max.x");
	m_vecXName.push_back("./MapToolX/fence_A_02.max.x");
	Load("./MapToolX/fence_A_03.max.x");
	m_vecXName.push_back("./MapToolX/fence_A_03.max.x");
	Load("./MapToolX/fence_B_01.max.x");
	m_vecXName.push_back("./MapToolX/fence_B_01.max.x");
	Load("./MapToolX/fence_B_02.max.x");
	m_vecXName.push_back("./MapToolX/fence_B_02.max.x");
	Load("./MapToolX/fence_B_03.max.x");
	m_vecXName.push_back("./MapToolX/fence_B_03.max.x");
	Load("./MapToolX/fence_C_01.max.x");
	m_vecXName.push_back("./MapToolX/fence_C_01.max.x");
	Load("./MapToolX/fence_C_02.max.x");
	m_vecXName.push_back("./MapToolX/fence_C_02.max.x");
	Load("./MapToolX/fence_C_03.max.x");
	m_vecXName.push_back("./MapToolX/fence_C_03.max.x");
	Load("./MapToolX/fence_D_01.max.x");
	m_vecXName.push_back("./MapToolX/fence_D_01.max.x");
	Load("./MapToolX/fence_D_02.max.x");
	m_vecXName.push_back("./MapToolX/fence_D_02.max.x");
	Load("./MapToolX/fence_D_03.max.x");
	m_vecXName.push_back("./MapToolX/fence_D_03.max.x");
	Load("./MapToolX/fence_E_01.max.x");
	m_vecXName.push_back("./MapToolX/fence_E_01.max.x");
	Load("./MapToolX/fence_E_02.max.x");
	m_vecXName.push_back("./MapToolX/fence_E_02.max.x");
	Load("./MapToolX/fir-tree_A_01.max.x");
	m_vecXName.push_back("./MapToolX/fir-tree_A_01.max.x");
	Load("./MapToolX/fir-tree_B_01.max.x");
	m_vecXName.push_back("./MapToolX/fir-tree_B_01.max.x");
	Load("./MapToolX/fir-tree_C_01.max.x");
	m_vecXName.push_back("./MapToolX/fir-tree_C_01.max.x");
	Load("./MapToolX/hayloft_A_01.max.x");
	m_vecXName.push_back("./MapToolX/hayloft_A_01.max.x");
	Load("./MapToolX/haystack_A_01.max.x");
	m_vecXName.push_back("./MapToolX/haystack_A_01.max.x");
	Load("./MapToolX/haystack_B_01.max.x");
	m_vecXName.push_back("./MapToolX/haystack_B_01.max.x");
	Load("./MapToolX/house_A_v1_01.max.x");
	m_vecXName.push_back("./MapToolX/house_A_v1_01.max.x");
	Load("./MapToolX/house_A_v2_01.max.x");
	m_vecXName.push_back("./MapToolX/house_A_v2_01.max.x");
	Load("./MapToolX/house_B_v1_01.max.x");
	m_vecXName.push_back("./MapToolX/house_B_v1_01.max.x");
	Load("./MapToolX/house_B_v2_01.max.x");
	m_vecXName.push_back("./MapToolX/house_B_v2_01.max.x");
	Load("./MapToolX/house_Big_v1_01.max.x");
	m_vecXName.push_back("./MapToolX/house_Big_v1_01.max.x");
	Load("./MapToolX/house_Big_v2_01.max.x");
	m_vecXName.push_back("./MapToolX/house_Big_v2_01.max.x");
	Load("./MapToolX/house_C_v1_01.max.x");
	m_vecXName.push_back("./MapToolX/house_C_v1_01.max.x");
	Load("./MapToolX/house_C_v2_01.max.x");
	m_vecXName.push_back("./MapToolX/house_C_v2_01.max.x");
	Load("./MapToolX/house_D_v1_01.max.x");
	m_vecXName.push_back("./MapToolX/house_D_v1_01.max.x");
	Load("./MapToolX/house_D_v2_01.max.x");
	m_vecXName.push_back("./MapToolX/house_D_v2_01.max.x");
	Load("./MapToolX/house_E_v1_01.max.x");
	m_vecXName.push_back("./MapToolX/house_E_v1_01.max.x");
	Load("./MapToolX/house_E_v1_02.max.x");
	m_vecXName.push_back("./MapToolX/house_E_v1_02.max.x");
	Load("./MapToolX/house_F_v1_01.max.x");
	m_vecXName.push_back("./MapToolX/house_F_v1_01.max.x");
	Load("./MapToolX/house_F_v1_02.max.x");
	m_vecXName.push_back("./MapToolX/house_F_v1_02.max.x");
	Load("./MapToolX/lantern_A_01.max.x");
	m_vecXName.push_back("./MapToolX/lantern_A_01.max.x");
	Load("./MapToolX/lantern_A_02.max.x");
	m_vecXName.push_back("./MapToolX/lantern_A_02.max.x");
	Load("./MapToolX/lantern_B_01.max.x");
	m_vecXName.push_back("./MapToolX/lantern_B_01.max.x");
	Load("./MapToolX/lantern_B_02.max.x");
	m_vecXName.push_back("./MapToolX/lantern_B_02.max.x");
	Load("./MapToolX/pillar_A_01.max.x");
	m_vecXName.push_back("./MapToolX/pillar_A_01.max.x");
	Load("./MapToolX/pillar_B_01.max.x");
	m_vecXName.push_back("./MapToolX/pillar_B_01.max.x");
	Load("./MapToolX/pine_tree_A_01.max.x");
	m_vecXName.push_back("./MapToolX/pine_tree_A_01.max.x");
	Load("./MapToolX/pine_tree_B_01.max.x");
	m_vecXName.push_back("./MapToolX/pine_tree_B_01.max.x");
	Load("./MapToolX/pine_tree_C_01.max.x");
	m_vecXName.push_back("./MapToolX/pine_tree_C_01.max.x");
	Load("./MapToolX/Ratusha.max.x");
	m_vecXName.push_back("./MapToolX/Ratusha.max.x");
	Load("./MapToolX/swing_01.max.x");
	m_vecXName.push_back("./MapToolX/swing_01.max.x");
	Load("./MapToolX/swing_02.max.x");
	m_vecXName.push_back("./MapToolX/swing_02.max.x");
	Load("./MapToolX/Swings_A_01.max.x");
	m_vecXName.push_back("./MapToolX/Swings_A_01.max.x");
	Load("./MapToolX/Swings_B_01.max.x");
	m_vecXName.push_back("./MapToolX/Swings_B_01.max.x");
	Load("./MapToolX/Swings_B_02.max.x");
	m_vecXName.push_back("./MapToolX/Swings_B_02.max.x");
	Load("./MapToolX/tree_broken_01.max.x");
	m_vecXName.push_back("./MapToolX/tree_broken_01.max.x");
	Load("./MapToolX/tree_old_01.max.x");
	m_vecXName.push_back("./MapToolX/tree_old_01.max.x");
	Load("./MapToolX/wc_A_01.max.x");
	m_vecXName.push_back("./MapToolX/wc_A_01.max.x");
	Load("./MapToolX/Xmas-tree.max.x");
	m_vecXName.push_back("./MapToolX/Xmas-tree.max.x");
	Load("./MapToolX/Xmas-tree01.max.x");
	m_vecXName.push_back("./MapToolX/Xmas-tree01.max.x");
}

void cMapToolInUI::Load(char* lpstrFile)
{
	ID3DXBuffer* adjBuffer = 0;
	ID3DXBuffer* mtrlBuffer = 0;
	DWORD		numMtrls = 0;
	LPD3DXMESH	pMesh = 0;

	D3DXLoadMeshFromX(
		lpstrFile,
		D3DXMESH_MANAGED,
		g_pD3DDevice,
		&adjBuffer,
		&mtrlBuffer,
		0,
		&numMtrls,
		&pMesh);

	//X파일로 부터 얻은 메쉬 값을 담는다.
	m_vecMesh.push_back(pMesh);

	//x파일 내에 오브젝트가 여러개인경우 다 그리기 위해서 갯수를 받는다.
	m_vecMtlNum.push_back(numMtrls);

	//텍스쳐 이름을 담고, 텍스쳐매니저에 추가한다.
	D3DXMATERIAL* mtrls = (D3DXMATERIAL*)mtrlBuffer->GetBufferPointer();

	for (int i = 0; i < numMtrls; i++)
	{
		string temp(mtrls[i].pTextureFilename);
		string FullPath("./MapToolX/"); //이곳에서 폴더명 변경
		FullPath += mtrls[i].pTextureFilename;

		//텍스쳐 추가
		g_pTextureManager->AddTexture(FullPath, FullPath, NULL);

		//텍스쳐 이름 담기
		m_vecTextureName.push_back(FullPath);
		//매터리얼 담기
		m_vecMaterial.push_back(mtrls[i].MatD3D);
	}
}

void cMapToolInUI::Update(D3DXVECTOR3* pos /*= NULL*/)
{
	if (m_pUIRoot)
		m_pUIRoot->Update();

	//버튼 false
	m_IsLClick = false;
	m_IsRClick = false;
}

void cMapToolInUI::Render()
{
	g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);		// 알파 블렌드 옵션 킨다.
	g_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);		// Z버퍼(깊이 버퍼)에 의해서 그리는 순서가 정해지는 옵션을 끈다. : 반투명을 표현할 때 속도 향상 효과
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, false);

	if (m_pUIRoot)
		m_pUIRoot->Render(m_pSprite);

	//원본 매트리스 저장
	D3DXMATRIXA16 recoverymatWorld;
	g_pD3DDevice->GetTransform(D3DTS_WORLD, &recoverymatWorld);
	D3DXMATRIXA16 recoverymatView;
	g_pD3DDevice->GetTransform(D3DTS_VIEW, &recoverymatView);

	//뷰포트로 그리기 위한 작업
	D3DXMATRIXA16 matView;
	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 matS;
	D3DXMATRIXA16 matT;

	//View매트리스 설정
	D3DXMatrixIdentity(&matView);
	g_pD3DDevice->SetTransform(D3DTS_VIEW, &matView);

	//메쉬가 있는 경우에만 그린다.
	if (m_vecMesh.size() > 0)
	{
		//메쉬 사이즈만큼
		for (int i = 0; i < m_vecMesh.size(); i++)
		{
			//매터리얼이 1개인 경우
			if (m_vecMtlNum[m_XIndex] - 1 == 0)
			{
				//계산후 월드 매트리스 설정
				BoundingBox    boundingBox;
				ComputeBoundingBox(m_vecMesh[m_XIndex], &boundingBox);

				if (boundingBox._max.x > 7.5f)
				{
					D3DXMatrixIdentity(&matS);
					D3DXMatrixScaling(&matS, 0.03f, 0.03f, 0.03f);
				}
				else if (boundingBox._max.x > 5.0f)
				{
					D3DXMatrixIdentity(&matS);
					D3DXMatrixScaling(&matS, 0.05f, 0.05f, 0.05f);
				}
				else if (boundingBox._max.x > 2.5f)
				{
					D3DXMatrixIdentity(&matS);
					D3DXMatrixScaling(&matS, 0.075f, 0.075f, 0.075f);
				}
				else if (boundingBox._max.x > 1.0f)
				{
					D3DXMatrixIdentity(&matS);
					D3DXMatrixScaling(&matS, 0.09f, 0.09f, 0.09f);
				}
				else
				{
					D3DXMatrixIdentity(&matS);
					D3DXMatrixScaling(&matS, 0.2f, 0.2f, 0.2f);
				}

				D3DXMatrixIdentity(&matT);
				D3DXMatrixTranslation(&matT, 3.25f, -1.7f, 5.0f);
				matWorld = matS * matT;
				g_pD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);

				g_pD3DDevice->SetMaterial(&m_vecMaterial[m_XIndex]);
				g_pD3DDevice->SetTexture(0, g_pTextureManager->GetTexture(m_vecTextureName[m_XIndex]));
				m_vecMesh[m_XIndex]->DrawSubset(0);
			}
			//매터리얼이 여러개인 경우
			else
			{
				for (int j = 0; j < m_vecMtlNum[i]; j++)
				{
					//계산후 월드 매트리스 설정
					BoundingBox    boundingBox;
					ComputeBoundingBox(m_vecMesh[m_XIndex], &boundingBox);

					if (boundingBox._max.x > 7.5f)
					{
						D3DXMatrixIdentity(&matS);
						D3DXMatrixScaling(&matS, 0.03f, 0.03f, 0.03f);
					}
					else if (boundingBox._max.x > 5.0f)
					{
						D3DXMatrixIdentity(&matS);
						D3DXMatrixScaling(&matS, 0.05f, 0.05f, 0.05f);
					}
					else if (boundingBox._max.x > 2.5f)
					{
						D3DXMatrixIdentity(&matS);
						D3DXMatrixScaling(&matS, 0.075f, 0.075f, 0.075f);
					}
					else if (boundingBox._max.x > 1.0f)
					{
						D3DXMatrixIdentity(&matS);
						D3DXMatrixScaling(&matS, 0.09f, 0.09f, 0.09f);
					}
					else
					{
						D3DXMatrixIdentity(&matS);
						D3DXMatrixScaling(&matS, 0.2f, 0.2f, 0.2f);
					}

					D3DXMatrixIdentity(&matT);
					D3DXMatrixTranslation(&matT, 3.25f, -1.7f, 5.0f);
					matWorld = matS * matT;
					g_pD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);

					g_pD3DDevice->SetMaterial(&m_vecMaterial[j]);
					g_pD3DDevice->SetTexture(0, g_pTextureManager->GetTexture(m_vecTextureName[j]));
					m_vecMesh[m_XIndex]->DrawSubset(j);
				}
			}
		}
	}

	g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);	// 기본 값 복구
	g_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);			// 기본 값 복구
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, false);
	//원본 매트리스로 다시 복구
	g_pD3DDevice->SetTransform(D3DTS_VIEW, &recoverymatView);
	g_pD3DDevice->SetTransform(D3DTS_WORLD, &recoverymatWorld);
}

void cMapToolInUI::OnClick(cUIButton* pSender)
{
	cUITextView* pTextView = (cUITextView*)m_pUIRoot->GetChildByTag(E_TEXTVIEW);
	cUITextView* pFolderFileTextView = (cUITextView*)m_pUIRoot->GetChildByTag(E_FolderFileTEXTVIEW);

	if (pSender->GetTag() == E_BUTTON1)
	{
		m_XIndex--;
		m_IsLClick = true;
	}
	else if (pSender->GetTag() == E_BUTTON2)
	{
		m_XIndex++;
		m_IsRClick = true;
	}

	if (m_XIndex < 0)
		m_XIndex = m_vecMesh.size() - 1;
	else if (m_XIndex > m_vecMesh.size() - 1)
		m_XIndex = 0;

	if (g_pKeyManager->isToggleKey(VK_OEM_2))
		pFolderFileTextView->SetText(m_vecXName[m_XIndex]);
	else
		pFolderFileTextView->SetText("if you want see Folder+File Name, Press /");
}

bool cMapToolInUI::ComputeBoundingBox(ID3DXMesh* mesh, BoundingBox* box)
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

void cMapToolInUI::AddXFile(char* lpstrFile)
{
	bool isFind = false;

	//중복된 파일이 있는지 검색
	for (int i = 0; i < m_vecXName.size(); i++)
	{
		if (m_vecXName[i] == lpstrFile)
			isFind = true;
	}

	//없으면 추가
	if (!isFind)
	{
		//파일명 추가
		m_vecXName.push_back(lpstrFile);

		//X파일 추가
		Load(lpstrFile);
	}
}
