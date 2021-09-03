#pragma once
#include "GridFluidSim.h"

class PICLiquidSim: public GridFluidSim
{
public:
	PICLiquidSim(float timeStep);
	~PICLiquidSim();

private:
	void _update() override;

	void _force() override;
	void _advect() override;
	void _diffuse() override;
	void _project() override;
};

