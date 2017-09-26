#include "stdafx.h"
#include "cMapTool.h"
#include "cPickingCamera.h"
#include "cToolCameraController.h"
#include "cTerrain.h"
#include "cChildTool.h"
#include "cXobjectLoader.h"
#include "cMapToolInUI.h"
#include "cSkyBox.h"


cMapTool::cMapTool()
	: m_pPickingCamera(NULL)
	, m_pToolCameraController(NULL)
	, m_pTerrain(NULL)
	, m_pChildTool(NULL)
	, m_pMapToolInUI(NULL)
	, m_pSkyBox(NULL)
	, m_pXLoadTimeCount(10)
	, m_lpstrFile("")//����·� �ʱ�ȭ (NULL)�� �ʱ�ȭ�ϸ� ����
	, m_fRotY(0)
{
}


cMapTool::~cMapTool()
{
}

HRESULT cMapTool::Init()
{
	m_pPickingCamera = new cPickingCamera;
	m_pPickingCamera->AutoRelease();
	m_pPickingCamera->Setup();

	m_pToolCameraController = new cToolCameraController;
	m_pToolCameraController->AutoRelease();

	m_pTerrain = new cTerrain;
	m_pTerrain->AutoRelease();
	m_pTerrain->Setup();

	m_pChildTool = new cChildTool;
	m_pChildTool->AutoRelease();
	m_pChildTool->Setup();

	m_pMapToolInUI = new cMapToolInUI;
	m_pMapToolInUI->AutoRelease();
	m_pMapToolInUI->Setup();

	//���� �׸��� ���õ� ������ Tool�� ���.
	m_pChildTool->SetVertexNum(m_pTerrain->GetVertexNum());
	m_pChildTool->SetTileNum(m_pTerrain->GetTileNum());
	m_pChildTool->SetVertexMinHeight(m_pTerrain->GetVertexMinHeight());
	m_pChildTool->SetVertexMaxHeight(m_pTerrain->GetVertexMaxHeight());
	m_pChildTool->SetFlatteningHeight(m_pTerrain->GetFlatteningHeight());
	m_pChildTool->SetTerrainSpeed(m_pTerrain->GetTerrainSpeed());

	//���� ����Ʈ ����
	D3DXVECTOR3 dir(1.0f, -1.0f, 0.0f);
	D3DXVec3Normalize(&dir, &dir);
	D3DXCOLOR c = WHITE;
	D3DLIGHT9 light = InitDirectionalLight(&dir, &c);
	g_pD3DDevice->SetLight(0, &light);
	g_pD3DDevice->LightEnable(0, true);

	//��ī�̹ڽ�
	m_pSkyBox = new cSkyBox;
	m_pSkyBox->init();
	m_pSkyBox->AutoRelease();

	return S_OK;
}

