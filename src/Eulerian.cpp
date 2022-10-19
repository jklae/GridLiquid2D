#include "Eulerian.h"

using namespace DirectX;
using namespace std;
using namespace DXViewer::xmfloat2;
using namespace DXViewer::xmint2;

Eulerian::Eulerian(int x, int y, EX ex, float timeStep)
	:GridLiquid(x, y, timeStep)
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
			else
			{
				_gridVelocity[_INDEX(i, j)] = { 0.0f, 0.0f };
			}
			
		}
		
	}
	_setBoundary(_gridVelocity);
}

void Eulerian::_advect()
{
	float dt = _timeStep;
	XMINT2 N = _gridCount - 2;

	float yMax = _gridPosition[_INDEX(0, N.y + 1)].y - 0.5f;
	float yMin = _gridPosition[_INDEX(0, 0)].y + 0.5f;
	float xMax = _gridPosition[_INDEX(N.x + 1, 0)].x - 0.5f;
	float xMin = _gridPosition[_INDEX(0, 0)].x + 0.5f;

	vector<XMFLOAT2> oldVelocity = _gridVelocity;

	for (int j = 1; j <= N.y; j++)
	{
		for (int i = 1; i <= N.x; i++)
		{
			if (_gridState[_INDEX(i, j)] == STATE::LIQUID)
			{
				XMFLOAT2 backPos =
					XMFLOAT2(
						_gridPosition[_INDEX(i, j)].x - dt * oldVelocity[_INDEX(i, j)].x,
						_gridPosition[_INDEX(i, j)].y - dt * oldVelocity[_INDEX(i, j)].y
					);

				// Boundary condition
				if (backPos.x > xMax) backPos.x = xMax;
				else if (backPos.x < xMin) backPos.x = xMin;

				if (backPos.y > yMax) backPos.y = yMax;
				else if (backPos.y < yMin) backPos.y = yMin;

				// Semi Largrangian
				_gridVelocity[_INDEX(i, j)] = gridToParticle(backPos, oldVelocity);
			}
			
		}
	}
	_setBoundary(_gridVelocity);
	_setFreeSurface(_gridVelocity);
}

void Eulerian::_project()
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

	//_setBoundary(_gridDivergence);
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
	_setBoundary(_gridVelocity);

}

void Eulerian::_updateParticlePos()
{
	XMINT2 N = _gridCount - 2;
	float dt = _timeStep;

	float yMax = _gridPosition[_INDEX(0, N.y + 1)].y - 0.5f;
	float yMin = _gridPosition[_INDEX(0, 0)].y + 0.5f;
	float xMax = _gridPosition[_INDEX(N.x + 1, 0)].x - 0.5f;
	float xMin = _gridPosition[_INDEX(0, 0)].x + 0.5f;

	for (int i = 0; i < _particlePosition.size(); i++)
	{
		_particleVelocity[i] = gridToParticle(_particlePosition[i], _gridVelocity);
		_particlePosition[i] += _particleVelocity[i] * dt;

		// Boundary condition
		if (_particlePosition[i].x > xMax) _particlePosition[i].x = xMax;
		else if (_particlePosition[i].x < xMin) _particlePosition[i].x = xMin;

		if (_particlePosition[i].y > yMax) _particlePosition[i].y = yMax;
		else if (_particlePosition[i].y < yMin) _particlePosition[i].y = yMin;
	}
}