#pragma once
#include "GridFluidSim.h"

class PICFLIPSim : public GridFluidSim
{
public:
	PICFLIPSim(float timeStep);
	~PICFLIPSim() override;

	void setFlipRatio(int value);

	void initialize() override;

private:

	std::vector<DirectX::XMFLOAT2> _oldVel;
	std::vector<DirectX::XMFLOAT2> _tempVel;
	std::vector<float> _pCount;

	float _flipRatio = 0.99f;

	void _update() override;
	void _advect();
	void _force();
	void _project();
	void _updateParticlePos() override;
};

