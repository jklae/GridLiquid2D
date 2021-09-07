#pragma once
#include "GridFluidSim.h"

class PICLiquidSim: public GridFluidSim
{
public:
	PICLiquidSim(float timeStep, int delayTime);
	~PICLiquidSim() override;

	void update() override;
	
private:

	void _force() override;
	void _advect() override;
	void _diffuse() override;
	void _project() override;
};

