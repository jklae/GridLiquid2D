#pragma once
#include "gridsimheader.h"

class TimeIntegration
{
public:
	TimeIntegration(float timeStep, GridData& index);
	virtual ~TimeIntegration();

	virtual int getIterNum() = 0;
	virtual int getGroup(int i) = 0;
	virtual void setGroup(std::vector<DirectX::XMFLOAT2>& vel) = 0;

	virtual float computeGridTimeStep(DirectX::XMFLOAT2 vel, int i, int j) = 0;
	virtual float computeParticleTimeStep(DirectX::XMFLOAT2 vel, int i) = 0;
	virtual void computeGlobalTimeStep(std::vector<DirectX::XMFLOAT2>& vel, std::vector<STATE>& state) = 0;
	virtual void computeAdvectTimeStep(std::vector<float>& pCount, int i, int j) = 0;
	virtual void reInterpTimeStep( DirectX::XMFLOAT2 ratio, DirectX::XMINT2 minIndex, DirectX::XMINT2 maxIndex, int i) = 0;

	float getTimeStep();

protected:
	float _maxTimeStep = FPS_60;
	float _maxVelocity = _cflConstant * 60.0f;

	float _timeStep;
	GridData& _INDEX;

	float _cflCondition(float magnitude);

private:
	static constexpr float _cflConstant = 0.2f;
};

