#pragma once
#include "GridFluidSim.h"

class EulerGasSim : public GridFluidSim
{
public:
	EulerGasSim(float timeStep);
	~EulerGasSim() override;

private:

	void _update() override;
	void _force();
	void _advect();
	void _project();
};

