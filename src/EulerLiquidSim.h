#pragma once
#include "GridFluidSim.h"

class EulerLiquidSim : public GridFluidSim
{
public:
	EulerLiquidSim(float timeStep);
	~EulerLiquidSim() override;

	void update() override;

private:

	void _force();
	void _advect();
	void _project();
};

