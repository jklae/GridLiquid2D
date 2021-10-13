#pragma once
#include "ITimeIntegration.h"

class FIxedIntegration : public ITimeIntegration
{
public:
	float computeTimeStep() override;
	void initialize(float timeStep) override;

private:

};

