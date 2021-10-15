#include "OursIntegration.h"

using namespace DirectX;
using namespace std;

OursIntegration::OursIntegration(float timeStep, GridIndex& index)
	:TimeIntegration(timeStep, index)
{
}

OursIntegration::~OursIntegration()
{
}

float OursIntegration::computeTimeStep(DirectX::XMFLOAT2 vel)
{
	float magnitude = sqrtf(powf(vel.x, 2.0f) + powf(vel.y, 2.0f));
	float timestep = _cflCondition(magnitude);

	return timestep;
}

void OursIntegration::initialize(std::vector<DirectX::XMFLOAT2>& vel, std::vector<STATE>& state)
{
}
