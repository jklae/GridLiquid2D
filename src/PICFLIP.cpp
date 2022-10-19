#include "PICFLIP.h"

using namespace DirectX;
using namespace std;
using namespace DXViewer::xmfloat2;
using namespace DXViewer::xmint2;

PICFLIP::PICFLIP(int x, int y, EX ex, float timeStep)
	:GridLiquid(x, y, timeStep)
{
	_initialize(ex);
}

PICFLIP::~PICFLIP()
{
}

void PICFLIP::setFlipRatio(int value)
{
	_flipRatio = static_cast<float>(value) / 100.0f;
}

void PICFLIP::_initialize(EX ex)
{
	GridLiquid::_initialize(ex);

	size_t vSize = static_cast<size_t>(_gridCount.x) * static_cast<size_t>(_gridCount.y);
	_oldVel.assign(vSize, { 0.0f, 0.0f });
	_tempVel.assign(vSize, { 0.0f, 0.0f });
	_pCount.assign(vSize, 0.0f);
}



void PICFLIP::_update()
{
	_advect();

	_force();
	_setBoundary(_gridVelocity);
	_setFreeSurface(_gridVelocity);

	_project();
	// Solve boundary condition again due to numerical errors in previous step.
	_setBoundary(_gridVelocity);
	_updateParticlePos();

	_paintLiquid();

	
}

void PICFLIP::_advect()
{
	XMINT2 N = _gridCount - 2;

	for (int i = 0; i < _particlePosition.size(); i++)
	{
		XMFLOAT2 pos = _particlePosition[i]; 
		XMFLOAT2 vel = _particleVelocity[i];

		XMINT2 minIndex = _computeCenterMinMaxIndex(VALUE::MIN, pos);
		XMINT2 maxIndex = _computeCenterMinMaxIndex(VALUE::MAX, pos);

		// Ratio of the Distance.
		XMFLOAT2 ratio = pos - _gridPosition[_INDEX(minIndex.x, minIndex.y)];

		// Since the grid spacing is 1 (i.e. the difference in distance is between 0 and 1), 
		// Normalization of the difference is not necessary.
		float minMinRatio = _gridState[_INDEX(minIndex.x, minIndex.y)] == STATE::LIQUID ? (1.0f - ratio.x) * (1.0f - ratio.y) : 0.0f;
		float minMaxRatio = _gridState[_INDEX(minIndex.x, maxIndex.y)] == STATE::LIQUID ? (1.0f - ratio.x) * ratio.y : 0.0f;
		float maxMinRatio = _gridState[_INDEX(maxIndex.x, minIndex.y)] == STATE::LIQUID ? ratio.x * (1.0f - ratio.y) : 0.0f;
		float maxMaxRatio = _gridState[_INDEX(maxIndex.x, maxIndex.y)] == STATE::LIQUID ? ratio.x * ratio.y : 0.0f;

		// Normalization of the ratio.
		float totalRatio = minMinRatio + minMaxRatio + maxMinRatio + maxMaxRatio;
		if (totalRatio > FLT_EPSILON)
		{
			minMinRatio /= totalRatio;
			minMaxRatio /= totalRatio;
			maxMinRatio /= totalRatio;
			maxMaxRatio /= totalRatio;
		}

		// Count the number of particles affecting _INDEX(i, j).
		_pCount[_INDEX(minIndex.x, minIndex.y)] += minMinRatio;
		_pCount[_INDEX(minIndex.x, maxIndex.y)] += minMaxRatio;
		_pCount[_INDEX(maxIndex.x, minIndex.y)] += maxMinRatio;
		_pCount[_INDEX(maxIndex.x, maxIndex.y)] += maxMaxRatio;

		// Add the velocity multiplied by the ratio.
		_tempVel[_INDEX(minIndex.x, minIndex.y)] += vel * minMinRatio;
		_tempVel[_INDEX(minIndex.x, maxIndex.y)] += vel * minMaxRatio;
		_tempVel[_INDEX(maxIndex.x, minIndex.y)] += vel * maxMinRatio;
		_tempVel[_INDEX(maxIndex.x, maxIndex.y)] += vel * maxMaxRatio;
	}

	for (int j = 0; j < _gridCount.y; j++)
	{
		for (int i = 0; i < _gridCount.x; i++)
		{

			if (_pCount[_INDEX(i, j)] > FLT_EPSILON)
			{
				// Average
				_gridVelocity[_INDEX(i, j)] = _oldVel[_INDEX(i, j)] = _tempVel[_INDEX(i, j)] / (_pCount[_INDEX(i, j)]);
			}
			else
			{
				_gridVelocity[_INDEX(i, j)] = _oldVel[_INDEX(i, j)] = { 0.0f, 0.0f };
			}

			// Reset
			_tempVel[_INDEX(i, j)] = { 0.0f, 0.0f };
			_pCount[_INDEX(i, j)] = 0.0f;
		}
	}
}

