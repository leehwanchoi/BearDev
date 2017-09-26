#pragma once

struct ParticleAttribute
{
	D3DXVECTOR3 pos = D3DXVECTOR3(0, 0, 0);			// 파티클 위치
	D3DXVECTOR3 velocity = D3DXVECTOR3(0, 0, 0);	// 파티클 속도(보통 초당 속도)
	float lifeTime = 0.0f;							// 최대 유지 시간
	float age = 0.0f;								// 생성 후 흐른 시간
	D3DXCOLOR color = D3DCOLOR_XRGB(255, 255, 255);	// 파티클 색상
	D3DXCOLOR colorFade = D3DCOLOR_XRGB(0, 0, 0);	// 시간에 따라 변화 될 색상
	bool isAlive = false;							// 파티클이 살아 있는 확인
};

class cParticleSystem
{
protected:
	D3DXMATRIXA16 m_matWorld;
	DWORD FloatToDWORD(float f) { return *((DWORD*)(&f)); }

	float m_fSize = 0.1f;							// 파티클의 크기
	LPDIRECT3DTEXTURE9 m_pTexture = NULL;			// 파티클의 텍스쳐
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer = NULL;	// 버텍스 버퍼
	list<ParticleAttribute> m_lstParticles;			// 전체 파티클 리스트

	// 파티클을 좀 더 효율적으로 그리기 위해서 사용하는 변수들
	DWORD m_vbSize = 2048;							// 버텍스 버퍼 사이즈
	DWORD m_vbOffset = 0;							// 버텍스 버퍼를 잠그기 위한 오프셋(파티클 갯수 단위)
	DWORD m_vbBatchSize = 512;						// 하나의 단계에 들어갈 파티클 갯수

	virtual void PreRender();
	virtual void PostRender();

public:
	cParticleSystem();
	virtual ~cParticleSystem();

	virtual void Setup(char* textureFileName);
	virtual void Update() PURE;
	virtual void Render();
	virtual void Destroy();

	virtual void AddParticle();			//파티클 한개 추가
	virtual void Reset();				//파티클 초기화(재사용)
	virtual void ResetParticle(ParticleAttribute& attribute) PURE; //파티클 타입에 따라 리셋하기 위한 함수

	bool IsEmpty();		//파티클이 비었음
	bool IsDead();		//파티클이 다 죽었음
};