void cMapTool::Update()
{
	if (m_pToolCameraController)
		m_pToolCameraController->Update(m_pPickingCamera->GetRotX(), m_pPickingCamera->GetRotY());

	if (m_pPickingCamera)
		m_pPickingCamera->Update(m_pToolCameraController->GetPosition());

	if (m_pTerrain)
		m_pTerrain->Update();

	//UI
	if (m_pMapToolInUI)
		m_pMapToolInUI->Update();

	if (m_pTerrain)
	{
		if (m_pChildTool)
			m_pTerrain->SetIsMod(m_pChildTool->GetIsMod()); //Mod ����ȭ

		//X���� ���߽��� ������ ���� ī���ͷ� ����
		m_pXLoadTimeCount += g_pTimeManager->getElapsedTime();

		if (m_pXLoadTimeCount > 10) //Ȥ�� �� ������ �����ϱ� ���� ī���� ����
			m_pXLoadTimeCount = 10;

		if (m_pChildTool->GetIsXFile())
		{
			//�Է� ���ڸ� ���� ���� �̸��� �Է� �ޱ�
			OPENFILENAME OFN;
			vector<D3DXVECTOR2> temp;
			char str[300];
			char lpstrFile[MAX_PATH] = "";
			memset(&OFN, 0, sizeof(OPENFILENAME));

			OFN.lStructSize = sizeof(OPENFILENAME);
			OFN.hwndOwner = g_hWnd;
			OFN.lpstrFilter = "Every File(*.*)\0*.*\0image File\0*.jpg;*.png\0";
			OFN.lpstrFile = lpstrFile;
			OFN.nMaxFile = 256;
			OFN.lpstrInitialDir = "./";
			OFN.Flags = OFN_NOCHANGEDIR;
			if (GetOpenFileName(&OFN) != 0)
			{
				//X���ϸ� ����
				m_lpstrFile = lpstrFile;

				//�̸����� X���� �߰�
				m_pMapToolInUI->AddXFile(lpstrFile);
				//�߰� �� ������ �ε����� �����ؼ� �̸����� ������ �ֱ�.
				m_pMapToolInUI->SetXIndex(m_pMapToolInUI->GetvecXName().size() - 1);

				wsprintf(str, "%s ������ ���������� �ҷ��Խ��ϴ�.", OFN.lpstrFile);
				MessageBox(g_hWnd, str, "���� �ҷ�����", MB_OK);
			}
			else {
				wsprintf(str, "���� �ҷ����Ⱑ ��� �Ǿ����ϴ�.", OFN.lpstrFile);
				MessageBox(g_hWnd, str, "���� �ҷ�����", MB_OK);
			}
		}

		if (m_pTerrain->GetIsfastCheck() && m_pChildTool->GetIsMod()) //Mod�� true�϶�
		{
			//X������ �ε�
			if (m_pXLoadTimeCount > 0.1f) //���� ������ �ð����� Ŀ����
			{
				if (/*m_lpstrFile != ""*/ m_pMapToolInUI->GetvecXName().size() > 0) //�̸��� ����°� �ƴ϶��
				{
					//������� ���콺 ����
					RECT winRC = { WINSTARTX, WINSTARTY, WINSIZEX, WINSIZEY };
					POINT pt;
					GetCursorPos(&pt);

					//XLoader ����
					cXobjectLoader*	m_XLoader = new cXobjectLoader;
					m_XLoader->SetInitScaling(1, 1, 1);
					//���� ��ǥ ����
					m_XLoader->SetInitPosition(
						m_pTerrain->GetPNTVertex()[m_pTerrain->GetDWORDDest()].p.x,
						m_pTerrain->GetPNTVertex()[m_pTerrain->GetDWORDDest()].p.y,
						m_pTerrain->GetPNTVertex()[m_pTerrain->GetDWORDDest()].p.z);

					//Ŭ���ϸ�, APIâ ���� �� ����.
					if (m_pTerrain->GetIsPicked() && PtInRect(&winRC, pt) && g_DI_Mouse->PushLeft() && !m_pMapToolInUI->GetIsLClick() && !m_pMapToolInUI->GetIsRClick())
					{
						char str[300];
						int YESNO;
						wsprintf(str, "�浹ü�� ������ �����Ͻðڽ��ϱ�?");
						YESNO = MessageBox(g_hWnd, str, "���� �ҷ�����", MB_YESNO);

						if (YESNO == IDYES)
							m_XLoader->Load((char*)m_pMapToolInUI->GetvecXName()[m_pMapToolInUI->GetXIndex()].c_str(), true);
						else if (YESNO == IDNO)
							m_XLoader->Load((char*)m_pMapToolInUI->GetvecXName()[m_pMapToolInUI->GetXIndex()].c_str(), false);

						m_pTerrain->GetXLoader().push_back(m_XLoader);
					}
				}

				m_pXLoadTimeCount = 0;

				//���߽��� ������ ���� ��ŷ false
				m_pTerrain->SetIsPicked(false);
				m_pTerrain->SetIsfastCheck(false);
			}
		}

		//���߽��� ������ ���� false
		m_pChildTool->SetIsXFile(false);

		//Z������ X���Ϻ����� �� �ε������� ������ �� �����Ҵ� ����
		if (g_pKeyManager->isOnceKeyDown('Z'))
		{
			if (m_pTerrain->GetXLoader().size() > 0)
			{
				//������ �ε����� �����͸� �����
				m_pTerrain->GetXLoader().back()->Destroy();
				//������ �ε����� ���Ϳ��� �����Ѵ�.
				m_pTerrain->GetXLoader().pop_back();
			}
		}

		//cTerrain���� PageUp, PageDownŰ�� �ε����� �������ֱ� ������, �̰����� �� Ű�� �������� ȸ������ �ʱ�ȭ ���Ѽ�,
		//���� �ε����� �Ѿ���� ���� �ε����� �Ȱ��� ȸ������ ���� ���ϰ� �Ѵ�.
		if (g_pKeyManager->isOnceKeyDown(VK_MULTIPLY))
			m_fRotY = 0;

		if (m_pTerrain->GetXLoader().size() > 0)
		{
			//���� X���� SRT ���� (RotY�� POS ������ ����)
			if (g_pKeyManager->isStayKeyDown(VK_ADD)) //���������� ȸ��
			{
				m_fRotY += 0.5f * g_pTimeManager->getElapsedTime();
				m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->SetRotationY(m_fRotY);

				//m_pTerrain->GetXLoader()->data()[m_pTerrain->GetXIndex()]->SetScaling(
				//	m_pTerrain->GetXLoader()->data()[m_pTerrain->GetXIndex()]->GetMeshScaling().x + 0.1f ,
				//	m_pTerrain->GetXLoader()->data()[m_pTerrain->GetXIndex()]->GetMeshScaling().y + 0.1f ,
				//	m_pTerrain->GetXLoader()->data()[m_pTerrain->GetXIndex()]->GetMeshScaling().z + 0.1f );
			}
			else if (g_pKeyManager->isStayKeyDown(VK_SUBTRACT)) //���������� ȸ��
			{
				m_fRotY -= 0.5 * g_pTimeManager->getElapsedTime();
				if (m_pTerrain->GetXLoader().size() > 0)
					m_pTerrain->GetXLoader().data()[m_pTerrain->GetXIndex()]->SetRotationY(m_fRotY);

				//m_pTerrain->GetXLoader()->data()[m_pTerrain->GetXIndex()]->SetScaling(
				//	m_pTerrain->GetXLoader()->data()[m_pTerrain->GetXIndex()]->GetMeshScaling().x - 0.1f ,
				//	m_pTerrain->GetXLoader()->data()[m_pTerrain->GetXIndex()]->GetMeshScaling().y - 0.1f ,
				//	m_pTerrain->GetXLoader()->data()[m_pTerrain->GetXIndex()]->GetMeshScaling().z - 0.1f );
			}
			else if (g_pKeyManager->isStayKeyDown(VK_NUMPAD8)) //���� �̵�
			{
				m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->SetPosition(
					m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->GetMeshPosition().x,
					m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->GetMeshPosition().y + 0.5f * g_pTimeManager->getElapsedTime(),
					m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->GetMeshPosition().z);
			}
			else if (g_pKeyManager->isStayKeyDown(VK_NUMPAD2)) //�Ʒ��� �̵�
			{
				m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->SetPosition(
					m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->GetMeshPosition().x,
					m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->GetMeshPosition().y - 0.5f * g_pTimeManager->getElapsedTime(),
					m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->GetMeshPosition().z);
			}
			else if (g_pKeyManager->isStayKeyDown(VK_NUMPAD4)) //�������� �̵�
			{
				m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->SetPosition(
					m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->GetMeshPosition().x - 0.5f * g_pTimeManager->getElapsedTime(),
					m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->GetMeshPosition().y,
					m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->GetMeshPosition().z);
			}
			else if (g_pKeyManager->isStayKeyDown(VK_NUMPAD6)) //���������� �̵�
			{
				m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->SetPosition(
					m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->GetMeshPosition().x + 0.5f * g_pTimeManager->getElapsedTime(),
					m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->GetMeshPosition().y,
					m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->GetMeshPosition().z);
			}
			else if (g_pKeyManager->isStayKeyDown(VK_NUMPAD1)) //Z-������ �̵�
			{
				m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->SetPosition(
					m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->GetMeshPosition().x,
					m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->GetMeshPosition().y,
					m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->GetMeshPosition().z - 0.5f * g_pTimeManager->getElapsedTime());
			}
			else if (g_pKeyManager->isStayKeyDown(VK_NUMPAD9)) //Z+�������� �̵�
			{
				m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->SetPosition(
					m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->GetMeshPosition().x,
					m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->GetMeshPosition().y,
					m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->GetMeshPosition().z + 0.5f * g_pTimeManager->getElapsedTime());
			}
		}
	}

	if (m_pChildTool)
	{
		m_pChildTool->Update();

		m_pTerrain->SetVertexMaxHeight(m_pChildTool->GetVertexMaxHeight());
		m_pTerrain->SetVertexMinHeight(m_pChildTool->GetVertexMinHeight());
		m_pTerrain->SetFlatteningHeight(m_pChildTool->GetFlatteningHeight());
		m_pTerrain->SetTerrainSpeed(m_pChildTool->GetTerrainSpeed());

		if (m_pChildTool->GetIsActivate()) //�ʱ�ȭ �� �缳�� ��ư
		{
			if (m_pTerrain->GetXLoader().size() > 0)
			{
				for (int i = m_pTerrain->GetXLoader().size(); i > 0; i--)//�������ε������� �����.
				{
					//������ �ε����� �����͸� �����
					m_pTerrain->GetXLoader().back()->Destroy();
					//������ �ε����� ���Ϳ��� �����Ѵ�.
					m_pTerrain->GetXLoader().pop_back();
				}
			}

			//���ؽ�, Ÿ�� ���� ���� �� �׸��� �缳��
			m_pTerrain->SetVertexNum(m_pChildTool->GetVertexNum());
			m_pTerrain->SetTileNum(m_pChildTool->GetTileNum());
			m_pTerrain->Setup();
			
			//���߽��� ������ ���� false
			m_pChildTool->SetIsActivate(false);
		}

		if (m_pChildTool->GetIsTexture())
		{
			//�Է� ���ڸ� ���� ���� �̸��� �Է� �ޱ�
			OPENFILENAME OFN;
			vector<D3DXVECTOR2> temp;
			char str[300];
			char lpstrFile[MAX_PATH] = "";
			memset(&OFN, 0, sizeof(OPENFILENAME));

			OFN.lStructSize = sizeof(OPENFILENAME);
			OFN.hwndOwner = g_hWnd;
			OFN.lpstrFilter = "Every File(*.*)\0*.*\0image File\0*.jpg;*.png\0";
			OFN.lpstrFile = lpstrFile;
			OFN.nMaxFile = 256;
			OFN.lpstrInitialDir = "./";
			OFN.Flags = OFN_NOCHANGEDIR;
			if (GetOpenFileName(&OFN) != 0)
			{
				//�ҷ����� ���Ϸ� �ؽ��� ����
				g_pTextureManager->AddTexture(lpstrFile, lpstrFile, m_pTerrain->GetTexture());
				m_pTerrain->SetTextureName(lpstrFile);

				wsprintf(str, "%s ������ ���������� �ҷ��Խ��ϴ�.", OFN.lpstrFile);
				MessageBox(g_hWnd, str, "���� �ҷ�����", MB_OK);
			}
			else {
				wsprintf(str, "���� �ҷ����Ⱑ ��� �Ǿ����ϴ�.", OFN.lpstrFile);
				MessageBox(g_hWnd, str, "���� �ҷ�����", MB_OK);
			}

			//���߽��� ������ ���� false
			m_pChildTool->SetIsTexture(false);
		}

		//�ε�
		if (g_pKeyManager->isOnceKeyDown(VK_F1) || m_pChildTool->GetIsLoad())
		{
			//�ͷ��� �ε�
			m_pTerrain->LoadAll();
			//������Ʈ �ε�
			LoadAll();

			//���߽��� ������ ���� false
			m_pChildTool->SetIsLoad(false);
		}
		//���̺�
		else if (g_pKeyManager->isOnceKeyDown(VK_F2) || m_pChildTool->GetIsSave())
		{
			//�ͷ��� ���̺�
			m_pTerrain->SaveAll();
			//������Ʈ ���̺�
			SaveAll();

			//���߽��� ������ ���� false
			m_pChildTool->SetIsSave(false);
		}
	}
}

