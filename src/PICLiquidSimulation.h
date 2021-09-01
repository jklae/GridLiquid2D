#pragma once
#include "EulerianSimulation.h"

class PICLiquidSimulation : public EulerianSimulation
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

