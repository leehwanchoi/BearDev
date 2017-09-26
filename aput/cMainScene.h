#pragma once
#include"cGameNode.h"

#include "iMap.h"

class cCamera;
class cCharacterController;
class cMonsterController;
class cNPCController;
class cHeightMap;
class cSkinnedMesh;

//Level �߰�
class cSkyBox;
class cLoadMap;

//A* �˰���
class cAStar;

//�������� �ø�
class cFrustum;

class cMainScene : public cGameNode
{
private:
	cCamera*				m_pCamera;
	cCharacterController*	m_pController;			//ĳ���� ��Ʈ�ѷ�
	cMonsterController*		m_pMonsterController;	//���� ��Ʈ�ѷ�
	cNPCController*			m_pNPCController;		//NPC ��Ʈ�ѷ�
	cNPCController*			m_pNPCController2;		//NPC2 ��Ʈ�ѷ�
	cNPCController*			m_pNPCController3;		//NPC3 ��Ʈ�ѷ�

	iMap*					m_pMap;
	cHeightMap*				m_pHeight;

	cSkinnedMesh*			m_pSkinnedCharacter;	//ĳ����
	cSkinnedMesh*			m_pSkinnedMonster;		//����
	cSkinnedMesh*			m_pSkinnedNPC;			//NPC
	cSkinnedMesh*			m_pSkinnedNPC2;			//NPC2
	cSkinnedMesh*			m_pSkinnedNPC3;			//NPC3

	//vector<cSkinnedMesh*>	m_vecSkinnedNPC;		//NPC vector

	//x�� �߰�
	cSkinnedMesh*			m_pXMap;
	bool					m_isShowXMap;

	//A* �˰��� �߰�
	cAStar*					m_pAStar;

	//�������� �ø�
	cFrustum*				m_pFrustum;


	//����� ���� ���� ���� �߰�
	bool					m_isOnKey1;
	bool					m_isOnKey3;
	bool					m_isObjMap;		//m_pMap�� Obj������ Ȯ��


	/*===================================================================================*/
	/*                                 New class, This Area                              */
	/*===================================================================================*/
	//Level
	cSkyBox*				m_pSkyBox;
	cLoadMap*				m_pLoadMap;

public:
	cMainScene();
	virtual ~cMainScene();

	virtual HRESULT init() override;
	virtual void Release() override;
	virtual void update() override;
	virtual void render() override;

	//����� ���� �Լ� �߰�
	void DebugRender();

	//ĳ����&���� �浹ó�� �Լ�
	void Collision();

	virtual void WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
};