void cMapTool::Render()
{
	//SetStreamSource �Ǿ� �ֱ� ������, SkyBox���� �׷����Ѵ�. (���� �ڷΰ��� UI ©��)
	if (m_pSkyBox)
		m_pSkyBox->render();

	if (m_pTerrain)
		m_pTerrain->Render();

	if (m_pChildTool)
		m_pChildTool->Render();

	if (m_pMapToolInUI)
		m_pMapToolInUI->Render();
}

void cMapTool::Release()
{

}

void cMapTool::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (m_pPickingCamera)
		m_pPickingCamera->WndProc(hWnd, message, wParam, lParam);

	if (m_pTerrain)
		m_pTerrain->WndProc(hWnd, message, wParam, lParam);

	if (m_pChildTool)
		m_pChildTool->WndProc(hWnd, message, wParam, lParam);
}

void cMapTool::LoadAll()
{
	/*
	vector<D3DXMATRIXA16>	m_vecMatW;
	vector<LPD3DXMESH>		m_vecMesh;
	vector<D3DMATERIAL9>	m_vecMaterial;
	vector<DWORD>			m_vecMtlNum;
	vector<string>			m_vecTextureName;
	vector<LPD3DXMESH>		m_vecCollision;
	vector<D3DXMATRIXA16>	m_vecCollisionMatW;
	*/

	FILE* fp;
	string fileName;
	//X ���� �ε�

	//X ���� �� ����(��ü ���� �ľ��� ���� ���̱⿡, �ѹ��� ���ָ��)
	fp = fopen("./save/XTotalNum", "r");
	char Temp[1024]; //���� �ӽ� �����.
	fgets(Temp, sizeof(Temp), fp);
	int nXTotalNum = atoi(Temp);
	fclose(fp);

	fp = fopen("./save/XTotalSize", "r");
	fgets(Temp, sizeof(Temp), fp);
	int nXTotalSize = atoi(Temp);
	fclose(fp);

	if (nXTotalNum > 0) // �����ϴ� ��쿡��
	{
		//���� ������ ����
		for (int i = m_pTerrain->GetXLoader().size(); i > 0; i--)
		{
			m_pTerrain->GetXLoader().back()->Destroy();
			m_pTerrain->GetXLoader().pop_back();
		}

		//�޸� �籸��
		//m_pTerrain->GetXLoader().resize(nXTotalNum * sizeof(cXobjectLoader*));
		//m_pTerrain->GetXLoader().resize(nXTotalSize);
		//m_pTerrain->GetXLoader().resize(nXTotalNum);

		for (int i = 0; i < nXTotalNum; i++)
		{
			//�浹ü ����
			fileName = "./save/XisSphere";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "r");
			fgets(Temp, sizeof(Temp), fp);
			bool bisSphere = atoi(Temp);
			fclose(fp);

			//���� �̸�
			fileName = "./save/XFileName";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "r");
			fgets(Temp, sizeof(Temp), fp);
			fclose(fp);

			cXobjectLoader* temp = new cXobjectLoader;
			temp->Load(Temp, bisSphere);
			m_pTerrain->GetXLoader().push_back(temp);

			//�Ž� ��Ʈ����
			fileName = "./save/XMatW";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "rb");
			while (!feof(fp))
			{
				D3DXMATRIXA16 MatW;
				fread(&MatW, sizeof(D3DXMATRIXA16), 1, fp);

				if (MatW != '\0')
					m_pTerrain->GetXLoader()[i]->SetMeshMatrix(MatW);
			}
			fclose(fp);

			//�Ž� �浹ü ��Ʈ����
			fileName = "./save/XCollisionMatW";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "rb");
			while (!feof(fp))
			{
				D3DXMATRIXA16 CollisionMatW;
				fread(&CollisionMatW, sizeof(D3DXMATRIXA16), 1, fp);

				if (CollisionMatW != '\0')
					m_pTerrain->GetXLoader()[i]->SetCollisionMatrix(CollisionMatW);
			}
			fclose(fp);


			//�� �ҽ� ���� ���� �ε� ����(�̿ϼ� & �Ƹ� �̹���� �ȵɵ�)
			/*//�Ž�
			fileName = "./save/XMesh";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "rb");
			while (!feof(fp))
			{
				LPD3DXMESH mesh;
				fread(&mesh, sizeof(LPD3DXMESH), 1, fp);
				if (mesh != '\0')
					m_pTerrain->GetXLoader()[i]->GetvecMesh().push_back(mesh);
			}
			fclose(fp);

			//���͸���
			fileName = "./save/XMaterial";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "rb");
			while (!feof(fp))
			{
				D3DMATERIAL9 mtrl;
				fread(&mtrl, sizeof(D3DMATERIAL9), 1, fp);
				//if (mtrl != '\0')
					m_pTerrain->GetXLoader()[i]->GetvecMaterial().push_back(mtrl);
			}
			fclose(fp);

			//���͸��� ����
			fileName = "./save/XMtlNum";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "rb");
			while (!feof(fp))
			{
				DWORD mtrlNum;
				fread(&mtrlNum, sizeof(DWORD), 1, fp);
				if (mtrlNum != '\0')
					m_pTerrain->GetXLoader()[i]->GetvecMtlNum().push_back(mtrlNum);
			}
			fclose(fp);

			//�ؽ��� �̸�
			fileName = "./save/XTextureName";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "rb");
			while (!feof(fp))
			{
				string TextureName;
				fread(&TextureName, sizeof(string), 1, fp);
				if (TextureName != "\0")
					m_pTerrain->GetXLoader()[i]->GetvecTextureName().push_back(TextureName);
			}
			fclose(fp);

			//�浹ü �Ž�
			fileName = "./save/XCollision";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "rb");
			while (!feof(fp))
			{
				LPD3DXMESH Collision;
				fread(&Collision, sizeof(LPD3DXMESH), 1, fp);
				if (Collision != '\0')
					m_pTerrain->GetXLoader()[i]->GetvecCollision().push_back(Collision);
			}
			fclose(fp);

			//�浹ü ��Ʈ����
			fileName = "./save/XCollisionMatW";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "rb");
			while (!feof(fp))
			{
				D3DXMATRIXA16 CollisionMatW;
				fread(&CollisionMatW, sizeof(D3DXMATRIXA16), 1, fp);
				if (CollisionMatW != '\0')
					m_pTerrain->GetXLoader()[i]->GetvecCollisionMatW().push_back(CollisionMatW);
			}
			fclose(fp);*/
		}
	}
}

