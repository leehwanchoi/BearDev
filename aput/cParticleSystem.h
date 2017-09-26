#pragma once

struct ParticleAttribute
{
	D3DXVECTOR3 pos = D3DXVECTOR3(0, 0, 0);			// ��ƼŬ ��ġ
	D3DXVECTOR3 velocity = D3DXVECTOR3(0, 0, 0);	// ��ƼŬ �ӵ�(���� �ʴ� �ӵ�)
	float lifeTime = 0.0f;							// �ִ� ���� �ð�
	float age = 0.0f;								// ���� �� �帥 �ð�
	D3DXCOLOR color = D3DCOLOR_XRGB(255, 255, 255);	// ��ƼŬ ����
	D3DXCOLOR colorFade = D3DCOLOR_XRGB(0, 0, 0);	// �ð��� ���� ��ȭ �� ����
	bool isAlive = false;							// ��ƼŬ�� ��� �ִ� Ȯ��
};

class cParticleSystem
{
protected:
	D3DXMATRIXA16 m_matWorld;
	DWORD FloatToDWORD(float f) { return *((DWORD*)(&f)); }

	float m_fSize = 0.1f;							// ��ƼŬ�� ũ��
	LPDIRECT3DTEXTURE9 m_pTexture = NULL;			// ��ƼŬ�� �ؽ���
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer = NULL;	// ���ؽ� ����
	list<ParticleAttribute> m_lstParticles;			// ��ü ��ƼŬ ����Ʈ

	// ��ƼŬ�� �� �� ȿ�������� �׸��� ���ؼ� ����ϴ� ������
	DWORD m_vbSize = 2048;							// ���ؽ� ���� ������
	DWORD m_vbOffset = 0;							// ���ؽ� ���۸� ��ױ� ���� ������(��ƼŬ ���� ����)
	DWORD m_vbBatchSize = 512;						// �ϳ��� �ܰ迡 �� ��ƼŬ ����

	virtual void PreRender();
	virtual void PostRender();

public:
	cParticleSystem();
	virtual ~cParticleSystem();

	virtual void Setup(char* textureFileName);
	virtual void Update() PURE;
	virtual void Render();
	virtual void Destroy();

	virtual void AddParticle();			//��ƼŬ �Ѱ� �߰�
	virtual void Reset();				//��ƼŬ �ʱ�ȭ(����)
	virtual void ResetParticle(ParticleAttribute& attribute) PURE; //��ƼŬ Ÿ�Կ� ���� �����ϱ� ���� �Լ�

	bool IsEmpty();		//��ƼŬ�� �����
	bool IsDead();		//��ƼŬ�� �� �׾���
};

