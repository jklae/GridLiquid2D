#pragma once
#include <iostream>
#include "FluidSimulation.h"

class EulerianSimulation : public FluidSimulation
{
public:
	EulerianSimulation();
	~EulerianSimulation();

	void initialize() override;

	void setGridDomain(int xCount, int yCount);


private:
	
	void _printVelocity();



	void _update(double timestep) override;

	void _force(double timestep);
	void _advect(double timestep);
	void _diffuse(double timestep);
	void _project(double timestep);

	void _setBoundary(std::vector<DirectX::XMFLOAT2>& vec);
	void _setBoundary(std::vector<float>& scalar);
	void _paintGrid();
	void _updateParticlePosition(double timestep);

	// ---

	int _computeFaceMinMaxIndex(VALUE vState, AXIS axis, DirectX::XMFLOAT2 particlePos);
	int _computeCenterMinMaxIndex(VALUE vState, AXIS axis, DirectX::XMFLOAT2 particlePos);
	DirectX::XMFLOAT2 _velocityInterpolation(DirectX::XMFLOAT2 pos, std::vector<DirectX::XMFLOAT2> oldvel);
	float _interpolation(float value1, float value2, float ratio);
};

