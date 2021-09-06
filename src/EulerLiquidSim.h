#pragma once
#include "GridFluidSim.h"

class EulerLiquidSim : public GridFluidSim
{
public:
	EulerLiquidSim(float timeStep, int delayTime);
	~EulerLiquidSim() override;

	void update() override;

private:

	void _force() override;
	void _advect() override;
	void _diffuse() override;
	void _project() override;
};

