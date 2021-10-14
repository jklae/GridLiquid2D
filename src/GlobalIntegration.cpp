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
				float magnitude = sqrtf(powf(vel[_INDEX(i, j)].x, 2.0f) + powf(vel[_INDEX(i, j)].y, 2.0f));
				float timestep = (magnitude > EPS_FLOAT) ? min((1 / magnitude) * 0.25f, 0.01f) : 0.01f;
				cout << i << ", " << j << " : " << timestep << ", " << magnitude << endl;
				//cout << _gridVelocity[_INDEX(10, 2)].x << ", " << _gridVelocity[_INDEX(10, 2)].y << endl;
			}
		}
	}
}
