#include "FLIPLiquidSim.h"

using namespace DirectX;
using namespace std;

FLIPLiquidSim::FLIPLiquidSim(float timeStep, int delayTime)
	:GridFluidSim::GridFluidSim(timeStep, delayTime)
{
}

FLIPLiquidSim::~FLIPLiquidSim()
{
}

void FLIPLiquidSim::update()
{

	_advect();
	_saveVelocity();
	_force();
	_setBoundary(_gridVelocity);
	_project();
	_updateParticlePos();

	_paintGrid();
}

void FLIPLiquidSim::_force()
{
	int N = _gridCount - 2;
	for (int i = 1; i <= N; i++)
	{
		for (int j = 1; j <= N; j++)
		{
			if (_gridState[_INDEX(i, j)] == _STATE::FLUID) _gridVelocity[_INDEX(i, j)].y -= 4.0f * _timeStep;
		}
	}
}

void FLIPLiquidSim::_advect()
{
	int N = _gridCount - 2;
	vector<XMFLOAT2> tempVel;
	vector<float> pCount;

	for (int i = 0; i < _gridCount; i++)
	{
		for (int j = 0; j < _gridCount; j++)
		{
			tempVel.push_back(XMFLOAT2(0.0f, 0.0f));
			pCount.push_back(0.0f);
		}
	}

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


		tempVel[_INDEX(minXIndex, minYIndex)] += { minMinX, minMinY };
		pCount[_INDEX(minXIndex, minYIndex)] += (1.0f - xRatio) * (1.0f - yRatio);

		tempVel[_INDEX(minXIndex, maxYIndex)] += { minMaxX, minMaxY };
		pCount[_INDEX(minXIndex, maxYIndex)] += (1.0f - xRatio) * yRatio;

		tempVel[_INDEX(maxXIndex, minYIndex)] += { maxMinX, maxMinY };
		pCount[_INDEX(maxXIndex, minYIndex)] += xRatio * (1.0f - yRatio);

		tempVel[_INDEX(maxXIndex, maxYIndex)] += { maxMaxX, maxMaxY };
		pCount[_INDEX(maxXIndex, maxYIndex)] += xRatio * yRatio;

	}

	float eps = 0.000001f;
	for (int i = 0; i < _gridCount; i++)
	{
		for (int j = 0; j < _gridCount; j++)
		{
			if (pCount[_INDEX(i, j)] > eps)
			{
				_gridVelocity[_INDEX(i, j)] = tempVel[_INDEX(i, j)] / pCount[_INDEX(i, j)];
			}
		}
	}
}

void FLIPLiquidSim::_saveVelocity()
{
	_oldVel = _gridVelocity;
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
				_gridPressure[_INDEX(i, j)] =
					(
						_gridDivergence[_INDEX(i, j)] -
						(_gridPressure[_INDEX(i + 1, j)] + _gridPressure[_INDEX(i - 1, j)] +
							_gridPressure[_INDEX(i, j + 1)] + _gridPressure[_INDEX(i, j - 1)])
						) / -4.0f;
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

void FLIPLiquidSim::_updateParticlePos()
{
	int N = _gridCount - 2;
	for (int i = 0; i < _oldVel.size(); i++)
	{
		_oldVel[i] = _gridVelocity[i] - _oldVel[i];
	}

	float yMax = _gridPosition[_INDEX(0, N + 1)].y - 0.5f;
	float yMin = _gridPosition[_INDEX(0, 0)].y + 0.5f;
	float xMax = _gridPosition[_INDEX(N + 1, 0)].x - 0.5f;
	float xMin = _gridPosition[_INDEX(0, 0)].x + 0.5f;

	for (int i = 0; i < _particlePosition.size(); i++)
	{
		_particleVelocity[i] += _velocityInterpolation(_particlePosition[i], _oldVel);
		_particlePosition[i] += _particleVelocity[i] * _timeStep;

		if (_particlePosition[i].x > xMax) _particlePosition[i].x = xMax;
		else if (_particlePosition[i].x < xMin) _particlePosition[i].x = xMin;

		if (_particlePosition[i].y > yMax) _particlePosition[i].y = yMax;
		else if (_particlePosition[i].y < yMin) _particlePosition[i].y = yMin;
	}
}