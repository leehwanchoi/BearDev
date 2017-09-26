#pragma once
#include "cParticleSystem.h"


class cParticle_Snow : public cParticleSystem
{
private:
	BoundingBox m_stBoundingBox;		// 눈이 내리는 범위

public:
	cParticle_Snow(BoundingBox& boundingBox, int numParticles);
	virtual ~cParticle_Snow();

	void Update() override;
	void ResetParticle(ParticleAttribute& attribute) override;

	void SetBoundingBox(BoundingBox Box) { m_stBoundingBox = Box; }
};

