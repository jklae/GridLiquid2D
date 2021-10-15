#pragma once
#include "TimeIntegration.h"

class OursIntegration : public TimeIntegration
{
public:
	OursIntegration(float timeStep, GridIndex& index);
	~OursIntegration() override;

	float computeTimeStep(DirectX::XMFLOAT2 vel) override;
	void initialize(std::vector<DirectX::XMFLOAT2>& vel, std::vector<STATE>& state) override;

private:
};

