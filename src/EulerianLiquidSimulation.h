#pragma once
#include "EulerianSimulation.h"

class EulerianLiquidSimulation : public EulerianSimulation
{
public:
	EulerianLiquidSimulation();
	~EulerianLiquidSimulation() final;

private:
	void _update(float timestep) override;

	void _force(float timestep) override;
	void _advect(float timestep) override;
	void _diffuse(float timestep) override;
	void _project(float timestep) override;
};

