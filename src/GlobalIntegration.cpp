#include "GlobalIntegration.h"
using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace std;

GlobalIntegration::GlobalIntegration(float timeStep, GridIndex& index)
	:TimeIntegration(timeStep, index)
{
}

GlobalIntegration::~GlobalIntegration()
{
}

float GlobalIntegration::computeTimeStep()
{
	return _timeStep;
}

void GlobalIntegration::initialize(vector<XMFLOAT2>& vel, std::vector<STATE>& state)
{
	int N = _INDEX.gridCount - 2;
	for (int i = 1; i <= N; i++)
	{
		for (int j = 1; j <= N; j++)
		{
			if (state[_INDEX(i, j)] == STATE::FLUID)
			{
			}
		}
	}
}
