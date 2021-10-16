#include "OursIntegration.h"

using namespace DirectX;
using namespace std;

OursIntegration::OursIntegration(float timeStep, GridData& index)
	:TimeIntegration(timeStep, index)
{
	int gCount = index.gridCount;
	int pCount = index.particleCount;
	int gSize = gCount * gCount;
	int pSsize = gSize * pCount * pCount;

	_gridTimeStep.assign(gSize, _maxTimeStep);
	_particleTimeStep.assign(pSsize, _maxTimeStep);
}

OursIntegration::~OursIntegration()
{
}

float OursIntegration::computeGridTimeStep(DirectX::XMFLOAT2 vel)
{
	return 0.02f;
}

float OursIntegration::computeParticleTimeStep(DirectX::XMFLOAT2 vel, int i)
{
	float magnitude = sqrtf(powf(vel.x, 2.0f) + powf(vel.y, 2.0f));
	float timeStep = _cflCondition(magnitude);

	return timeStep;
}

void OursIntegration::computeGlobalTimeStep(std::vector<DirectX::XMFLOAT2>& vel, std::vector<STATE>& state)
{
}
