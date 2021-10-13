#pragma once
#include "TimeIntegration.h"

class FixedIntegration : public TimeIntegration
{
public:
	FixedIntegration(float timeStep);
	~FixedIntegration();

	float computeTimeStep() override;
	void initialize(float timeStep) override;

private:

};

