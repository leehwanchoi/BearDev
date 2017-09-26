#pragma once
#include "cParticleSystem.h"


class cParticle_Explosion : public cParticleSystem
{
private:
	D3DXVECTOR3		m_vOrigin;
	int				m_nAlpha;
	int				m_nDelta;

	void PreRender() override;
	void PostRender() override;

public:
	cParticle_Explosion(D3DXVECTOR3& origin, int numParticles);
	virtual ~cParticle_Explosion();

	void Update() override;
	void Update(bool& explosion);
	void ResetParticle(ParticleAttribute& attribute) override;
};