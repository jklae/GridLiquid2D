#pragma once
#include "GridLiquid.h"

class PICFLIP : public GridLiquid
{
public:
	PICFLIP(GridData& index, EX ex, float timeStep);
	~PICFLIP() override;

	void setFlipRatio(int value);

private:
	std::vector<DirectX::XMFLOAT2> _oldVel;

	float _flipRatio = 0.99f;

	void _initialize(EX ex) override;

	void _update() override;
	void _advect(int iter);
	void _force(int iter);
	void _project(int iter);

	void _updateParticlePos(int iter);
};

