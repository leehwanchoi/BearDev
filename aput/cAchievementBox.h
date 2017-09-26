#pragma once
#include <string>
class cUIObject;

//�ּ� 2�� �̻� �־����.
enum
{
	None = 0,
	E_AchievementTEXT
};

class cAchievementBox : public cObject
{
private:
	LPD3DXSPRITE			m_pSprite2;
	D3DXIMAGE_INFO			m_stImageInfo;
	cUIObject*				m_pUIRoot;

	bool					m_isQuestSuccess;//����Ʈ �����ߴ��� Ȯ��
	string					m_sQuestName;	 //����Ʈ �̸�

	bool					m_isAnimationEnd;//���� �ִϸ��̼� Ȯ��
	float					m_fCount;		 //�ð� ���
	float					m_fTime;		 //���� ������ �ð�
	int						m_nDir;			 //������ �����̴� ���� (y��ǥ)

	int						m_nPlayCnt;

	//�ʱ� ��ġ�� ����
	void	InitPosition();

public:
	cAchievementBox();
	~cAchievementBox();

	void Setup();
	void Update(D3DXVECTOR3* pos = NULL);
	void Render();

	//����Ʈ �Ϸ��ߴ��� �����ϴ� �Լ� (�� �Լ��� true�ؼ� ������ �����Ų��.)
	void SetisisQuestSuccess(bool isQuestSuccess) { m_isQuestSuccess = isQuestSuccess; }
	//�Ϸ��� ����Ʈ �̸�
	void SetQuestName(string QuestName);

	//����Ʈ�� �����̰� �ϴ� �Լ�
	void AchievementBarUpdate();

	void SetPlayCnt(int cnt){ m_nPlayCnt = cnt; }
};

