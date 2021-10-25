#pragma once
#include "gridsimheader.h"

class TimeIntegration
{
public:
	TimeIntegration(float timeStep, GridData& index);
	virtual ~TimeIntegration();

	virtual int getGroup() = 0;
	virtual float computeGridTimeStep(DirectX::XMFLOAT2 vel, int i, int j) = 0;
	virtual float computeParticleTimeStep(DirectX::XMFLOAT2 vel, int i) = 0;
	virtual void computeGlobalTimeStep(std::vector<DirectX::XMFLOAT2>& vel, std::vector<STATE>& state) = 0;
	virtual void computeAdvectTimeStep(std::vector<float>& pCount, int i, int j) = 0;
	virtual void reInterpTimeStep( DirectX::XMFLOAT2 ratio, DirectX::XMINT2 minIndex, DirectX::XMINT2 maxIndex, int i) = 0;

protected:
	float _timeStep;
	float _maxTimeStep;
	GridData& _INDEX;

	float _cflCondition(float magnitude);
};

