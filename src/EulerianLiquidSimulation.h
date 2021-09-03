#pragma once
#include "GridFluidSim.h"

class EulerianLiquidSimulation : public GridFluidSim
{
public:
	EulerianLiquidSimulation(float timeStep);
	~EulerianLiquidSimulation() final;

private:
	void _update() override;

	void _force() override;
	void _advect() override;
	void _diffuse() override;
	void _project() override;
};

