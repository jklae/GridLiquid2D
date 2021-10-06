#include "FLIPLiquidSim.h"

using namespace DirectX;
using namespace std;

FLIPLiquidSim::FLIPLiquidSim(float timeStep)
	:GridFluidSim::GridFluidSim(timeStep)
{
}

FLIPLiquidSim::~FLIPLiquidSim()
{
}

void FLIPLiquidSim::initialize()
{
	GridFluidSim::initialize();

	size_t vSize = static_cast<size_t>(_gridCount) * static_cast<size_t>(_gridCount);

	_oldVel.assign(vSize, { 0.0f, 0.0f });
	_tempVel.assign(vSize, { 0.0f, 0.0f });
	_pCount.assign(vSize, 0.0f);
}

void FLIPLiquidSim::update()
{
	clock_t startTime = clock();

	_advect();

	_force();
	_setBoundary(_gridVelocity);
	_setFreeSurface(_gridVelocity);

	_project();
	// Solve boundary condition again due to numerical errors in previous step
	_setBoundary(_gridVelocity);
	_setFreeSurface(_gridVelocity);
	_updateParticlePos(0.0f);

	_paintGrid();


	clock_t endTime = clock();

	// ms
	clock_t elapsed = endTime - startTime;

	//std::cout << elapsed << std::endl;
}

void FLIPLiquidSim::_force()
{
	int N = _gridCount - 2;
	for (int i = 1; i <= N; i++)
	{
		for (int j = 1; j <= N; j++)
		{
			if (_gridState[_INDEX(i, j)] == _STATE::FLUID)
			{
				_gridVelocity[_INDEX(i, j)].y -= 9.8f * _timeStep;
			}
		}
	}
}

void FLIPLiquidSim::_advect()
{
	int N = _gridCount - 2;



	for (int i = 0; i < _particlePosition.size(); i++)
	{
		XMFLOAT2 pos = _particlePosition[i];

		int minXIndex = _computeCenterMinMaxIndex(_VALUE::MIN, _AXIS::X, pos);
		int minYIndex = _computeCenterMinMaxIndex(_VALUE::MIN, _AXIS::Y, pos);
		int maxXIndex = _computeCenterMinMaxIndex(_VALUE::MAX, _AXIS::X, pos);
		int maxYIndex = _computeCenterMinMaxIndex(_VALUE::MAX, _AXIS::Y, pos);

		float xRatio = (pos.x - _gridPosition[_INDEX(minXIndex, minYIndex)].x);
		float yRatio = (pos.y - _gridPosition[_INDEX(minXIndex, minYIndex)].y);

		float minMin_minMax_X = _particleVelocity[i].x * (1.0f - xRatio);
		float maxMin_maxMax_X = _particleVelocity[i].x * xRatio;
		float minMinX = minMin_minMax_X * (1.0f - yRatio);
		float minMaxX = minMin_minMax_X * yRatio;
		float maxMinX = maxMin_maxMax_X * (1.0f - yRatio);
		float maxMaxX = maxMin_maxMax_X * yRatio;

		float minMin_minMax_Y = _particleVelocity[i].y * (1.0f - xRatio);
		float maxMin_maxMax_Y = _particleVelocity[i].y * xRatio;
		float minMinY = minMin_minMax_Y * (1.0f - yRatio);
		float minMaxY = minMin_minMax_Y * yRatio;
		float maxMinY = maxMin_maxMax_Y * (1.0f - yRatio);
		float maxMaxY = maxMin_maxMax_Y * yRatio;


		_tempVel[_INDEX(minXIndex, minYIndex)] += { minMinX, minMinY };
		_pCount[_INDEX(minXIndex, minYIndex)] += (1.0f - xRatio) * (1.0f - yRatio);

		_tempVel[_INDEX(minXIndex, maxYIndex)] += { minMaxX, minMaxY };
		_pCount[_INDEX(minXIndex, maxYIndex)] += (1.0f - xRatio) * yRatio;

		_tempVel[_INDEX(maxXIndex, minYIndex)] += { maxMinX, maxMinY };
		_pCount[_INDEX(maxXIndex, minYIndex)] += xRatio * (1.0f - yRatio);

		_tempVel[_INDEX(maxXIndex, maxYIndex)] += { maxMaxX, maxMaxY };
		_pCount[_INDEX(maxXIndex, maxYIndex)] += xRatio * yRatio;

	}

	float eps = 0.000001f;
	for (int i = 0; i < _gridCount; i++)
	{
		for (int j = 0; j < _gridCount; j++)
		{
			if (_pCount[_INDEX(i, j)] > eps)
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

void FLIPLiquidSim::_project()
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

	for (int iter = 0; iter < 20; iter++)
	{
		for (int i = 1; i <= N; i++)
		{
			for (int j = 1; j <= N; j++)
			{
				if (_gridState[_INDEX(i, j)] == _STATE::FLUID)
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

void FLIPLiquidSim::_updateParticlePos(float dt)
{
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
		XMFLOAT2 _picVel = _velocityInterpolation(_particlePosition[i], _gridVelocity);
		XMFLOAT2 _flipVel = _particleVelocity[i] + _velocityInterpolation(_particlePosition[i], _oldVel);

		_particleVelocity[i] = _picVel * 0.02f + _flipVel * 0.98f;
		_particlePosition[i] += _particleVelocity[i] * _timeStep;

		if (_particlePosition[i].x > xMax) _particlePosition[i].x = xMax;
		else if (_particlePosition[i].x < xMin) _particlePosition[i].x = xMin;

		if (_particlePosition[i].y > yMax) _particlePosition[i].y = yMax;
		else if (_particlePosition[i].y < yMin) _particlePosition[i].y = yMin;
	}
}