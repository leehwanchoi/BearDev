#pragma once
#include "cParticleSystem.h"

class cParticle_Walk : public cParticleSystem
{
private:
	BoundingBox m_stBoundingBox;
	D3DXVECTOR3 m_vPos;

public:
	cParticle_Walk(BoundingBox& boundingBox, int numParticles, D3DXVECTOR3 vPos);
	virtual ~cParticle_Walk();

	void Update() override;
	void ResetParticle(ParticleAttribute& attribute) override;

	void SetBoundingBox(BoundingBox Box) { m_stBoundingBox = Box; }
	void SetPlayerPosition(D3DXVECTOR3 vPos) { m_vPos = vPos; }
};

