#include "PICLiquidSim.h"

using namespace DirectX;
using namespace std;

PICLiquidSim::PICLiquidSim(float timeStep, int delayTime)
	:GridFluidSim::GridFluidSim(timeStep, delayTime)
{
}

PICLiquidSim::~PICLiquidSim()
{
}

void PICLiquidSim::update()
{

	//_project();
	_advect();
	_force();

	_project();
	_updateParticlePosition();
	_paintGrid();
}

void PICLiquidSim::_force()
{
	int N = _gridCount - 2;
	for (int i = 1; i <= N; i++)
	{
		for (int j = 1; j <= N; j++)
		{
			if (_gridState[_INDEX(i, j)] == _STATE::FLUID) _gridVelocity[_INDEX(i, j)].y -= 1.0f * _timeStep;
		}
	}
}

void PICLiquidSim::_advect()
{
	int N = _gridCount - 2;
	vector<XMFLOAT2> pCount;
	vector<XMFLOAT2> tempVel;

	for (int i = 0; i < _gridCount; i++)
	{
		for (int j = 0; j < _gridCount; j++)
		{
			pCount.push_back(XMFLOAT2(0.0f, 0.0f));
			tempVel.push_back(XMFLOAT2(0.0f, 0.0f));
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

		/*cout << _particleVelocity[i].x << ", " << _particleVelocity[i].y << endl;
		_gridVelocity[_INDEX(maxXIndex, maxYIndex)].y += maxMaxY* 0.001f;
		cout << minMinX << ", " << minMinY << ", " << minMaxX << ", " << minMaxY << endl;
		cout << maxMinX << ", " << maxMinY << ", " << maxMaxX << ", " << maxMaxY << endl;*/

		tempVel[_INDEX(minXIndex, minYIndex)].x += minMinX;
		pCount[_INDEX(minXIndex, minYIndex)].x += (1.0f - xRatio) * (1.0f - yRatio);
		tempVel[_INDEX(minXIndex, minYIndex)].y += minMinY;
		pCount[_INDEX(minXIndex, minYIndex)].y += (1.0f - xRatio) * (1.0f - yRatio);

		tempVel[_INDEX(minXIndex, maxYIndex)].x += minMaxX;
		pCount[_INDEX(minXIndex, maxYIndex)].x += (1.0f - xRatio) * yRatio;
		tempVel[_INDEX(minXIndex, maxYIndex)].y += minMaxY;
		pCount[_INDEX(minXIndex, maxYIndex)].y += (1.0f - xRatio) * yRatio;

		tempVel[_INDEX(maxXIndex, minYIndex)].x += maxMinX;
		pCount[_INDEX(maxXIndex, minYIndex)].x += xRatio * (1.0f - yRatio);
		tempVel[_INDEX(maxXIndex, minYIndex)].y += maxMinY;
		pCount[_INDEX(maxXIndex, minYIndex)].y += xRatio * (1.0f - yRatio);

		tempVel[_INDEX(maxXIndex, maxYIndex)].x += maxMaxX;
		pCount[_INDEX(maxXIndex, maxYIndex)].x += xRatio * yRatio;
		tempVel[_INDEX(maxXIndex, maxYIndex)].y += maxMaxY;
		pCount[_INDEX(maxXIndex, maxYIndex)].y += xRatio * yRatio;
		

		//_interpolation(_interpolation(minMinVelocity.x, minMaxVelocity.x, yRatio), _interpolation(maxMinVelocity.x, maxMaxVelocity.x, yRatio), xRatio),
		//_interpolation(_interpolation(minMinVelocity.y, minMaxVelocity.y, yRatio), _interpolation(maxMinVelocity.y, maxMaxVelocity.y, yRatio), xRatio)
	}

	float eps = 0.000001f;
	for (int i = 0; i < _gridCount; i++)
	{
		for (int j = 0; j < _gridCount; j++)
		{
			if (pCount[_INDEX(i, j)].x > eps)
			{
				_gridVelocity[_INDEX(i, j)].x = tempVel[_INDEX(i, j)].x / pCount[_INDEX(i, j)].x;
			}

			if (pCount[_INDEX(i, j)].y > eps)
			{
				_gridVelocity[_INDEX(i, j)].y = tempVel[_INDEX(i, j)].y / pCount[_INDEX(i, j)].y;
			}
		}
	}

	_setBoundary(_gridVelocity);
}

void PICLiquidSim::_diffuse()
{

}

void PICLiquidSim::_project()
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
	_setBoundary(_gridVelocity);

}