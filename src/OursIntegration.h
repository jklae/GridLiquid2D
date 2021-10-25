#pragma once
#include "TimeIntegration.h"

class OursIntegration : public TimeIntegration
{
public:
	OursIntegration(float timeStep, GridData& index);
	~OursIntegration() override;

	int getGroup() override;
	float computeGridTimeStep(DirectX::XMFLOAT2 vel, int i, int j) override;
	float computeParticleTimeStep(DirectX::XMFLOAT2 vel, int i) override;
	void computeGlobalTimeStep(std::vector<DirectX::XMFLOAT2>& vel, std::vector<STATE>& state) override;

	void computeAdvectTimeStep(std::vector<float>& pCount, int i, int j) override;
	void reInterpTimeStep(DirectX::XMFLOAT2 ratio, DirectX::XMINT2 minIndex, DirectX::XMINT2 maxIndex, int i) override;

	//DirectX::XMFLOAT2 synchronize(std::vector<DirectX::XMFLOAT2>& vel, int standard, );

private:
	ReverseInterpolation reInterp;

	std::vector<float> _tempStep;
	std::vector<float> _gridTimeStep;
	std::vector<float> _particleTimeStep;
};

