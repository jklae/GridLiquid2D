#include "FixedIntegration.h"

using namespace DirectX;
using namespace std;

FixedIntegration::FixedIntegration(float timeStep, GridIndex& index)
	:TimeIntegration(timeStep, index)
{
}

FixedIntegration::~FixedIntegration()
{
}

float FixedIntegration::computeTimeStep(DirectX::XMFLOAT2 vel)
{
	return _timeStep;
}

void FixedIntegration::initialize(vector<XMFLOAT2>& vel, vector<STATE>& state)
{
}
