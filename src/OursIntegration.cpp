#include "OursIntegration.h"

using namespace DirectX;
using namespace std;

OursIntegration::OursIntegration(float timeStep, GridData& index)
	:TimeIntegration(timeStep, index)
{
	int gCount = index.gridCount;
	int pCount = index.particleCount;
	int gSize = gCount * gCount;
	int pSsize = gSize * pCount * pCount;

	_tempStep.assign(gSize, 0.0f);
	_gridTimeStep.assign(gSize, _maxTimeStep);
	_particleTimeStep.assign(pSsize, _maxTimeStep);
}

OursIntegration::~OursIntegration()
{
}

int OursIntegration::getGroup()
{
	return 0;
}

float OursIntegration::computeGridTimeStep(DirectX::XMFLOAT2 vel, int i, int j)
{
	return _gridTimeStep[_INDEX(i, j)];
}

float OursIntegration::computeParticleTimeStep(DirectX::XMFLOAT2 vel, int i)
{
	float magnitude = sqrtf(powf(vel.x, 2.0f) + powf(vel.y, 2.0f));
	_particleTimeStep[i] = _cflCondition(magnitude);

	return _particleTimeStep[i];
}

void OursIntegration::computeGlobalTimeStep(std::vector<DirectX::XMFLOAT2>& vel, std::vector<STATE>& state)
{
}

void OursIntegration::computeAdvectTimeStep(std::vector<float>& pCount, int i, int j)
{
	if (pCount[_INDEX(i, j)] > EPS_FLOAT)
	{
		_gridTimeStep[_INDEX(i, j)] = _tempStep[_INDEX(i, j)] / pCount[_INDEX(i, j)];
	}
	else
	{
		_gridTimeStep[_INDEX(i, j)] = 0.0f;
	}
	
	// Reset
	_tempStep[_INDEX(i, j)] = 0.0f;
}

void OursIntegration::reInterpTimeStep(DirectX::XMFLOAT2 ratio, DirectX::XMINT2 minIndex, DirectX::XMINT2 maxIndex, int i)
{
	reInterp(_particleTimeStep[i], _tempStep, ratio, minIndex, maxIndex, _INDEX);
}
