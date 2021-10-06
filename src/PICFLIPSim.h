#pragma once
#include "GridFluidSim.h"

class PICFLIPSim : public GridFluidSim
{
public:
	PICFLIPSim(float timeStep);
	~PICFLIPSim() override;

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

