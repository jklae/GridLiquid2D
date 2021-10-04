#pragma once
#include "GridFluidSim.h"

class FLIPLiquidSim : public GridFluidSim
{
public:
	FLIPLiquidSim(float timeStep, int delayTime);
	~FLIPLiquidSim() override;

	void update() override;

private:

	std::vector<DirectX::XMFLOAT2> _oldVel;

	std::vector<DirectX::XMFLOAT2> _picVel;
	std::vector<DirectX::XMFLOAT2> _flipVel;

	void _advect();
	void _saveVelocity();
	void _force();
	void _project();
	void _updateParticlePos(float dt) override;
};

