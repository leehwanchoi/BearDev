#pragma once
class cUIObject;

#define	HPSTARTX 50
#define HPENDX 280
#define HPPERCENT ((HPENDX) - (HPSTARTX))

enum DamagedState
{
	NoneDamaged,
	SetDamaged,
	Damaged
};

class cHPBar : public cObject
{
private:
	LPD3DXSPRITE			m_pSprite;
	D3DXIMAGE_INFO			m_stImageInfo;
	cUIObject*				m_pUIRoot;
	cUIObject*				m_pUIHP;

	DamagedState			m_eDamagedState;//������ ���� Ȯ�� (���� ����)
	float					m_fCurrentHP;	//���� HP

	bool					m_isDamaged;	//������ ���� ���� Ȯ�� (�ܺ� ���ۿ�)
	bool					m_isOneKill;	//�� ��
	bool					m_isDeath;		//���� �������� Ȯ��
	bool					m_isSetHp;
	bool					m_isDamageOnUpdate;

	//���� �Լ�
	void HPUpdate();

public:
	cHPBar();
	~cHPBar();

	void Setup();
	void Update(D3DXVECTOR3* pos = NULL);
	void Render();

	void DamagedOn() { m_isDamaged = true; } //������ ���·� ����
	void SetIsOneKill(bool isOneKill){ m_isOneKill = isOneKill; }
	bool GetisDeath() { return m_isDeath; }  //���� �������� Ȯ��
	void SetIsDeath(bool isDeath){ m_isDeath = isDeath; }
	void SetFullHP(); //HP�� Full���·� ����

	float GetCurrentHp(){ return m_fCurrentHP; }
	void SetCurrentHp(float hp){ m_fCurrentHP = hp; }

	bool GetIsSetHp(){ return m_isSetHp; }
	void SetIsSetHp(bool isSet){ m_isSetHp = isSet; }

	void SetIsDamageOnUpdate(bool isDamageOnUpdate){ m_isDamageOnUpdate = isDamageOnUpdate; }
};