#pragma once
#include "TimeIntegration.h"

class FixedIntegration : public TimeIntegration
{
public:
	FixedIntegration(float timeStep);
	~FixedIntegration() override;

	float computeTimeStep() override;
	void initialize(std::vector<DirectX::XMFLOAT2>& vel) override;

private:

};

