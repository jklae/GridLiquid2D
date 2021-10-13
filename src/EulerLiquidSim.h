#pragma once
#include "GridFluidSim.h"

class EulerLiquidSim : public GridFluidSim
{
public:
	EulerLiquidSim(float timeStep);
	~EulerLiquidSim() override;

private:
	void _update() override;
	void _force();
	void _advect();
	void _project();

	void _updateParticlePos() override;
};

