#include "GlobalIntegration.h"

GlobalIntegration::GlobalIntegration(float timeStep)
	:TimeIntegration(timeStep)
{
}

GlobalIntegration::~GlobalIntegration()
{
}

float GlobalIntegration::computeTimeStep()
{
	return _timeStep;
}

void GlobalIntegration::initialize(float timeStep)
{
}
