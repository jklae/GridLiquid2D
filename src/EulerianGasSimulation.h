#pragma once
#include "EulerianSimulation.h"

class EulerianGasSimulation : public EulerianSimulation
{
public:
	EulerianGasSimulation();
	~EulerianGasSimulation() final;

private:
	void _update(double timestep) override;

	void _force(double timestep) override;
	void _advect(double timestep) override;
	void _diffuse(double timestep) override;
	void _project(double timestep) override;
};

