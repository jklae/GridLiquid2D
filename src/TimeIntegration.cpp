#include "TimeIntegration.h"

using namespace DirectX;
using namespace std;

TimeIntegration::TimeIntegration(float timeStep, GridData& index)
	:_timeStep(timeStep), _INDEX(index)
{
	assert(timeStep <= _maxTimeStep);
	//cout << "maxTimeStep : " << _maxTimeStep << "\nmaxVelocity : " << _maxVelocity << "\n";
}

TimeIntegration::~TimeIntegration()
{
}

float TimeIntegration::getTimeStep()
{
	return _timeStep;
}


float TimeIntegration::_cflCondition(float magnitude)
{
	float cflTimeStep = min((1 / magnitude) * _cflConstant, _maxTimeStep);
	float timestep = (magnitude > EPS_FLOAT) ? cflTimeStep : _maxTimeStep;

	return timestep;
}
