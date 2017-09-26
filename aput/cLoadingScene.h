#pragma once
#include <string>
class cUIObject;

class cLoadingScene : public cObject
{
private:
	LPD3DXSPRITE			m_pSprite;
	D3DXIMAGE_INFO			m_stImageInfo;
	cUIObject*				m_pUIRoot;

	bool					m_isLoadingRender;	//로딩씬 렌더 할지 확인(로딩씬을 렌더 후->씬 전환)
	bool					m_isChange;			//씬 전환인지 확인
	string					m_sSceneName;		//변경할 씬 네임
				

	void LoadingUpdate();	//로딩 작업

public:
	cLoadingScene();
	~cLoadingScene();

	void Setup();
	void Update(D3DXVECTOR3* pos = NULL);
	void Render();

	// 로딩씬 렌더 및 씬 변경 (씬 추가는 cMainGame에서 추가후 사용)
	void LoadingScene(string Scene);
};

