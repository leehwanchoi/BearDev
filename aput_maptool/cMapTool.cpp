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
	, m_lpstrFile("")//빈상태로 초기화 (NULL)로 초기화하면 터짐
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

	//최초 그리드 관련된 정보를 Tool에 담기.
	m_pChildTool->SetVertexNum(m_pTerrain->GetVertexNum());
	m_pChildTool->SetTileNum(m_pTerrain->GetTileNum());
	m_pChildTool->SetVertexMinHeight(m_pTerrain->GetVertexMinHeight());
	m_pChildTool->SetVertexMaxHeight(m_pTerrain->GetVertexMaxHeight());
	m_pChildTool->SetFlatteningHeight(m_pTerrain->GetFlatteningHeight());
	m_pChildTool->SetTerrainSpeed(m_pTerrain->GetTerrainSpeed());

	//최초 라이트 설정
	D3DXVECTOR3 dir(1.0f, -1.0f, 0.0f);
	D3DXVec3Normalize(&dir, &dir);
	D3DXCOLOR c = WHITE;
	D3DLIGHT9 light = InitDirectionalLight(&dir, &c);
	g_pD3DDevice->SetLight(0, &light);
	g_pD3DDevice->LightEnable(0, true);

	//스카이박스
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
			m_pTerrain->SetIsMod(m_pChildTool->GetIsMod()); //Mod 동기화

		//X파일 다중실행 방지를 위해 카운터로 제어
		m_pXLoadTimeCount += g_pTimeManager->getElapsedTime();

		if (m_pXLoadTimeCount > 10) //혹시 모를 터짐을 방지하기 위해 카운터 제한
			m_pXLoadTimeCount = 10;

		if (m_pChildTool->GetIsXFile())
		{
			//입력 상자를 통해 파일 이름을 입력 받기
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
				//X파일명 저장
				m_lpstrFile = lpstrFile;

				//미리보기 X파일 추가
				m_pMapToolInUI->AddXFile(lpstrFile);
				//추가 후 마지막 인덱스로 변경해서 미리보기 갱신해 주기.
				m_pMapToolInUI->SetXIndex(m_pMapToolInUI->GetvecXName().size() - 1);

				wsprintf(str, "%s 파일을 성공적으로 불러왔습니다.", OFN.lpstrFile);
				MessageBox(g_hWnd, str, "파일 불러오기", MB_OK);
			}
			else {
				wsprintf(str, "파일 불러오기가 취소 되었습니다.", OFN.lpstrFile);
				MessageBox(g_hWnd, str, "파일 불러오기", MB_OK);
			}
		}

		if (m_pTerrain->GetIsfastCheck() && m_pChildTool->GetIsMod()) //Mod가 true일때
		{
			//X파일을 로드
			if (m_pXLoadTimeCount > 0.1f) //지정 딜레이 시간보다 커지면
			{
				if (/*m_lpstrFile != ""*/ m_pMapToolInUI->GetvecXName().size() > 0) //이름이 빈상태가 아니라면
				{
					//윈도우와 마우스 갱신
					RECT winRC = { WINSTARTX, WINSTARTY, WINSIZEX, WINSIZEY };
					POINT pt;
					GetCursorPos(&pt);

					//XLoader 생성
					cXobjectLoader*	m_XLoader = new cXobjectLoader;
					m_XLoader->SetInitScaling(1, 1, 1);
					//생성 좌표 설정
					m_XLoader->SetInitPosition(
						m_pTerrain->GetPNTVertex()[m_pTerrain->GetDWORDDest()].p.x,
						m_pTerrain->GetPNTVertex()[m_pTerrain->GetDWORDDest()].p.y,
						m_pTerrain->GetPNTVertex()[m_pTerrain->GetDWORDDest()].p.z);

					//클릭하면, API창 실행 후 생성.
					if (m_pTerrain->GetIsPicked() && PtInRect(&winRC, pt) && g_DI_Mouse->PushLeft() && !m_pMapToolInUI->GetIsLClick() && !m_pMapToolInUI->GetIsRClick())
					{
						char str[300];
						int YESNO;
						wsprintf(str, "충돌체를 원으로 설정하시겠습니까?");
						YESNO = MessageBox(g_hWnd, str, "파일 불러오기", MB_YESNO);

						if (YESNO == IDYES)
							m_XLoader->Load((char*)m_pMapToolInUI->GetvecXName()[m_pMapToolInUI->GetXIndex()].c_str(), true);
						else if (YESNO == IDNO)
							m_XLoader->Load((char*)m_pMapToolInUI->GetvecXName()[m_pMapToolInUI->GetXIndex()].c_str(), false);

						m_pTerrain->GetXLoader().push_back(m_XLoader);
					}
				}

				m_pXLoadTimeCount = 0;

				//다중실행 방지을 위해 픽킹 false
				m_pTerrain->SetIsPicked(false);
				m_pTerrain->SetIsfastCheck(false);
			}
		}

		//다중실행 방지을 위해 false
		m_pChildTool->SetIsXFile(false);

		//Z누르면 X파일벡터의 뒤 인덱스부터 데이터 및 동적할당 제거
		if (g_pKeyManager->isOnceKeyDown('Z'))
		{
			if (m_pTerrain->GetXLoader().size() > 0)
			{
				//마지막 인덱스의 데이터를 지우고
				m_pTerrain->GetXLoader().back()->Destroy();
				//마지막 인덱스를 벡터에서 제거한다.
				m_pTerrain->GetXLoader().pop_back();
			}
		}

		//cTerrain에서 PageUp, PageDown키로 인덱스를 변경해주기 때문에, 이곳에서 두 키를 눌렀을때 회전값을 초기화 시켜서,
		//다음 인덱스가 넘어갔을때 이전 인덱스와 똑같은 회전값을 갖지 못하게 한다.
		if (g_pKeyManager->isOnceKeyDown(VK_MULTIPLY))
			m_fRotY = 0;

		if (m_pTerrain->GetXLoader().size() > 0)
		{
			//현재 X파일 SRT 수정 (RotY와 POS 수정만 가능)
			if (g_pKeyManager->isStayKeyDown(VK_ADD)) //오른쪽으로 회전
			{
				m_fRotY += 0.5f * g_pTimeManager->getElapsedTime();
				m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->SetRotationY(m_fRotY);

				//m_pTerrain->GetXLoader()->data()[m_pTerrain->GetXIndex()]->SetScaling(
				//	m_pTerrain->GetXLoader()->data()[m_pTerrain->GetXIndex()]->GetMeshScaling().x + 0.1f ,
				//	m_pTerrain->GetXLoader()->data()[m_pTerrain->GetXIndex()]->GetMeshScaling().y + 0.1f ,
				//	m_pTerrain->GetXLoader()->data()[m_pTerrain->GetXIndex()]->GetMeshScaling().z + 0.1f );
			}
			else if (g_pKeyManager->isStayKeyDown(VK_SUBTRACT)) //오른쪽으로 회전
			{
				m_fRotY -= 0.5 * g_pTimeManager->getElapsedTime();
				if (m_pTerrain->GetXLoader().size() > 0)
					m_pTerrain->GetXLoader().data()[m_pTerrain->GetXIndex()]->SetRotationY(m_fRotY);

				//m_pTerrain->GetXLoader()->data()[m_pTerrain->GetXIndex()]->SetScaling(
				//	m_pTerrain->GetXLoader()->data()[m_pTerrain->GetXIndex()]->GetMeshScaling().x - 0.1f ,
				//	m_pTerrain->GetXLoader()->data()[m_pTerrain->GetXIndex()]->GetMeshScaling().y - 0.1f ,
				//	m_pTerrain->GetXLoader()->data()[m_pTerrain->GetXIndex()]->GetMeshScaling().z - 0.1f );
			}
			else if (g_pKeyManager->isStayKeyDown(VK_NUMPAD8)) //위로 이동
			{
				m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->SetPosition(
					m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->GetMeshPosition().x,
					m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->GetMeshPosition().y + 0.5f * g_pTimeManager->getElapsedTime(),
					m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->GetMeshPosition().z);
			}
			else if (g_pKeyManager->isStayKeyDown(VK_NUMPAD2)) //아래로 이동
			{
				m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->SetPosition(
					m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->GetMeshPosition().x,
					m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->GetMeshPosition().y - 0.5f * g_pTimeManager->getElapsedTime(),
					m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->GetMeshPosition().z);
			}
			else if (g_pKeyManager->isStayKeyDown(VK_NUMPAD4)) //왼쪽으로 이동
			{
				m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->SetPosition(
					m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->GetMeshPosition().x - 0.5f * g_pTimeManager->getElapsedTime(),
					m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->GetMeshPosition().y,
					m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->GetMeshPosition().z);
			}
			else if (g_pKeyManager->isStayKeyDown(VK_NUMPAD6)) //오른쪽으로 이동
			{
				m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->SetPosition(
					m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->GetMeshPosition().x + 0.5f * g_pTimeManager->getElapsedTime(),
					m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->GetMeshPosition().y,
					m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->GetMeshPosition().z);
			}
			else if (g_pKeyManager->isStayKeyDown(VK_NUMPAD1)) //Z-쪽으로 이동
			{
				m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->SetPosition(
					m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->GetMeshPosition().x,
					m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->GetMeshPosition().y,
					m_pTerrain->GetXLoader()[m_pTerrain->GetXIndex()]->GetMeshPosition().z - 0.5f * g_pTimeManager->getElapsedTime());
			}
			else if (g_pKeyManager->isStayKeyDown(VK_NUMPAD9)) //Z+왼쪽으로 이동
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

		if (m_pChildTool->GetIsActivate()) //초기화 및 재설정 버튼
		{
			if (m_pTerrain->GetXLoader().size() > 0)
			{
				for (int i = m_pTerrain->GetXLoader().size(); i > 0; i--)//마지막인덱스부터 지운다.
				{
					//마지막 인덱스의 데이터를 지우고
					m_pTerrain->GetXLoader().back()->Destroy();
					//마지막 인덱스를 벡터에서 제거한다.
					m_pTerrain->GetXLoader().pop_back();
				}
			}

			//버텍스, 타일 숫자 설정 후 그리드 재설정
			m_pTerrain->SetVertexNum(m_pChildTool->GetVertexNum());
			m_pTerrain->SetTileNum(m_pChildTool->GetTileNum());
			m_pTerrain->Setup();
			
			//다중실행 방지을 위해 false
			m_pChildTool->SetIsActivate(false);
		}

		if (m_pChildTool->GetIsTexture())
		{
			//입력 상자를 통해 파일 이름을 입력 받기
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
				//불러들인 파일로 텍스쳐 설정
				g_pTextureManager->AddTexture(lpstrFile, lpstrFile, m_pTerrain->GetTexture());
				m_pTerrain->SetTextureName(lpstrFile);

				wsprintf(str, "%s 파일을 성공적으로 불러왔습니다.", OFN.lpstrFile);
				MessageBox(g_hWnd, str, "파일 불러오기", MB_OK);
			}
			else {
				wsprintf(str, "파일 불러오기가 취소 되었습니다.", OFN.lpstrFile);
				MessageBox(g_hWnd, str, "파일 불러오기", MB_OK);
			}

			//다중실행 방지을 위해 false
			m_pChildTool->SetIsTexture(false);
		}

		//로드
		if (g_pKeyManager->isOnceKeyDown(VK_F1) || m_pChildTool->GetIsLoad())
		{
			//터레인 로드
			m_pTerrain->LoadAll();
			//오브젝트 로드
			LoadAll();

			//다중실행 방지을 위해 false
			m_pChildTool->SetIsLoad(false);
		}
		//세이브
		else if (g_pKeyManager->isOnceKeyDown(VK_F2) || m_pChildTool->GetIsSave())
		{
			//터레인 세이브
			m_pTerrain->SaveAll();
			//오브젝트 세이브
			SaveAll();

			//다중실행 방지을 위해 false
			m_pChildTool->SetIsSave(false);
		}
	}
}

