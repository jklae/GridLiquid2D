#pragma once
#include "GridFluidSim.h"

class PICFLIPSim : public GridFluidSim
{
public:
	PICFLIPSim(GridData& index);
	~PICFLIPSim() override;

	void setFlipRatio(int value);

private:

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

	void _inverseInterpolation(DirectX::XMFLOAT2 data, std::vector<DirectX::XMFLOAT2>& temp, DirectX::XMFLOAT2 ratio, DirectX::XMINT2 minIndex, DirectX::XMINT2 maxIndex);
};

