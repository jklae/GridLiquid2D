#include "TimeIntegration.h"

TimeIntegration::TimeIntegration(float timeStep, GridData& index)
	:_timeStep(timeStep), _maxTimeStep(timeStep), _INDEX(index)
{
}

TimeIntegration::~TimeIntegration()
{
}

float TimeIntegration::_cflCondition(float magnitude)
{
	float cflConstant = 0.5f;
	float timestep = (magnitude > EPS_FLOAT) ? min((1 / magnitude) * cflConstant, _maxTimeStep) : _maxTimeStep;
	return timestep;
}
