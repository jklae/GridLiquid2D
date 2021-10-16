#pragma once
#include "TimeIntegration.h"

class FixedIntegration : public TimeIntegration
{
public:
	FixedIntegration(float timeStep, GridData& index);
	~FixedIntegration() override;

	float computeGridTimeStep(DirectX::XMFLOAT2 vel) override;
	float computeParticleTimeStep(DirectX::XMFLOAT2 vel, int i) override;
	void computeGlobalTimeStep(std::vector<DirectX::XMFLOAT2>& vel, std::vector<STATE>& state) override;

	void computeAdvectTimeStep(int i, int j, std::vector<float>& pCount) override;
	void reInterpTimeStep(int i, DirectX::XMFLOAT2 ratio, DirectX::XMINT2 minIndex, DirectX::XMINT2 maxIndex) override;

private:

};

