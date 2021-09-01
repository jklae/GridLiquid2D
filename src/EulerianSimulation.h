#pragma once
#include <iostream>
#include "FluidSimulation.h"

class EulerianSimulation : public FluidSimulation
{
public:
	EulerianSimulation(float timeStep);
	~EulerianSimulation() override;


	void initialize() override;


protected:
	virtual void _force() = 0;
	virtual void _advect() = 0;
	virtual void _diffuse() = 0;
	virtual void _project() = 0;

	void _setBoundary(std::vector<DirectX::XMFLOAT2>& vec);
	void _setBoundary(std::vector<float>& scalar);
	void _paintGrid();
	void _updateParticlePosition();

	// ---

	int _computeFaceMinMaxIndex(VALUE vState, AXIS axis, DirectX::XMFLOAT2 particlePos);
	int _computeCenterMinMaxIndex(VALUE vState, AXIS axis, DirectX::XMFLOAT2 particlePos);
	DirectX::XMFLOAT2 _velocityInterpolation(DirectX::XMFLOAT2 pos, std::vector<DirectX::XMFLOAT2> oldvel);
	float _interpolation(float value1, float value2, float ratio);
};

