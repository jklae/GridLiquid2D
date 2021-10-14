#pragma once
#include "TimeIntegration.h"

class FixedIntegration : public TimeIntegration
{
public:
	FixedIntegration(float timeStep);
	~FixedIntegration() override;

	float computeTimeStep() override;
	void initialize(float timeStep) override;

private:

};

