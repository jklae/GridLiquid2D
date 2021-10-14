#pragma once
#include "dx12header.h"

class TimeIntegration
{
public:
	TimeIntegration(float timeStep);
	virtual ~TimeIntegration();

	virtual float computeTimeStep() = 0;
	virtual void initialize(float timeStep) = 0;

protected:
	float _timeStep;
};

