#include "PICLiquidSimulation.h"

using namespace DirectX;
using namespace std;

PICLiquidSimulation::PICLiquidSimulation(float timeStep)
	:EulerianSimulation(timeStep)
{
}

PICLiquidSimulation::~PICLiquidSimulation()
{
}

void PICLiquidSimulation::_update()
{

	//_project();
	_advect();
	_force();

	_project();
	_updateParticlePosition();
	_paintGrid();
}

void PICLiquidSimulation::_force()
{
	int N = _gridCount - 2;
	for (int i = 1; i <= N; i++)
	{
		for (int j = 1; j <= N; j++)
		{
			if (_gridState[_INDEX(i, j)] == STATE::FLUID) _gridVelocity[_INDEX(i, j)].y -= 1.0f * _timeStep;
		}
	}
}

void PICLiquidSimulation::_advect()
{
	int N = _gridCount - 2;
	for (int i = 1; i <= N; i++)
	{
		for (int j = 1; j <= N; j++)
		{
			_gridVelocity[_INDEX(i, j)].x = 0.0f;
			_gridVelocity[_INDEX(i, j)].y = 0.0f;
		}
	}

	for (int i = 0; i < _particlePosition.size(); i++)
	{
		XMFLOAT2 pos = _particlePosition[i];

		int minXIndex = _computeCenterMinMaxIndex(VALUE::MIN, AXIS::X, pos);
		int minYIndex = _computeCenterMinMaxIndex(VALUE::MIN, AXIS::Y, pos);
		int maxXIndex = _computeCenterMinMaxIndex(VALUE::MAX, AXIS::X, pos);
		int maxYIndex = _computeCenterMinMaxIndex(VALUE::MAX, AXIS::Y, pos);

		float xRatio = 0.5f;
		float yRatio = 0.5f;

		float ppX1 = _interpolation(0.002f, 0.003f, yRatio);
		float ppX2 = _interpolation(0.025f, 0.021f, yRatio);
		float pX = _interpolation(ppX1, ppX2, xRatio);
		float pY = _interpolation(_interpolation(-0.09f, 0.006f, yRatio), _interpolation(0.02f, 0.08f, yRatio), xRatio);

		float minMin_minMax_X = pX * (1.0f - xRatio);
		float maxMin_maxMax_X = pX * xRatio;
		float minMinX = minMin_minMax_X * (1.0f - yRatio);
		float minMaxX = minMin_minMax_X * yRatio;
		float maxMinX = maxMin_maxMax_X * (1.0f - yRatio);
		float maxMaxX = maxMin_maxMax_X * yRatio;

		float minMin_minMax_Y = pY * (1.0f - xRatio);
		float maxMin_maxMax_Y = pY * xRatio;
		float minMinY = minMin_minMax_Y * (1.0f - yRatio);
		float minMaxY = minMin_minMax_Y * yRatio;
		float maxMinY = minMin_minMax_Y * (1.0f - yRatio);
		float maxMaxY = minMin_minMax_Y * yRatio;

		//cout << ppX1 << ", " << ppX2 << ", " << minMin_minMax_X << ", " << maxMin_maxMax_X << endl;
		//cout << minMinX << ", " << minMinY << ", " << minMaxX << ", " << minMaxY << endl;
		//cout << maxMinX << ", " << maxMinY << ", " << maxMaxX << ", " << maxMaxY << endl;

		_gridVelocity[_INDEX(minXIndex, minYIndex)].x = _particleVelocity[i].x;
		_gridVelocity[_INDEX(minXIndex, minYIndex)].y = _particleVelocity[i].y;

		/*_gridVelocity[_INDEX(minXIndex, maxYIndex)].x = minMaxX;
		_gridVelocity[_INDEX(minXIndex, maxYIndex)].y = minMaxY;

		_gridVelocity[_INDEX(maxXIndex, minYIndex)].x = maxMinX;
		_gridVelocity[_INDEX(maxXIndex, minYIndex)].y = maxMinY;

		_gridVelocity[_INDEX(maxXIndex, maxYIndex)].x = maxMaxX;
		_gridVelocity[_INDEX(maxXIndex, maxYIndex)].y = maxMaxY;*/
		

		//_interpolation(_interpolation(minMinVelocity.x, minMaxVelocity.x, yRatio), _interpolation(maxMinVelocity.x, maxMaxVelocity.x, yRatio), xRatio),
		//_interpolation(_interpolation(minMinVelocity.y, minMaxVelocity.y, yRatio), _interpolation(maxMinVelocity.y, maxMaxVelocity.y, yRatio), xRatio)
	}

	_setBoundary(_gridVelocity);
}

void PICLiquidSimulation::_diffuse()
{

}

void PICLiquidSimulation::_project()
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