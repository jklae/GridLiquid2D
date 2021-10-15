#pragma once
#include "gridsimheader.h"

class TimeIntegration
{
public:
	TimeIntegration(float timeStep, GridData& index);
	virtual ~TimeIntegration();

	virtual float computeGridTimeStep(DirectX::XMFLOAT2 vel) = 0;
	virtual float computeParticleTimeStep(DirectX::XMFLOAT2 vel, int i) = 0;
	virtual void computeGlobalTimeStep(std::vector<DirectX::XMFLOAT2>& vel, std::vector<STATE>& state) = 0;

protected:
	float _timeStep;
	float _maxTimeStep;
	GridData& _INDEX;

	float _cflCondition(float magnitude);
};

