#pragma once
#include "EulerianSimulation.h"

class EulerianGasSimulation : public EulerianSimulation
{
public:
	EulerianGasSimulation();
	~EulerianGasSimulation();

private:
	void _force(double timestep) override;
	void _advect(double timestep) override;
	void _diffuse(double timestep) override;
	void _project(double timestep) override;
};

