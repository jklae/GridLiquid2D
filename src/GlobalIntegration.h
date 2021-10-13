#pragma once
#include "TimeIntegration.h"

class GlobalIntegration : public TimeIntegration
{
public:
	GlobalIntegration(float timeStep);
	~GlobalIntegration();

	float computeTimeStep() override;
	void initialize(float timeStep) override;

private:

};