void cMapTool::SaveAll()
{
	/*
	vector<D3DXMATRIXA16>	m_vecMatW;
	vector<LPD3DXMESH>		m_vecMesh;
	vector<D3DMATERIAL9>	m_vecMaterial;
	vector<DWORD>			m_vecMtlNum;
	vector<string>			m_vecTextureName;
	vector<LPD3DXMESH>		m_vecCollision;
	vector<D3DXMATRIXA16>	m_vecCollisionMatW;
	*/

	FILE* fp;
	string fileName;
	//X ���� ����
	if (m_pTerrain->GetXLoader().size() > 0) // (X������ �����ϴ� ��츸)
	{
		for (int i = 0; i < m_pTerrain->GetXLoader().size(); i++)
		{
			//���ϱ� ���� ����
			cXobjectLoader* Xobj = m_pTerrain->GetXLoader()[i];

			//ó�� �ѹ��� �ϸ��. (��ü ������ �޴� ���̱� ������)
			if (i == 0)
			{
				//X ���� �� ����
				fp = fopen("./save/XTotalNum", "w");
				char temp[1024];
				itoa(m_pTerrain->GetXLoader().size(), temp, 10);
				fputs(temp, fp);
				fclose(fp);

				//XLoader�� size
				fp = fopen("./save/XTotalSize", "w");
				itoa(sizeof(m_pTerrain->GetXLoader()), temp, 10);
				fputs(temp, fp);
				fclose(fp);
			}

			//�Ž� ��Ʈ����
			vector<D3DXMATRIXA16> vMatW;
			fileName = "./save/XMatW";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "wb");

			vMatW.push_back(Xobj->GetvecMatW()[0]);

			fwrite(&vMatW[0], vMatW.size() * sizeof(D3DXMATRIXA16), 1, fp); //�߿�!! ������ * sizeof(Ÿ��)
			fclose(fp);

			//�浹ü ��Ʈ����
			vector<D3DXMATRIXA16> vCollisionMatW;
			fileName = "./save/XCollisionMatW";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "wb");

			vCollisionMatW.push_back(Xobj->GetvecCollisionMatW()[0]);

			fwrite(&vCollisionMatW[0], vCollisionMatW.size() * sizeof(D3DXMATRIXA16), 1, fp); //�߿�!! ������ * sizeof(Ÿ��)
			fclose(fp);

			//���� �̸�
			fileName = "./save/XFileName";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "w");
			fputs(Xobj->GetvecFileName()[0].c_str(), fp);
			fclose(fp);

			//�浹ü ����
			fileName = "./save/XisSphere";
			fileName += to_string(i);
			char temp[1024];
			fp = fopen(fileName.c_str(), "w");
			itoa(Xobj->GetIsSphere(), temp, 10);
			fputs(temp, fp);
			fclose(fp);

			//���� ���� �۾� �����൵ ��. (���� �̸��� ��Ʈ������ �����ؼ� �ε��Ҷ� �����̸����� ���� �ε� �� ��Ʈ������ �����Ѵ�.)
			/*
			//�Ž�
			vector<LPD3DXMESH> vMesh;
			fileName = "./save/XMesh";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "wb");

			vMesh.push_back(Xobj->GetvecMesh()[0]);

			fwrite(&vMesh[0], vMesh.size() * sizeof(LPD3DXMESH), 1, fp); //�߿�!! ������ * sizeof(Ÿ��)
			fclose(fp);

			//���͸���
			vector<D3DMATERIAL9> vMaterial;
			fileName = "./save/XMaterial";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "wb");

			if (Xobj->GetvecMaterial().size() > 1)
			{
				for (int j = 0; j < Xobj->GetvecMtlNum()[0]; j++)
					vMaterial.push_back(Xobj->GetvecMaterial()[j]);
			}
			else
			{
				vMaterial.push_back(Xobj->GetvecMaterial()[0]);
			}

			fwrite(&vMaterial[0], vMaterial.size() * sizeof(D3DMATERIAL9), 1, fp); //�߿�!! ������ * sizeof(Ÿ��)
			fclose(fp);

			//���͸��� ����
			vector<DWORD> vMtlNum;
			fileName = "./save/XMtlNum";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "wb");

			vMtlNum.push_back(Xobj->GetvecMtlNum()[0]);

			fwrite(&vMtlNum[0], vMtlNum.size() * sizeof(DWORD), 1, fp); //�߿�!! ������ * sizeof(Ÿ��)
			fclose(fp);

			//�ؽ��� �̸�
			vector<string> vTextureName;
			fileName = "./save/XTextureName";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "wb");

			if (Xobj->GetvecTextureName().size() > 1)
			{
				for (int j = 0; j < Xobj->GetvecMtlNum()[0]; j++)
					vTextureName.push_back(Xobj->GetvecTextureName()[j]);
			}
			else
			{
				vTextureName.push_back(Xobj->GetvecTextureName()[0]);
			}

			fwrite(&vTextureName[0], vTextureName.size() * sizeof(string), 1, fp); //�߿�!! ������ * sizeof(Ÿ��)
			fclose(fp);

			//�浹ü �Ž�
			vector<LPD3DXMESH> vCollision;
			fileName = "./save/XCollision";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "wb");

			vCollision.push_back(Xobj->GetvecCollision()[0]);

			fwrite(&vCollision[0], vCollision.size() * sizeof(LPD3DXMESH), 1, fp); //�߿�!! ������ * sizeof(Ÿ��)
			fclose(fp);
			*/
		}
	}
}
