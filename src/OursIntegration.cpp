#include "OursIntegration.h"

using namespace DirectX;
using namespace std;

OursIntegration::OursIntegration(float timeStep, GridData& index)
	:TimeIntegration(timeStep, index)
{
	int gcount = index.gridCount;
	int pcount = index.particleCount;
	int size = gcount * gcount * pcount * pcount;

	_particleTimeStep.assign(size, _maxTimeStep);
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
	_particleTimeStep[i] = _cflCondition(magnitude);

	return _particleTimeStep[i];
}

void OursIntegration::computeGlobalTimeStep(std::vector<DirectX::XMFLOAT2>& vel, std::vector<STATE>& state)
{
}
