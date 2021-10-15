#pragma once
#include "GridFluidSim.h"

class EulerGasSim : public GridFluidSim
{
public:
	EulerGasSim(GridData& index);
	~EulerGasSim() override;

private:
	void _update() override;
	void _force();
	void _advect();
	void _project();

	void _updateParticlePos() override;
};

