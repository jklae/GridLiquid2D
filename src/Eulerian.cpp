#include "Eulerian.h"

using namespace DirectX;
using namespace std;

Eulerian::Eulerian(GridData& index, EX ex, float timeStep)
	:GridLiquid(index, timeStep)
{
	_initialize(ex);
}

Eulerian::~Eulerian()
{
}

void Eulerian::_update()
{
	_force();

	//_project();
	_advect();

	_project();
	_updateParticlePos();
	_paintLiquid();

}

void Eulerian::_force()
{
	float dt = _timeStep;

	int N = _gridCount - 2;
	for (int i = 1; i <= N; i++)
	{
		for (int j = 1; j <= N; j++)
		{
			if (_gridState[_INDEX(i, j)] == STATE::LIQUID)
			{
				_gridVelocity[_INDEX(i, j)].y -= 9.8f * dt;
			}
			else
			{
				_gridVelocity[_INDEX(i, j)] = { 0.0f, 0.0f };
			}
		}
	}
	_setBoundary(_gridVelocity);
	_setFreeSurface(_gridVelocity);
}

void Eulerian::_advect()
{
	float dt = _timeStep;

	int N = _gridCount - 2;

	float yMax = _gridPosition[_INDEX(0, N + 1)].y - 0.5f;
	float yMin = _gridPosition[_INDEX(0, 0)].y + 0.5f;
	float xMax = _gridPosition[_INDEX(N + 1, 0)].x - 0.5f;
	float xMin = _gridPosition[_INDEX(0, 0)].x + 0.5f;

	vector<XMFLOAT2> oldVelocity = _gridVelocity;

	for (int i = 1; i <= N; i++)
	{
		for (int j = 1; j <= N; j++)
		{
			if (_gridState[_INDEX(i, j)] == STATE::LIQUID)
			{
				XMFLOAT2 backPos =
					XMFLOAT2(
						_gridPosition[_INDEX(i, j)].x - dt * oldVelocity[_INDEX(i, j)].x,
						_gridPosition[_INDEX(i, j)].y - dt * oldVelocity[_INDEX(i, j)].y
					);
				if (backPos.x > xMax) backPos.x = xMax;
				else if (backPos.x < xMin) backPos.x = xMin;

				if (backPos.y > yMax) backPos.y = yMax;
				else if (backPos.y < yMin) backPos.y = yMin;


				_gridVelocity[_INDEX(i, j)] = _interp->gridToParticle(backPos, oldVelocity, _gridPosition);
			}
		}
	}
	_setBoundary(_gridVelocity);
	_setFreeSurface(_gridVelocity);
}

void Eulerian::_project()
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
			//if (_gridState[_INDEX(i, j)] == STATE::FLUID) _gridPressure[_INDEX(i, j)] = 0.01f;
			//else  _gridPressure[_INDEX(i, j)] = 0.0f;
			//printf("%f  ", _gridDivergence[_INDEX(i, j)]);
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

void Eulerian::_updateParticlePos()
{
	int N = _gridCount - 2;
	float dt = _timeStep;

	// 0.5f is the correct value.
	// But we assign a value of 1.1f to minmax for boundary conditions.
	// By doing this, "the velocity of the boundary" is not affected by the interpolation of the particle velocity.
	float yMax = _gridPosition[_INDEX(0, N + 1)].y - 1.1f;
	float yMin = _gridPosition[_INDEX(0, 0)].y + 1.1f;
	float xMax = _gridPosition[_INDEX(N + 1, 0)].x - 1.1f;
	float xMin = _gridPosition[_INDEX(0, 0)].x + 1.1f;

	for (int i = 0; i < _particlePosition.size(); i++)
	{
		// 2. 3.
		_particleVelocity[i] = _interp->gridToParticle(_particlePosition[i], _gridVelocity, _gridPosition);
		_particlePosition[i] += _particleVelocity[i] * dt;

		if (_particlePosition[i].x > xMax) _particlePosition[i].x = xMax;
		else if (_particlePosition[i].x < xMin) _particlePosition[i].x = xMin;

		if (_particlePosition[i].y > yMax) _particlePosition[i].y = yMax;
		else if (_particlePosition[i].y < yMin) _particlePosition[i].y = yMin;

		cout << "particle velocity : " << _particleVelocity[i].x << ", " << _particleVelocity[i].y << endl;
	}
}