void PICFLIP::_force()
{
	float dt = _timeStep;
	XMINT2 N = _gridCount - 2;

	for (int j = 1; j <= N.y; j++)
	{
		for (int i = 1; i <= N.x; i++)
		{
			if (_gridState[_INDEX(i, j)] == STATE::LIQUID)
			{
				// Gravity
				_gridVelocity[_INDEX(i, j)].y -= 9.8f * dt;
			}

		}
	}
}


void PICFLIP::_project()
{
	XMINT2 N = _gridCount - 2;

	// Initialize the divergence and pressure.
	for (int j = 1; j <= N.y; j++)
	{
		for (int i = 1; i <= N.x; i++)
		{
			_gridDivergence[_INDEX(i, j)] =
				0.5f * (_gridVelocity[_INDEX(i + 1, j)].x - _gridVelocity[_INDEX(i - 1, j)].x
					+ _gridVelocity[_INDEX(i, j + 1)].y - _gridVelocity[_INDEX(i, j - 1)].y);

			_gridPressure[_INDEX(i, j)] = 0.0f;

		}
	}

	_setBoundary(_gridDivergence);
	_setBoundary(_gridPressure);
	
	// Gauss-Seidel method
	for (int iter = 0; iter < 200; iter++)
	{
		
		for (int j = 1; j <= N.y; j++)
		{
			for (int i = 1; i <= N.x; i++)
			{

				if (_gridState[_INDEX(i, j)] == STATE::LIQUID)
				{
					_gridPressure[_INDEX(i, j)] =
						(
							_gridDivergence[_INDEX(i, j)] -
							(_gridPressure[_INDEX(i + 1, j)] + _gridPressure[_INDEX(i - 1, j)] +
								_gridPressure[_INDEX(i, j + 1)] + _gridPressure[_INDEX(i, j - 1)])
							) / -4.0f;

				}
			
			}

		}
		_setBoundary(_gridPressure);
	}

	for (int j = 1; j <= N.y; j++)
	{
		for (int i = 1; i <= N.x; i++)
		{
			if (_gridState[_INDEX(i, j)] == STATE::LIQUID)
			{
				// Apply the pressure force to the velocity.
				_gridVelocity[_INDEX(i, j)].x -= (_gridPressure[_INDEX(i + 1, j)] - _gridPressure[_INDEX(i - 1, j)]) * 0.5f;
				_gridVelocity[_INDEX(i, j)].y -= (_gridPressure[_INDEX(i, j + 1)] - _gridPressure[_INDEX(i, j - 1)]) * 0.5f;

			}
		}
	}

}

void PICFLIP::_updateParticlePos()
{
	float dt = _timeStep;
	XMINT2 N = _gridCount - 2;

	for (int i = 0; i < _oldVel.size(); i++)
	{
		_oldVel[i] = _gridVelocity[i] - _oldVel[i];
	}

	float yMax = _gridPosition[_INDEX(0, N.y + 1)].y - 0.5f;
	float yMin = _gridPosition[_INDEX(0, 0)].y + 0.5f;
	float xMax = _gridPosition[_INDEX(N.x + 1, 0)].x - 0.5f;
	float xMin = _gridPosition[_INDEX(0, 0)].x + 0.5f;

	for (int i = 0; i < _particlePosition.size(); i++)
	{
		XMFLOAT2 _picVel = gridToParticle(_particlePosition[i], _gridVelocity);
		XMFLOAT2 _flipVel = _particleVelocity[i] + gridToParticle(_particlePosition[i], _oldVel);

		// PIC/FLIP blending
		_particleVelocity[i] = _picVel * (1 - _flipRatio) + _flipVel * _flipRatio;
		_particlePosition[i] += _particleVelocity[i] * dt;

		// Boundary condition
		if (_particlePosition[i].x > xMax) _particlePosition[i].x = xMax;
		else if (_particlePosition[i].x < xMin) _particlePosition[i].x = xMin;

		if (_particlePosition[i].y > yMax) _particlePosition[i].y = yMax;
		else if (_particlePosition[i].y < yMin) _particlePosition[i].y = yMin;
	}
}
