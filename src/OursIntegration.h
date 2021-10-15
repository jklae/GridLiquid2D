#pragma once
#include "TimeIntegration.h"

class OursIntegration : public TimeIntegration
{
public:
	OursIntegration(float timeStep, GridData& index);
	~OursIntegration() override;

	float computeGridTimeStep(DirectX::XMFLOAT2 vel) override;
	float computeParticleTimeStep(DirectX::XMFLOAT2 vel, int i) override;
	void computeGlobalTimeStep(std::vector<DirectX::XMFLOAT2>& vel, std::vector<STATE>& state) override;

private:
	std::vector<float> _particleTimeStep;
};

