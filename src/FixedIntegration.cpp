#include "FixedIntegration.h"

FixedIntegration::FixedIntegration(float timeStep)
	:TimeIntegration(timeStep)
{
}

FixedIntegration::~FixedIntegration()
{
}

float FixedIntegration::computeTimeStep()
{
	return _timeStep;
}

void FixedIntegration::initialize(float timeStep)
{
}
