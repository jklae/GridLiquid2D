#pragma once
#include "gridsimheader.h"

class TimeIntegration
{
public:
	TimeIntegration(float timeStep, GridIndex& index);
	virtual ~TimeIntegration();

	virtual float computeTimeStep(DirectX::XMFLOAT2 vel) = 0;
	virtual void initialize(std::vector<DirectX::XMFLOAT2>& vel, std::vector<STATE>& state) = 0;

protected:
	float _timeStep;
	GridIndex& _INDEX;

	float _cflCondition(float magnitude);
};

