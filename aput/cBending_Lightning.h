#pragma once

class cBending_Lightning
{
private:
	D3DXMATRIXA16			m_matW;
	float					m_fRotY;

	LPDIRECT3DTEXTURE9		m_pTexture;	//텍스쳐
	float					m_fX;		//텍스쳐 좌표 X
	float					m_fY;		//텍스쳐 좌표 Y

	vector<ST_PT_VERTEX>	m_vecPTVertex;		//텍스쳐를 표시할 공간

	bool					m_isPause;			//재생 중인지 확인
	bool					m_isFront;			//앞으로 재생하는지 확인
	int						m_nFullFrame;		//전체 프레임
	int						m_nOneFrame;		//한 프레임
	float					m_fTime;			//시간 저장
	float					m_fSetTime;			//설정 시간 (속도)

public:
	cBending_Lightning();
	~cBending_Lightning();

	void Setup();
	void Update();
	void Render();

	void PrevImage();
	void NextImage();

	//앞,뒤 재생 설정
	bool GetisFront() { return m_isFront; }
	void SetisFront(bool isFront) { m_isFront = isFront; }

	//재생, 정지 설정
	bool GetisPause() { return m_isPause; }
	void SetisPause(bool isplay) { m_isPause = isplay; }

	void SetPosition(float x, float y, float z) { m_matW._41 = x, m_matW._42 = y, m_matW._43 = z; }
	void SetScailing(float x, float y, float z) { m_matW._11 = x, m_matW._22 = y, m_matW._33 = z; }

	void SetRotationY(float rotX)
	{
		//SRT로 진행되는데, R만 하게 되면, POS가 0,0,0으로 초기화 되버린다.
		//그래서, _41(x), _42(y), _43(z) 값만 따로 저장해서 크기를 조절하고, 위치를 재조정해준다.
		D3DXVECTOR3 Pos(m_matW._41, m_matW._42, m_matW._43);

		m_fRotY += rotX;

		D3DXMATRIXA16 matY;
		D3DXMatrixRotationY(&matY, m_fRotY);
		m_matW = matY;

		//메쉬 위치 조정
		m_matW._41 = Pos.x;
		m_matW._42 = Pos.y;
		m_matW._43 = Pos.z;
	};
};