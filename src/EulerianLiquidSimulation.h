#pragma once
#include "EulerianSimulation.h"

class EulerianLiquidSimulation : public EulerianSimulation
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

