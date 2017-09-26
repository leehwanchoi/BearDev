#pragma once

class cAnimationTexture
{
private:
	D3DXMATRIXA16			m_matW;

	LPDIRECT3DTEXTURE9		m_pTexture;	//�ؽ���
	float					m_fX;		//�ؽ��� ��ǥ X
	float					m_fY;		//�ؽ��� ��ǥ Y

	vector<ST_PT_VERTEX>	m_vecPTVertex;		//�ؽ��ĸ� ǥ���� ����

	bool					m_isPause;			//��� ������ Ȯ��
	bool					m_isFront;			//������ ����ϴ��� Ȯ��
	D3DXVECTOR2				m_textureCoor[4];	//������ ��ǥ ����
	int						m_nFullFrame;		//��ü ������
	float					m_fOneFrame;		//��ü ������ / 1.0f (�� ������)
	float					m_fTime;			//�ð� ����
	float					m_fSetTime;			//���� �ð� (�ӵ�)

public:
	cAnimationTexture();
	~cAnimationTexture();

	void Load(char* lpFileName, int FrameNum);
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
};

