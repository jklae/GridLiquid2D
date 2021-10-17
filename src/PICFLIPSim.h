#pragma once
#include "GridFluidSim.h"

class PICFLIPSim : public GridFluidSim
{
public:
	PICFLIPSim(GridData& index);
	~PICFLIPSim() override;

	void setFlipRatio(int value);

private:
	ReverseInterpolation reInterp;

	std::vector<DirectX::XMFLOAT2> _oldVel;
	std::vector<DirectX::XMFLOAT2> _tempVel;
	std::vector<float> _tempStep;
	std::vector<float> _pCount;

	float _flipRatio = 0.99f;

	void _initialize() override;

	void _update() override;
	void _advect();
	void _force();
	void _project();

	void _updateParticlePos() override;
};

