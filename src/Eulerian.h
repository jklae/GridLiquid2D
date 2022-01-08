#pragma once
#include "GridLiquid.h"

class Eulerian : public GridLiquid
{
public:
	Eulerian(int x, int y, EX ex, float timeStep);
	~Eulerian() override;

private:
	void _update() override;
	void _force();
	void _advect();
	void _project();

	void _updateParticlePos();
};

