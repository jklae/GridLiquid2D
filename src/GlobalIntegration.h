#pragma once
#include "TimeIntegration.h"

class GlobalIntegration : public TimeIntegration
{
public:
	GlobalIntegration(float timeStep, GridIndex& index);
	~GlobalIntegration() override;

	float computeTimeStep(DirectX::XMFLOAT2 vel) override;
	void initialize(std::vector<DirectX::XMFLOAT2>& vel, std::vector<STATE>& state) override;

private:

};

