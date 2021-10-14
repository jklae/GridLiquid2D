#include "TimeIntegration.h"

TimeIntegration::TimeIntegration(float timeStep, GridIndex& index)
	:_timeStep(timeStep), _INDEX(index)
{
}

TimeIntegration::~TimeIntegration()
{
}
