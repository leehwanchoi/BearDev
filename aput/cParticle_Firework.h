#pragma once
#include "cParticleSystem.h"


class cParticle_Firework : public cParticleSystem
{
private:
	D3DXVECTOR3 m_vOrigin;

	void PreRender() override;
	void PostRender() override;

public:
	cParticle_Firework(D3DXVECTOR3& origin, int numParticles);
	~cParticle_Firework();

	void Update(bool& fire);
	void Update() override;
	void ResetParticle(ParticleAttribute& attribute) override;

	//SetÇÔ¼ö
	void SetParticleSize(float size){ m_fSize = size; }
};

