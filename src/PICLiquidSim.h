#pragma once
#include "GridFluidSim.h"

class PICLiquidSim: public GridFluidSim
{
public:
	PICLiquidSim(float timeStep, int delayTime);
	~PICLiquidSim() override;

	void update() override;
	
private:

	void _advect();
	void _force();
	void _project();
};

