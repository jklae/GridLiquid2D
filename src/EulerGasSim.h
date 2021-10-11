#pragma once
#include "GridFluidSim.h"

class EulerGasSim : public GridFluidSim
{
public:
	EulerGasSim(float timeStep);
	~EulerGasSim() override;

	void update() override;

private:

	void _force();
	void _advect();
	void _project();
};

