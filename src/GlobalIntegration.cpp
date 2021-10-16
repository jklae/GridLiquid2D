#include "GlobalIntegration.h"

using namespace DirectX;
using namespace std;

GlobalIntegration::GlobalIntegration(float timeStep, GridData& index)
	:TimeIntegration(timeStep, index)
{
}

GlobalIntegration::~GlobalIntegration()
{
}

float GlobalIntegration::computeGridTimeStep(DirectX::XMFLOAT2 vel, int i, int j)
{
	return _timeStep;
}

float GlobalIntegration::computeParticleTimeStep(DirectX::XMFLOAT2 vel, int i)
{
	return _timeStep;
}

void GlobalIntegration::computeGlobalTimeStep(vector<XMFLOAT2>& vel, std::vector<STATE>& state)
{
	float maxMag = 0.0f;

	int N = _INDEX.gridCount - 2;
	for (int i = 1; i <= N; i++)
	{
		for (int j = 1; j <= N; j++)
		{
			if (state[_INDEX(i, j)] == STATE::FLUID)
			{
				float magnitude = sqrtf(powf(vel[_INDEX(i, j)].x, 2.0f) + powf(vel[_INDEX(i, j)].y, 2.0f));
				if (magnitude > maxMag) maxMag = magnitude;

			}
		}
	}

	float timestep = _cflCondition(maxMag);
	cout << "TimeStep : "  << timestep << ", Mag : " << maxMag << endl;

	_timeStep = timestep;
}

void GlobalIntegration::computeAdvectTimeStep(std::vector<float>& pCount, int i, int j)
{
}

void GlobalIntegration::reInterpTimeStep(DirectX::XMFLOAT2 ratio, DirectX::XMINT2 minIndex, DirectX::XMINT2 maxIndex, int i)
{
}
