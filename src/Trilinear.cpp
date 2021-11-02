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

	float minMinRatio =		 gridState[_INDEX(minIndex.x, minIndex.y)] == STATE::LIQUID	 ?	 (1.0f - ratio.x) * (1.0f - ratio.y)   : 0.0f;
	float minMaxRatio =		 gridState[_INDEX(minIndex.x, maxIndex.y)] == STATE::LIQUID	 ?	 (1.0f - ratio.x) * ratio.y			   : 0.0f;
	float maxMinRatio =		 gridState[_INDEX(maxIndex.x, minIndex.y)] == STATE::LIQUID	 ?	 ratio.x * (1.0f - ratio.y)			   : 0.0f;
	float maxMaxRatio =		 gridState[_INDEX(maxIndex.x, maxIndex.y)] == STATE::LIQUID	 ?	 ratio.x * ratio.y					   : 0.0f;

	float totalRatio = minMinRatio + minMaxRatio + maxMinRatio + maxMaxRatio;
	if (totalRatio > EPS_FLOAT)
	{
		minMinRatio /= totalRatio;
		minMaxRatio /= totalRatio;
		maxMinRatio /= totalRatio;
		maxMaxRatio /= totalRatio;
	}

	_pCount[_INDEX(minIndex.x, minIndex.y)] += minMinRatio;
	_pCount[_INDEX(minIndex.x, maxIndex.y)] += minMaxRatio;
	_pCount[_INDEX(maxIndex.x, minIndex.y)] += maxMinRatio;
	_pCount[_INDEX(maxIndex.x, maxIndex.y)] += maxMaxRatio;

	_tempVel[_INDEX(minIndex.x, minIndex.y)] += particleVel * minMinRatio;
	_tempVel[_INDEX(minIndex.x, maxIndex.y)] += particleVel * minMaxRatio;
	_tempVel[_INDEX(maxIndex.x, minIndex.y)] += particleVel * maxMinRatio;
	_tempVel[_INDEX(maxIndex.x, maxIndex.y)] += particleVel * maxMaxRatio;

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

	float minMinRatio =		gridState[_INDEX(minIndex.x, minIndex.y)] == STATE::LIQUID  	?	(1.0f - ratio.x) * (1.0f - ratio.y)  : 0.0f;
	float minMaxRatio =		gridState[_INDEX(minIndex.x, maxIndex.y)] == STATE::LIQUID		?	(1.0f - ratio.x) * ratio.y			 : 0.0f;
	float maxMinRatio =		gridState[_INDEX(maxIndex.x, minIndex.y)] == STATE::LIQUID		?	ratio.x * (1.0f - ratio.y)			 : 0.0f;
	float maxMaxRatio =		gridState[_INDEX(maxIndex.x, maxIndex.y)] == STATE::LIQUID		?	ratio.x * ratio.y				 	 : 0.0f;

	float totalRatio = minMinRatio + minMaxRatio + maxMinRatio + maxMaxRatio;
	if (totalRatio > EPS_FLOAT)
	{
		minMinRatio /= totalRatio;
		minMaxRatio /= totalRatio;
		maxMinRatio /= totalRatio;
		maxMaxRatio /= totalRatio;
	}
	
	//cout << "y ratio : " << minMaxRatio << ", " << maxMaxRatio << endl;
	//cout << "x ratio : " << minMinRatio << ", " << maxMinRatio << endl;

	XMFLOAT2 minMinVel = oldVel[_INDEX(minIndex.x, minIndex.y)];
	XMFLOAT2 minMaxVel = oldVel[_INDEX(minIndex.x, maxIndex.y)];
	XMFLOAT2 maxMinVel = oldVel[_INDEX(maxIndex.x, minIndex.y)];
	XMFLOAT2 maxMaxVel = oldVel[_INDEX(maxIndex.x, maxIndex.y)];

	/*cout << "min Min velocity : " << minMinVel.x << ", " << minMinVel.y << endl;
	cout << "min Max velocity : " << minMaxVel.x << ", " << minMaxVel.y << endl;
	cout << "max Min velocity : " << maxMinVel.x << ", " << maxMinVel.y << endl;
	cout << "max Max velocity : " << maxMaxVel.x << ", " << maxMaxVel.y << endl;*/

	return 
		minMinVel * minMinRatio + minMaxVel * minMaxRatio 
		+ maxMinVel * maxMinRatio + maxMaxVel * maxMaxRatio;
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