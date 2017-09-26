#pragma once

class cAnimationTexture
{
private:
	D3DXMATRIXA16			m_matW;

	LPDIRECT3DTEXTURE9		m_pTexture;	//텍스쳐
	float					m_fX;		//텍스쳐 좌표 X
	float					m_fY;		//텍스쳐 좌표 Y

	vector<ST_PT_VERTEX>	m_vecPTVertex;		//텍스쳐를 표시할 공간

	bool					m_isPause;			//재생 중인지 확인
	bool					m_isFront;			//앞으로 재생하는지 확인
	D3DXVECTOR2				m_textureCoor[4];	//프레임 좌표 저장
	int						m_nFullFrame;		//전체 프레임
	float					m_fOneFrame;		//전체 프레임 / 1.0f (한 프레임)
	float					m_fTime;			//시간 저장
	float					m_fSetTime;			//설정 시간 (속도)

public:
	cAnimationTexture();
	~cAnimationTexture();

	void Load(char* lpFileName, int FrameNum);
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
};

