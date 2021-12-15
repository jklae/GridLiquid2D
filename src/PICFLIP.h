#pragma once
#include "GridLiquid.h"

class PICFLIP : public GridLiquid
{
public:
	PICFLIP(int x, EX ex, float timeStep);
	~PICFLIP() override;

	void setFlipRatio(int value);

private:
	std::vector<DirectX::XMFLOAT2> _oldVel;
	std::vector<DirectX::XMFLOAT2> _tempVel;
	std::vector<float> _pCount;

	float _flipRatio = 0.0f;

	void _initialize(EX ex) override;

	void _update() override;
	void _advect();
	void _force();
	void _project();

	void _updateParticlePos();
};