void cMapTool::Render()
{
	//SetStreamSource 되어 있기 때문에, SkyBox부터 그려야한다. (순서 뒤로가면 UI 짤림)
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
	//X 파일 로드

	//X 파일 총 갯수(전체 갯수 파악을 위한 것이기에, 한번만 해주면됨)
	fp = fopen("./save/XTotalNum", "r");
	char Temp[1024]; //담을 임시 저장소.
	fgets(Temp, sizeof(Temp), fp);
	int nXTotalNum = atoi(Temp);
	fclose(fp);

	fp = fopen("./save/XTotalSize", "r");
	fgets(Temp, sizeof(Temp), fp);
	int nXTotalSize = atoi(Temp);
	fclose(fp);

	if (nXTotalNum > 0) // 존재하는 경우에만
	{
		//기존 데이터 제거
		for (int i = m_pTerrain->GetXLoader().size(); i > 0; i--)
		{
			m_pTerrain->GetXLoader().back()->Destroy();
			m_pTerrain->GetXLoader().pop_back();
		}

		//메모리 재구성
		//m_pTerrain->GetXLoader().resize(nXTotalNum * sizeof(cXobjectLoader*));
		//m_pTerrain->GetXLoader().resize(nXTotalSize);
		//m_pTerrain->GetXLoader().resize(nXTotalNum);

		for (int i = 0; i < nXTotalNum; i++)
		{
			//충돌체 구분
			fileName = "./save/XisSphere";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "r");
			fgets(Temp, sizeof(Temp), fp);
			bool bisSphere = atoi(Temp);
			fclose(fp);

			//파일 이름
			fileName = "./save/XFileName";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "r");
			fgets(Temp, sizeof(Temp), fp);
			fclose(fp);

			cXobjectLoader* temp = new cXobjectLoader;
			temp->Load(Temp, bisSphere);
			m_pTerrain->GetXLoader().push_back(temp);

			//매쉬 메트리스
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

			//매쉬 충돌체 메트리스
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


			//위 소스 생성 이전 로드 버젼(미완성 & 아마 이방식은 안될듯)
			/*//매쉬
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

			//매터리얼
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

			//매터리얼 갯수
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

			//텍스쳐 이름
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

			//충돌체 매쉬
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

			//충돌체 매트리스
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
	//X 파일 저장
	if (m_pTerrain->GetXLoader().size() > 0) // (X파일이 존재하는 경우만)
	{
		for (int i = 0; i < m_pTerrain->GetXLoader().size(); i++)
		{
			//편하기 쓰기 위해
			cXobjectLoader* Xobj = m_pTerrain->GetXLoader()[i];

			//처음 한번만 하면됨. (전체 갯수를 받는 것이기 때문에)
			if (i == 0)
			{
				//X 파일 총 갯수
				fp = fopen("./save/XTotalNum", "w");
				char temp[1024];
				itoa(m_pTerrain->GetXLoader().size(), temp, 10);
				fputs(temp, fp);
				fclose(fp);

				//XLoader의 size
				fp = fopen("./save/XTotalSize", "w");
				itoa(sizeof(m_pTerrain->GetXLoader()), temp, 10);
				fputs(temp, fp);
				fclose(fp);
			}

			//매쉬 매트리스
			vector<D3DXMATRIXA16> vMatW;
			fileName = "./save/XMatW";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "wb");

			vMatW.push_back(Xobj->GetvecMatW()[0]);

			fwrite(&vMatW[0], vMatW.size() * sizeof(D3DXMATRIXA16), 1, fp); //중요!! 사이즈 * sizeof(타입)
			fclose(fp);

			//충돌체 매트리스
			vector<D3DXMATRIXA16> vCollisionMatW;
			fileName = "./save/XCollisionMatW";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "wb");

			vCollisionMatW.push_back(Xobj->GetvecCollisionMatW()[0]);

			fwrite(&vCollisionMatW[0], vCollisionMatW.size() * sizeof(D3DXMATRIXA16), 1, fp); //중요!! 사이즈 * sizeof(타입)
			fclose(fp);

			//파일 이름
			fileName = "./save/XFileName";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "w");
			fputs(Xobj->GetvecFileName()[0].c_str(), fp);
			fclose(fp);

			//충돌체 구분
			fileName = "./save/XisSphere";
			fileName += to_string(i);
			char temp[1024];
			fp = fopen(fileName.c_str(), "w");
			itoa(Xobj->GetIsSphere(), temp, 10);
			fputs(temp, fp);
			fclose(fp);

			//이제 밑의 작업 안해줘도 됨. (파일 이름과 매트리스만 저장해서 로드할때 파일이름으로 새로 로드 후 매트리스만 저장한다.)
			/*
			//매쉬
			vector<LPD3DXMESH> vMesh;
			fileName = "./save/XMesh";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "wb");

			vMesh.push_back(Xobj->GetvecMesh()[0]);

			fwrite(&vMesh[0], vMesh.size() * sizeof(LPD3DXMESH), 1, fp); //중요!! 사이즈 * sizeof(타입)
			fclose(fp);

			//매터리얼
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

			fwrite(&vMaterial[0], vMaterial.size() * sizeof(D3DMATERIAL9), 1, fp); //중요!! 사이즈 * sizeof(타입)
			fclose(fp);

			//매터리얼 갯수
			vector<DWORD> vMtlNum;
			fileName = "./save/XMtlNum";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "wb");

			vMtlNum.push_back(Xobj->GetvecMtlNum()[0]);

			fwrite(&vMtlNum[0], vMtlNum.size() * sizeof(DWORD), 1, fp); //중요!! 사이즈 * sizeof(타입)
			fclose(fp);

			//텍스쳐 이름
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

			fwrite(&vTextureName[0], vTextureName.size() * sizeof(string), 1, fp); //중요!! 사이즈 * sizeof(타입)
			fclose(fp);

			//충돌체 매쉬
			vector<LPD3DXMESH> vCollision;
			fileName = "./save/XCollision";
			fileName += to_string(i);
			fp = fopen(fileName.c_str(), "wb");

			vCollision.push_back(Xobj->GetvecCollision()[0]);

			fwrite(&vCollision[0], vCollision.size() * sizeof(LPD3DXMESH), 1, fp); //중요!! 사이즈 * sizeof(타입)
			fclose(fp);
			*/
		}
	}
}
