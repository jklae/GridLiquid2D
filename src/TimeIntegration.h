#pragma once
#include "dx12header.h"

class TimeIntegration
{
public:
	virtual float computeTimeStep() = 0;
	virtual void initialize(float timeStep) = 0;

private:
	float _timeStep;
};

