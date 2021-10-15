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

	float cflConstant = 0.25f;
	float timestep = (maxMag > EPS_FLOAT) ? min((1 / maxMag) * cflConstant, 0.01f) : 0.01f;
	cout << "TimeStep : "  << timestep << ", Mag : " << maxMag << endl;

	_timeStep = timestep;
}
