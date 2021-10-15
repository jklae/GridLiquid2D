#include "TimeIntegration.h"

TimeIntegration::TimeIntegration(float timeStep, GridIndex& index)
	:_timeStep(timeStep), _INDEX(index)
{
}

TimeIntegration::~TimeIntegration()
{
}

float TimeIntegration::_cflCondition(float magnitude)
{
	float cflConstant = 0.5f;
	float timestep = (magnitude > EPS_FLOAT) ? min((1 / magnitude) * cflConstant, 0.02f) : 0.02f;
	return timestep;
}
