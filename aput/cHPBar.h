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

	DamagedState			m_eDamagedState;//데미지 상태 확인 (내부 변수)
	float					m_fCurrentHP;	//현재 HP

	bool					m_isDamaged;	//데미지 받은 상태 확인 (외부 조작용)
	bool					m_isOneKill;	//한 방
	bool					m_isDeath;		//죽은 상태인지 확인
	bool					m_isSetHp;
	bool					m_isDamageOnUpdate;

	//내부 함수
	void HPUpdate();

public:
	cHPBar();
	~cHPBar();

	void Setup();
	void Update(D3DXVECTOR3* pos = NULL);
	void Render();

	void DamagedOn() { m_isDamaged = true; } //데미지 상태로 변경
	void SetIsOneKill(bool isOneKill){ m_isOneKill = isOneKill; }
	bool GetisDeath() { return m_isDeath; }  //죽은 상태인지 확인
	void SetIsDeath(bool isDeath){ m_isDeath = isDeath; }
	void SetFullHP(); //HP를 Full상태로 변경

	float GetCurrentHp(){ return m_fCurrentHP; }
	void SetCurrentHp(float hp){ m_fCurrentHP = hp; }

	bool GetIsSetHp(){ return m_isSetHp; }
	void SetIsSetHp(bool isSet){ m_isSetHp = isSet; }

	void SetIsDamageOnUpdate(bool isDamageOnUpdate){ m_isDamageOnUpdate = isDamageOnUpdate; }
};