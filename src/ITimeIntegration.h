#pragma once
class ITimeIntegration abstract
{
public:
	virtual float computeTimeStep() = 0;
	virtual void initialize(float timeStep) = 0;
};

