#pragma once
#include "GridFluidSim.h"

class EulerGasSim : public GridFluidSim
{
public:
	EulerGasSim(float timeStep, int delayTime);
	~EulerGasSim() override;

	void update() override;

private:

	void _force() override;
	void _advect() override;
	void _diffuse() override;
	void _project() override;
};

