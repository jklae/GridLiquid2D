#pragma once
#include "FluidSimulation.h"

class PICLiquidSimulation : public FluidSimulation
{
public:
	PICLiquidSimulation(float timeStep);
	~PICLiquidSimulation();

private:
	void _update() override;

	void _force() override;
	void _advect() override;
	void _diffuse() override;
	void _project() override;
};

