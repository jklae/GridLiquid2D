#include "FixedIntegration.h"
using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace std;

FixedIntegration::FixedIntegration(float timeStep, GridIndex& index)
	:TimeIntegration(timeStep, index)
{
}

FixedIntegration::~FixedIntegration()
{
}

float FixedIntegration::computeTimeStep()
{
	return _timeStep;
}

void FixedIntegration::initialize(vector<XMFLOAT2>& vel, vector<STATE>& state)
{
}
