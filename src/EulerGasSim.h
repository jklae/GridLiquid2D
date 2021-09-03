#pragma once
#include "GridFluidSim.h"

class EulerGasSim : public GridFluidSim
{
public:
	EulerGasSim(float timeStep, int delayTime);
	~EulerGasSim() final;

private:
	void _update() override;

	void _force() override;
	void _advect() override;
	void _diffuse() override;
	void _project() override;
};

