#pragma once
#include "TimeIntegration.h"

class GlobalIntegration : public TimeIntegration
{
public:
	GlobalIntegration(float timeStep);
	~GlobalIntegration() override;

	float computeTimeStep() override;
	void initialize(float timeStep) override;

private:

};

