#include "EulerianLiquidSimulation.h"

using namespace DirectX;
using namespace std;

EulerianLiquidSimulation::EulerianLiquidSimulation()
{
}

EulerianLiquidSimulation::~EulerianLiquidSimulation()
{
}

void EulerianLiquidSimulation::_update(float timestep)
{
	_force(timestep);

	_project(timestep);
	_advect(timestep);

	_project(timestep);

	_updateParticlePosition(timestep);
	_paintGrid();
}

void EulerianLiquidSimulation::_force(float timestep)
{
	int N = _gridCount - 2;
	float tstep = static_cast<float>(timestep);
	for (int i = 1; i <= N; i++)
	{
		for (int j = 1; j <= N; j++)
		{
			//0.0000005f
//_gridVelocity[_INDEX(i, j)].x -= 2.8f * 1.0f * tstep;
			if (_gridState[_INDEX(i, j)] == STATE::FLUID) _gridVelocity[_INDEX(i, j)].y -= 0.1f * tstep;
		}
	}
}

void EulerianLiquidSimulation::_advect(float timestep)
{
	float tstep = static_cast<float>(timestep);
	int N = _gridCount - 2;
	float t0step = tstep * N;

	float yMax = _gridPosition[_INDEX(0, N + 1)].y - 0.5f;
	float yMin = _gridPosition[_INDEX(0, 0)].y + 0.5f;
	float xMax = _gridPosition[_INDEX(N + 1, 0)].x - 0.5f;
	float xMin = _gridPosition[_INDEX(0, 0)].x + 0.5f;

	vector<XMFLOAT2> oldVelocity = _gridVelocity;

	for (int i = 1; i <= N; i++)
	{
		for (int j = 1; j <= N; j++)
		{
			/*float magnitude = sqrtf(powf(_gridVelocity[i].x, 2.0f) + powf(_gridVelocity[i].y, 2.0f));

			if (magnitude > 0.05f && magnitude < 0.1f)
				t0step /= 2.0f;
			else if (magnitude >= 0.1f)
				t0step /= 4.0f;
			else
				t0step /= 1.0f;*/

			XMFLOAT2 backPos =
				XMFLOAT2(
					_gridPosition[_INDEX(i, j)].x - tstep * oldVelocity[_INDEX(i, j)].x,
					_gridPosition[_INDEX(i, j)].y - tstep * oldVelocity[_INDEX(i, j)].y
				);
			if (backPos.x > xMax) backPos.x = xMax;
			else if (backPos.x < xMin) backPos.x = xMin;

			if (backPos.y > yMax) backPos.y = yMax;
			else if (backPos.y < yMin) backPos.y = yMin;


			_gridVelocity[_INDEX(i, j)] = _velocityInterpolation(backPos, oldVelocity);
		}
	}
	_setBoundary(_gridVelocity);
}

void EulerianLiquidSimulation::_diffuse(float timestep)
{

}

void EulerianLiquidSimulation::_project(float timestep)
{
	float tstep = static_cast<float>(timestep);

	int N = _gridCount - 2;
	for (int i = 1; i <= N; i++)
	{
		for (int j = 1; j <= N; j++)
		{
			_gridDivergence[_INDEX(i, j)] =
				0.5f * (_gridVelocity[_INDEX(i + 1, j)].x - _gridVelocity[_INDEX(i - 1, j)].x
					+ _gridVelocity[_INDEX(i, j + 1)].y - _gridVelocity[_INDEX(i, j - 1)].y);
			_gridPressure[_INDEX(i, j)] = 0.0f;
			//if (_gridState[_INDEX(i, j)] == STATE::FLUID) _gridPressure[_INDEX(i, j)] = 0.01f;
			//else  _gridPressure[_INDEX(i, j)] = 0.0f;
			//printf("%f  ", _gridDivergence[_INDEX(i, j)]);
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
