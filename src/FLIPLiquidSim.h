#pragma once
#include "GridFluidSim.h"

class FLIPLiquidSim : public GridFluidSim
{
public:
	FLIPLiquidSim(float timeStep);
	~FLIPLiquidSim() override;

	void update() override;
	void initialize() override;

private:

	std::vector<DirectX::XMFLOAT2> _oldVel;
	std::vector<DirectX::XMFLOAT2> _tempVel;
	std::vector<float> _pCount;

	void _advect();
	void _force();
	void _project();
	void _updateParticlePos(float dt) override;
};

