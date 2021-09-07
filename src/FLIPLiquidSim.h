#pragma once
#include "GridFluidSim.h"
class FLIPLiquidSim : public GridFluidSim
{
public:
	FLIPLiquidSim(float timeStep, int delayTime);
	~FLIPLiquidSim() override;

	void update() override;

private:

	void _force() override;
	void _advect() override;
	void _diffuse() override;
	void _project() override;
};

