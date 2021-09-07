#pragma once
#include "GridFluidSim.h"

class FLIPLiquidSim : public GridFluidSim
{
public:
	FLIPLiquidSim(float timeStep, int delayTime);
	~FLIPLiquidSim() override;

	void update() override;

private:

	std::vector<DirectX::XMFLOAT2> oldVel;

	void _advect();
	void _force();
	void _project();
};

