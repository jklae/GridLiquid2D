#include "PICFLIPSim.h"

using namespace DirectX;
using namespace std;

PICFLIPSim::PICFLIPSim(GridData& index)
	:GridFluidSim(index)
{
	_initialize();
}

PICFLIPSim::~PICFLIPSim()
{
}

void PICFLIPSim::setFlipRatio(int value)
{
	_flipRatio = static_cast<float>(value) / 100.0f;
}

void PICFLIPSim::_initialize()
{
	GridFluidSim::_initialize();

	size_t vSize = static_cast<size_t>(_gridCount) * static_cast<size_t>(_gridCount);

	_oldVel.assign(vSize, { 0.0f, 0.0f });
	_tempVel.assign(vSize, { 0.0f, 0.0f });
	_pCount.assign(vSize, 0.0f);
}

void PICFLIPSim::_update()
{
	_timeInteg->computeGlobalTimeStep(_gridVelocity, _gridState);

	_advect();

	_force();
	_setFreeSurface(_gridVelocity);
	_setBoundary(_gridVelocity);

	_project();
	// Solve boundary condition again due to numerical errors in previous step
	_setFreeSurface(_gridVelocity);
	_setBoundary(_gridVelocity);
	_updateParticlePos();

	_paintGrid();
}

void PICFLIPSim::_advect()
{
	int N = _gridCount - 2;
	for (int i = 0; i < _particlePosition.size(); i++)
	{
		XMFLOAT2 pos = _particlePosition[i];

		XMINT2 minIndex = _computeCenterMinMaxIndex(_VALUE::MIN, pos);
		XMINT2 maxIndex = _computeCenterMinMaxIndex(_VALUE::MAX, pos);

		XMFLOAT2 ratio = pos - _gridPosition[_INDEX(minIndex.x, minIndex.y)];

		XMFLOAT2 minMin_minMax = _particleVelocity[i] * (1.0f - ratio.x);
		XMFLOAT2 maxMin_maxMax = _particleVelocity[i] * ratio.x;
		XMFLOAT2 minMin = minMin_minMax * (1.0f - ratio.y);
		XMFLOAT2 minMax = minMin_minMax * ratio.y;
		XMFLOAT2 maxMin = maxMin_maxMax * (1.0f - ratio.y);
		XMFLOAT2 maxMax = maxMin_maxMax * ratio.y;


		_tempVel[_INDEX(minIndex.x, minIndex.y)] += minMin;
		_pCount[_INDEX(minIndex.x, minIndex.y)] += (1.0f - ratio.x) * (1.0f - ratio.y);

		_tempVel[_INDEX(minIndex.x, maxIndex.y)] += minMax;
		_pCount[_INDEX(minIndex.x, maxIndex.y)] += (1.0f - ratio.x) * ratio.y;

		_tempVel[_INDEX(maxIndex.x, minIndex.y)] += maxMin;
		_pCount[_INDEX(maxIndex.x, minIndex.y)] += ratio.x * (1.0f - ratio.y);

		_tempVel[_INDEX(maxIndex.x, maxIndex.y)] += maxMax;
		_pCount[_INDEX(maxIndex.x, maxIndex.y)] += ratio.x * ratio.y;

	}

	for (int i = 0; i < _gridCount; i++)
	{
		for (int j = 0; j < _gridCount; j++)
		{
			if (_pCount[_INDEX(i, j)] > EPS_FLOAT)
			{
				_gridVelocity[_INDEX(i, j)] = _oldVel[_INDEX(i, j)] = _tempVel[_INDEX(i, j)] / _pCount[_INDEX(i, j)];
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

void PICFLIPSim::_force()
{
	assert(_timeInteg != nullptr);
	float dt;

	int N = _gridCount - 2;
	for (int i = 1; i <= N; i++)
	{
		for (int j = 1; j <= N; j++)
		{
			if (_gridState[_INDEX(i, j)] == STATE::FLUID)
			{
				dt = _timeInteg->computeGridTimeStep(_gridVelocity[_INDEX(i, j)]);
				_gridVelocity[_INDEX(i, j)].y -= 9.8f * dt;
			}
		}
	}
}


void PICFLIPSim::_project()
{
	int N = _gridCount - 2;
	for (int i = 1; i <= N; i++)
	{
		for (int j = 1; j <= N; j++)
		{
			_gridDivergence[_INDEX(i, j)] =
				0.5f * (_gridVelocity[_INDEX(i + 1, j)].x - _gridVelocity[_INDEX(i - 1, j)].x
					+ _gridVelocity[_INDEX(i, j + 1)].y - _gridVelocity[_INDEX(i, j - 1)].y);
			_gridPressure[_INDEX(i, j)] = 0.0f;
		}
	}

	_setBoundary(_gridDivergence);
	_setBoundary(_gridPressure);

	for (int iter = 0; iter < 200; iter++)
	{
		for (int i = 1; i <= N; i++)
		{
			for (int j = 1; j <= N; j++)
			{
				if (_gridState[_INDEX(i, j)] == STATE::FLUID)
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

	for (int i = 1; i <= N; i++)
	{
		for (int j = 1; j <= N; j++)
		{
			_gridVelocity[_INDEX(i, j)].x -= (_gridPressure[_INDEX(i + 1, j)] - _gridPressure[_INDEX(i - 1, j)]) * 0.5f;
			_gridVelocity[_INDEX(i, j)].y -= (_gridPressure[_INDEX(i, j + 1)] - _gridPressure[_INDEX(i, j - 1)]) * 0.5f;
		}
	}

}

void PICFLIPSim::_updateParticlePos()
{
	assert(_timeInteg != nullptr);
	float dt;

	int N = _gridCount - 2;
	for (int i = 0; i < _oldVel.size(); i++)
	{
		_oldVel[i] = _gridVelocity[i] - _oldVel[i];
	}

	// 0.5f is the correct value.
	// However, 0.5f is too 'correct', which causes particles to stick at the edges. (velocity is zero)
	float yMax = _gridPosition[_INDEX(0, N + 1)].y - 1.3f;
	float yMin = _gridPosition[_INDEX(0, 0)].y + 1.3f;
	float xMax = _gridPosition[_INDEX(N + 1, 0)].x - 1.3f;
	float xMin = _gridPosition[_INDEX(0, 0)].x + 1.3f;

	for (int i = 0; i < _particlePosition.size(); i++)
	{
		dt = _timeInteg->computeParticleTimeStep(_particleVelocity[i], i);

		XMFLOAT2 _picVel = _velocityInterpolation(_particlePosition[i], _gridVelocity);
		XMFLOAT2 _flipVel = _particleVelocity[i] + _velocityInterpolation(_particlePosition[i], _oldVel);

		_particleVelocity[i] = _picVel * (1 - _flipRatio) + _flipVel * _flipRatio;
		_particlePosition[i] += _particleVelocity[i] * dt;

		if (_particlePosition[i].x > xMax) _particlePosition[i].x = xMax;
		else if (_particlePosition[i].x < xMin) _particlePosition[i].x = xMin;

		if (_particlePosition[i].y > yMax) _particlePosition[i].y = yMax;
		else if (_particlePosition[i].y < yMin) _particlePosition[i].y = yMin;
	}
}