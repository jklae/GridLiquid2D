#include "Trilinear.h"

using namespace DirectX;
using namespace std;


Trilinear::Trilinear(GridData& index)
	:Interpolation(index)
{
}

Trilinear::~Trilinear()
{
}


												// For semi-Lagrangian and FLIP
XMFLOAT2 Trilinear::gridToParticle(XMFLOAT2 particlePos, vector<XMFLOAT2>& oldvel, vector<XMFLOAT2>& gridPos)
{
	// 2. 3.
	XMINT2 minIndex = _computeCenterMinMaxIndex(_VALUE::MIN, particlePos);
	XMINT2 maxIndex = _computeCenterMinMaxIndex(_VALUE::MAX, particlePos);

	float xRatio = (particlePos.x - gridPos[_INDEX(minIndex.x, minIndex.y)].x);
	float yRatio = (particlePos.y - gridPos[_INDEX(minIndex.x, minIndex.y)].y);

	XMFLOAT2 minMinVelocity = oldvel[_INDEX(minIndex.x, minIndex.y)];
	XMFLOAT2 minMaxVelocity = oldvel[_INDEX(minIndex.x, maxIndex.y)];
	XMFLOAT2 maxMinVelocity = oldvel[_INDEX(maxIndex.x, minIndex.y)];
	XMFLOAT2 maxMaxVelocity = oldvel[_INDEX(maxIndex.x, maxIndex.y)];

	// s0* (t0 * d0[IX(i0, j0)] + t1 * d0[IX(i0, j1)]) +
	//	s1 * (t0 * d0[IX(i1, j0)] + t1 * d0[IX(i1, j1)]);
	// minMinVelocity.x and minMinVelocity.x ​​can be different because they are "velocity", not position.
	return XMFLOAT2(
		_interpolation(_interpolation(minMinVelocity.x, minMaxVelocity.x, yRatio), _interpolation(maxMinVelocity.x, maxMaxVelocity.x, yRatio), xRatio),
		_interpolation(_interpolation(minMinVelocity.y, minMaxVelocity.y, yRatio), _interpolation(maxMinVelocity.y, maxMaxVelocity.y, yRatio), xRatio)
	);
}


// Different from _computeFaceMinMaxIndex().
// 1. Subtract the count of offset by 1.
// 2. Do not subtract particleStride from min, max calculation.
// 3. ceil maxIndex instead of floor.
// ------------------------------------------------------------------
// _PaintGrid() uses the face as the transition point.
// _updateParticlePosition() uses the center as the transition point.
XMINT2 Trilinear::_computeCenterMinMaxIndex(_VALUE vState, XMFLOAT2 particlePos)
{
	// 2.
	switch (vState)
	{
	case _VALUE::MIN:
		return { static_cast<int>(floor(particlePos.x)), static_cast<int>(floor(particlePos.y)) };
		break;
	case _VALUE::MAX:
		// 3.
		return { static_cast<int>(ceil(particlePos.x)), static_cast<int>(ceil(particlePos.y)) };
		break;
	default:
		return { -1, -1 };
		break;
	}
}

float Trilinear::_interpolation(float value1, float value2, float ratio)
{
	return value1 * (1.0f - ratio) + value2 * ratio;
}