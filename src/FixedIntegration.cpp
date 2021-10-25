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

void FixedIntegration::computeGlobalTimeStep(vector<XMFLOAT2>& vel, vector<STATE>& state)
{
}

void FixedIntegration::computeAdvectTimeStep(std::vector<float>& pCount, int i, int j)
{
}

void FixedIntegration::reInterpTimeStep(DirectX::XMFLOAT2 ratio, DirectX::XMINT2 minIndex, DirectX::XMINT2 maxIndex, int i)
{
}
