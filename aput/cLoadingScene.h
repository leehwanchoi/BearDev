#pragma once
#include <string>
class cUIObject;

class cLoadingScene : public cObject
{
private:
	LPD3DXSPRITE			m_pSprite;
	D3DXIMAGE_INFO			m_stImageInfo;
	cUIObject*				m_pUIRoot;

	bool					m_isLoadingRender;	//�ε��� ���� ���� Ȯ��(�ε����� ���� ��->�� ��ȯ)
	bool					m_isChange;			//�� ��ȯ���� Ȯ��
	string					m_sSceneName;		//������ �� ����
				

	void LoadingUpdate();	//�ε� �۾�

public:
	cLoadingScene();
	~cLoadingScene();

	void Setup();
	void Update(D3DXVECTOR3* pos = NULL);
	void Render();

	// �ε��� ���� �� �� ���� (�� �߰��� cMainGame���� �߰��� ���)
	void LoadingScene(string Scene);
};

