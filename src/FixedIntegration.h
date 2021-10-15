#pragma once
#include "TimeIntegration.h"

class FixedIntegration : public TimeIntegration
{
public:
	FixedIntegration(float timeStep, GridIndex& index);
	~FixedIntegration() override;

	float computeTimeStep(DirectX::XMFLOAT2 vel) override;
	void initialize(std::vector<DirectX::XMFLOAT2>& vel, std::vector<STATE>& state) override;

private:

};

