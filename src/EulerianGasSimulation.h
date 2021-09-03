#pragma once
#include "FluidSimulation.h"

class EulerianGasSimulation : public FluidSimulation
{
public:
	EulerianGasSimulation(float timeStep);
	~EulerianGasSimulation() final;

private:
	void _update() override;

	void _force() override;
	void _advect() override;
	void _diffuse() override;
	void _project() override;
};

