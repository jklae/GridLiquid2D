#include "PICFLIP.h"

using namespace DirectX;
using namespace std;

PICFLIP::PICFLIP(GridData& index, EX ex, float timeStep)
	:GridLiquid(index, timeStep)
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

	size_t vSize = static_cast<size_t>(_gridCount) * static_cast<size_t>(_gridCount);
	_oldVel.assign(vSize, { 0.0f, 0.0f });
}



void PICFLIP::_update()
{
	_advect();

	_force();
	_setBoundary(_gridVelocity);
	_setFreeSurface(_gridVelocity);

	_project();
	// Solve boundary condition again due to numerical errors in previous step
	_setBoundary(_gridVelocity);
	_updateParticlePos();

	_paintLiquid();

	
}

void PICFLIP::_advect()
{
	int N = _gridCount - 2;
	for (int i = 0; i < _particlePosition.size(); i++)
	{
		_interp->particleToGrid(_particlePosition[i], _particleVelocity[i], _gridPosition, _gridState);
	}

	for (int i = 0; i < _gridCount; i++)
	{
		for (int j = 0; j < _gridCount; j++)
		{
			_interp->setGridVelocity(_gridVelocity, _oldVel, i, j);
		}
	}
}

void PICFLIP::_force()
{
	float dt = _timeStep;

	int N = _gridCount - 2;
	for (int i = 1; i <= N; i++)
	{
		for (int j = 1; j <= N; j++)
		{
			if (_gridState[_INDEX(i, j)] == STATE::LIQUID)
			{
				_gridVelocity[_INDEX(i, j)].y -= 30.8f * dt;
			}

		}
	}
}


void PICFLIP::_project()
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
			if (_gridState[_INDEX(i, j)] == STATE::LIQUID)
			{
				_gridVelocity[_INDEX(i, j)].x -= (_gridPressure[_INDEX(i + 1, j)] - _gridPressure[_INDEX(i - 1, j)]) * 0.5f;
				_gridVelocity[_INDEX(i, j)].y -= (_gridPressure[_INDEX(i, j + 1)] - _gridPressure[_INDEX(i, j - 1)]) * 0.5f;
			}
		}
	}

}

void PICFLIP::_updateParticlePos()
{
	float dt = _timeStep;

	int N = _gridCount - 2;
	for (int i = 0; i < _oldVel.size(); i++)
	{
		_oldVel[i] = _gridVelocity[i] - _oldVel[i];
	}

	// 0.5f is the correct value.
	// But we assign a value of 1.1f to minmax for boundary conditions.
	// By doing this, "the velocity of the boundary" is not affected by the interpolation of the particle velocity.
	float yMax = _gridPosition[_INDEX(0, N + 1)].y - 0.5f;
	float yMin = _gridPosition[_INDEX(0, 0)].y + 0.5f;
	float xMax = _gridPosition[_INDEX(N + 1, 0)].x - 0.5f;
	float xMin = _gridPosition[_INDEX(0, 0)].x + 0.5f;

	for (int i = 0; i < _particlePosition.size(); i++)
	{
		XMFLOAT2 _picVel = _interp->gridToParticle(_particlePosition[i], _gridVelocity, _gridPosition, _gridState);
		XMFLOAT2 _flipVel = _particleVelocity[i] + _interp->gridToParticle(_particlePosition[i], _oldVel, _gridPosition, _gridState);

		_particleVelocity[i] = _picVel * (1 - _flipRatio) + _flipVel * _flipRatio;
		_particlePosition[i] += _particleVelocity[i] * dt;

		if (_particlePosition[i].x > xMax) _particlePosition[i].x = xMax;
		else if (_particlePosition[i].x < xMin) _particlePosition[i].x = xMin;

		if (_particlePosition[i].y > yMax) _particlePosition[i].y = yMax;
		else if (_particlePosition[i].y < yMin) _particlePosition[i].y = yMin;

		//cout << i << ":  particle velocity : " << _particleVelocity[i].x << ", " << _particleVelocity[i].y << endl;
	}
}
