#pragma once

class cBending_Lightning
{
private:
	D3DXMATRIXA16			m_matW;
	float					m_fRotY;

	LPDIRECT3DTEXTURE9		m_pTexture;	//�ؽ���
	float					m_fX;		//�ؽ��� ��ǥ X
	float					m_fY;		//�ؽ��� ��ǥ Y

	vector<ST_PT_VERTEX>	m_vecPTVertex;		//�ؽ��ĸ� ǥ���� ����

	bool					m_isPause;			//��� ������ Ȯ��
	bool					m_isFront;			//������ ����ϴ��� Ȯ��
	int						m_nFullFrame;		//��ü ������
	int						m_nOneFrame;		//�� ������
	float					m_fTime;			//�ð� ����
	float					m_fSetTime;			//���� �ð� (�ӵ�)

public:
	cBending_Lightning();
	~cBending_Lightning();

	void Setup();
	void Update();
	void Render();

	void PrevImage();
	void NextImage();

	//��,�� ��� ����
	bool GetisFront() { return m_isFront; }
	void SetisFront(bool isFront) { m_isFront = isFront; }

	//���, ���� ����
	bool GetisPause() { return m_isPause; }
	void SetisPause(bool isplay) { m_isPause = isplay; }

	void SetPosition(float x, float y, float z) { m_matW._41 = x, m_matW._42 = y, m_matW._43 = z; }
	void SetScailing(float x, float y, float z) { m_matW._11 = x, m_matW._22 = y, m_matW._33 = z; }

	void SetRotationY(float rotX)
	{
		//SRT�� ����Ǵµ�, R�� �ϰ� �Ǹ�, POS�� 0,0,0���� �ʱ�ȭ �ǹ�����.
		//�׷���, _41(x), _42(y), _43(z) ���� ���� �����ؼ� ũ�⸦ �����ϰ�, ��ġ�� ���������ش�.
		D3DXVECTOR3 Pos(m_matW._41, m_matW._42, m_matW._43);

		m_fRotY += rotX;

		D3DXMATRIXA16 matY;
		D3DXMatrixRotationY(&matY, m_fRotY);
		m_matW = matY;

		//�޽� ��ġ ����
		m_matW._41 = Pos.x;
		m_matW._42 = Pos.y;
		m_matW._43 = Pos.z;
	};
};