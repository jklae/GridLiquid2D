#pragma once
#include "TimeIntegration.h"

class GlobalIntegration : public TimeIntegration
{
public:
	GlobalIntegration(float timeStep, GridIndex& index);
	~GlobalIntegration() override;

	float computeTimeStep() override;
	void initialize(std::vector<DirectX::XMFLOAT2>& vel) override;

private:

};

