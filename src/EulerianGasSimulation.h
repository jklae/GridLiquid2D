#pragma once
#include "GridFluidSim.h"

class EulerianGasSimulation : public GridFluidSim
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

