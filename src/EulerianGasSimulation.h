#pragma once
#include "EulerianSimulation.h"

class EulerianGasSimulation : public EulerianSimulation
{
public:
	EulerianGasSimulation();
	~EulerianGasSimulation() final;

private:
	void _update(float timestep) override;

	void _force(float timestep) override;
	void _advect(float timestep) override;
	void _diffuse(float timestep) override;
	void _project(float timestep) override;
};

