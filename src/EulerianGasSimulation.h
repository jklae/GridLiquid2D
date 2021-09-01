#pragma once
#include "EulerianSimulation.h"

class EulerianGasSimulation : public EulerianSimulation
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

