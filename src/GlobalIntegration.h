#pragma once
#include "ITimeIntegration.h"

class GlobalIntegration : public ITimeIntegration
{
public:
	float computeTimeStep() override;
	void initialize(float timeStep) override;

private:

};

