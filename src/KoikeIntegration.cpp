#include "KoikeIntegration.h"

using namespace DirectX;
using namespace std;

KoikeIntegration::KoikeIntegration(float timeStep, GridData& index)
	:TimeIntegration(timeStep, index)
{
}

KoikeIntegration::~KoikeIntegration()
{
}

int KoikeIntegration::getGroup(XMFLOAT2 vel)
{
	float magnitude = sqrtf(powf(vel.x, 2.0f) + powf(vel.y, 2.0f));

	if (magnitude >= 24.0f)
		return 1;
	else if (magnitude > 12.0f && magnitude < 24.0f)
		return 2;
	else
		return 3;
	
}

float KoikeIntegration::computeGridTimeStep(DirectX::XMFLOAT2 vel, int i, int j)
{
	return _timeStep;
}

float KoikeIntegration::computeParticleTimeStep(DirectX::XMFLOAT2 vel, int i)
{
	return _timeStep;
}

void KoikeIntegration::computeGlobalTimeStep(std::vector<DirectX::XMFLOAT2>& vel, std::vector<STATE>& state)
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
	cout << "TimeStep : " << timestep << ", Mag : " << maxMag << endl;
}

void KoikeIntegration::computeAdvectTimeStep(std::vector<float>& pCount, int i, int j)
{
}

void KoikeIntegration::reInterpTimeStep(DirectX::XMFLOAT2 ratio, DirectX::XMINT2 minIndex, DirectX::XMINT2 maxIndex, int i)
{
}
