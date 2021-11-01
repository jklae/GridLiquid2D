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

void Trilinear::particleToGrid(XMFLOAT2 particlePos, XMFLOAT2 particleVel, vector<XMFLOAT2>& gridPos, vector<STATE>& gridState)
{
	XMFLOAT2 pos = particlePos;

	XMINT2 minIndex = _computeCenterMinMaxIndex(VALUE::MIN, pos);
	XMINT2 maxIndex = _computeCenterMinMaxIndex(VALUE::MAX, pos);

	XMFLOAT2 ratio = pos - gridPos[_INDEX(minIndex.x, minIndex.y)];
	_pCount[_INDEX(minIndex.x, minIndex.y)] += (1.0f - ratio.x) * (1.0f - ratio.y);			//if (gridState[_INDEX(minIndex.x, minIndex.y)] == STATE::FLUID)
	_pCount[_INDEX(minIndex.x, maxIndex.y)] += (1.0f - ratio.x) * ratio.y;				//	if (gridState[_INDEX(minIndex.x, maxIndex.y)] != STATE::FLUID)
	_pCount[_INDEX(maxIndex.x, minIndex.y)] += ratio.x * (1.0f - ratio.y);				//	if (gridState[_INDEX(maxIndex.x, minIndex.y)] != STATE::FLUID)
	_pCount[_INDEX(maxIndex.x, maxIndex.y)] += ratio.x * ratio.y;						//	if (gridState[_INDEX(maxIndex.x, minIndex.y)] != STATE::FLUID)

	XMFLOAT2 minMin_minMax = particleVel * (1.0f - ratio.x);
	XMFLOAT2 maxMin_maxMax = particleVel * ratio.x;
	XMFLOAT2 minMin = minMin_minMax * (1.0f - ratio.y);
	XMFLOAT2 minMax = minMin_minMax * ratio.y;
	XMFLOAT2 maxMin = maxMin_maxMax * (1.0f - ratio.y);
	XMFLOAT2 maxMax = maxMin_maxMax * ratio.y;

	_tempVel[_INDEX(minIndex.x, minIndex.y)] += minMin;
	_tempVel[_INDEX(minIndex.x, maxIndex.y)] += minMax;
	_tempVel[_INDEX(maxIndex.x, minIndex.y)] += maxMin;
	_tempVel[_INDEX(maxIndex.x, maxIndex.y)] += maxMax;

}

void Trilinear::setGridVelocity(vector<XMFLOAT2>& gridVel, vector<XMFLOAT2>& oldVel, int i, int j)
{
	if (_pCount[_INDEX(i, j)] > EPS_FLOAT)
	{
		gridVel[_INDEX(i, j)] = oldVel[_INDEX(i, j)] = _tempVel[_INDEX(i, j)] / _pCount[_INDEX(i, j)];
	}
	else
	{
		gridVel[_INDEX(i, j)] = oldVel[_INDEX(i, j)] = { 0.0f, 0.0f };
	}

	// Reset
	_tempVel[_INDEX(i, j)] = { 0.0f, 0.0f };
	_pCount[_INDEX(i, j)] = 0.0f;
}


												// For semi-Lagrangian and FLIP
XMFLOAT2 Trilinear::gridToParticle(XMFLOAT2 particlePos, vector<XMFLOAT2>& oldVel, vector<XMFLOAT2>& gridPos, vector<STATE>& gridState)
{
	// 2. 3.
	XMINT2 minIndex = _computeCenterMinMaxIndex(VALUE::MIN, particlePos);
	XMINT2 maxIndex = _computeCenterMinMaxIndex(VALUE::MAX, particlePos);

	XMFLOAT2 ratio = (particlePos - gridPos[_INDEX(minIndex.x, minIndex.y)]);

	float minMinRatio = (1.0f - ratio.x) * (1.0f - ratio.y);
	float minMaxRatio = (1.0f - ratio.x) * ratio.y;
	float maxMinRatio = ratio.x * (1.0f - ratio.y);
	float maxMaxRatio = ratio.x * ratio.y;

	//cout << "x ratio : " << 1 - ratio.x << ", " << ratio.x << endl;
	//cout << "y ratio : " << 1 - ratio.y << ", " << ratio.y << endl;

	XMFLOAT2 minMinVel = oldVel[_INDEX(minIndex.x, minIndex.y)];
	XMFLOAT2 minMaxVel = oldVel[_INDEX(minIndex.x, maxIndex.y)];
	XMFLOAT2 maxMinVel = oldVel[_INDEX(maxIndex.x, minIndex.y)];
	XMFLOAT2 maxMaxVel = oldVel[_INDEX(maxIndex.x, maxIndex.y)];

	/*cout << "min Min velocity : " << minMinVel.x << ", " << minMinVel.y << endl;
	cout << "min Max velocity : " << minMaxVel.x << ", " << minMaxVel.y << endl;
	cout << "max Min velocity : " << maxMinVel.x << ", " << maxMinVel.y << endl;
	cout << "max Max velocity : " << maxMaxVel.x << ", " << maxMaxVel.y << endl;*/

	return minMinVel * minMinRatio + minMaxVel * minMaxRatio + maxMinVel * maxMinRatio + maxMaxVel * maxMaxRatio;
}

// Different from _computeFaceMinMaxIndex().
// 1. Subtract the count of offset by 1.
// 2. Do not subtract particleStride from min, max calculation.
// 3. ceil maxIndex instead of floor.
// ------------------------------------------------------------------
// _PaintGrid() uses the face as the transition point.
// _updateParticlePosition() uses the center as the transition point.
XMINT2 Trilinear::_computeCenterMinMaxIndex(VALUE vState, XMFLOAT2 particlePos)
{
	// 2.
	switch (vState)
	{
	case VALUE::MIN:
		return { static_cast<int>(floor(particlePos.x)), static_cast<int>(floor(particlePos.y)) };
		break;
	case VALUE::MAX:
		// 3.
		return { static_cast<int>(ceil(particlePos.x)), static_cast<int>(ceil(particlePos.y)) };
		break;
	default:
		return { -1, -1 };
		break;
	}
}