#include "FixedIntegration.h"
using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace std;

FixedIntegration::FixedIntegration(float timeStep)
	:TimeIntegration(timeStep)
{
}

FixedIntegration::~FixedIntegration()
{
}

float FixedIntegration::computeTimeStep()
{
	return _timeStep;
}

void FixedIntegration::initialize(vector<XMFLOAT2>& vel)
{
}
