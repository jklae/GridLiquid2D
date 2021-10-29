#include "FixedIntegration.h"

using namespace DirectX;
using namespace std;

FixedIntegration::FixedIntegration(float timeStep, GridData& index)
	:TimeIntegration(timeStep, index)
{
}

FixedIntegration::~FixedIntegration()
{
}

int FixedIntegration::getIterNum()
{
	return 1;
}

int FixedIntegration::getGroup(XMFLOAT2 vel)
{
	return 0;
}


float FixedIntegration::computeGridTimeStep(DirectX::XMFLOAT2 vel, int i, int j)
{
	return _timeStep;
}

float FixedIntegration::computeParticleTimeStep(DirectX::XMFLOAT2 vel, int i)
{
	return _timeStep;
}

