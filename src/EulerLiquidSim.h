#pragma once
#include "GridFluidSim.h"

class EulerLiquidSim : public GridFluidSim
{
public:
	EulerLiquidSim(GridData& index, EX ex);
	~EulerLiquidSim() override;

private:
	void _update() override;
	void _force();
	void _advect();
	void _project();

	void _updateParticlePos();
};

