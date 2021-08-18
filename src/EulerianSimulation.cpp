#include "EulerianSimulation.h"


using namespace DirectX;
using namespace std;

EulerianSimulation::EulerianSimulation()
{
}

EulerianSimulation::~EulerianSimulation()
{
}

void EulerianSimulation::initialize()
{
	// 0 is not allowed.
	assert((_gridCount.x != 0) && (_gridCount.y != 0));
	assert(_gridScale != 0.0f);

	// Set _fluid
	for (int j = 0; j < _gridCount.y; j++)
	{
		for (int i = 0; i < _gridCount.x; i++)
		{
			if (i == 0 || j == 0
				|| i == _gridCount.x - 1
				|| j == _gridCount.y - 1)
			{
				_gridState.push_back(_STATE::BOUNDARY);
				_gridVelocity.push_back(XMFLOAT2(0.0f, 0.0f));
			}
			else
			{
				_gridState.push_back(_STATE::AIR);
				_gridVelocity.push_back(XMFLOAT2(0.0f, -0.0f * _gridScale));
			}

			_gridPressure.push_back(0.0f);
			_gridDivergence.push_back(0.0f);
		}
	}

	_gridState[_INDEX(10, 10)] = _STATE::FLUID;
	_gridState[_INDEX(10, 11)] = _STATE::FLUID;
	_gridState[_INDEX(11, 10)] = _STATE::FLUID;
	_gridState[_INDEX(11, 11)] = _STATE::FLUID;

	// Compute grid stride and offset
	_gridStride = (_gridSize * _gridScale);
	_gridOffset = 
		XMFLOAT2(
			//		radius    *     count
			(_gridStride / 2.0f) * static_cast<float>(_gridCount.x - 1),
			(_gridStride / 2.0f) * static_cast<float>(_gridCount.y - 1)
		);

	// Compute particle stride and offset
	_particleStride = (_gridSize / 2.0f) * _particleScale;
	_particleFaceOffset =
		XMFLOAT2(
			(_gridSize / 2.0f) * static_cast<float>(_gridCount.x),
			(_gridSize / 2.0f) * static_cast<float>(_gridCount.y)
		);
	_particleCenterOffset =
		XMFLOAT2(
																// 1.
			(_gridSize / 2.0f) * static_cast<float>(_gridCount.x - 1),
			(_gridSize / 2.0f) * static_cast<float>(_gridCount.y - 1)
		);
}

void EulerianSimulation::setGridCountXY(int xCount, int yCount)
{
	// 2 are boundaries.
	_gridCount.x = xCount + 2;
	_gridCount.y = yCount + 2;
}

void EulerianSimulation::setGridScale(float gridScale)
{
	_gridScale = gridScale;
}

void EulerianSimulation::_update(double timestep)
{
	//_particle[0].x += 0.00005f;
	//_particle[0].y += 0.00002f;

	//_particle[1].x -= 0.00003f;
	//_particle[1].y -= 0.00004f;

	_force(timestep);
	_advect(timestep);
	//_diffuse(timestep);
	//_project(timestep);
	
	_updateParticlePosition();
	_paintGrid();
}

void EulerianSimulation::_force(double timestep)
{
	float tstep = static_cast<float>(timestep);
	for (int j = 1; j < _gridCount.y - 1; j++)
	{
		for (int i = 1; i < _gridCount.x - 1; i++)
		{
			//_gridVelocity[_INDEX(i, j)].x -= 2.8f * 0.000005f * tstep * _gridScale;
			_gridVelocity[_INDEX(i, j)].y -= 9.8f * 0.000005f * tstep * _gridScale;
		}
	}
	_setBoundary(_gridVelocity);
}

void EulerianSimulation::_advect(double timestep)
{
	float tstep = static_cast<float>(timestep);

	float yMax = _gridPosition[_INDEX(0, _gridCount.y - 1)].y - (_gridSize * _gridScale) / 2.0f;
	float yMin = _gridPosition[_INDEX(0, 0)].y + (_gridSize * _gridScale) / 2.0f;
	float xMax = _gridPosition[_INDEX(_gridCount.x - 1, 0)].x - (_gridSize * _gridScale) / 2.0f;
	float xMin = _gridPosition[_INDEX(0, 0)].x + (_gridSize * _gridScale) / 2.0f;

	for (int j = 1; j < _gridCount.y - 1; j++)
	{
		for (int i = 1; i < _gridCount.x - 1; i++)
		{
			XMFLOAT2 backPos =
				XMFLOAT2(
					_gridPosition[_INDEX(i, j)].x - tstep * 30.0f * _gridVelocity[_INDEX(i, j)].x,
					_gridPosition[_INDEX(i, j)].y - tstep * 30.0f * _gridVelocity[_INDEX(i, j)].y
				);

			if (backPos.x > xMax) backPos.x = xMax;
			else if (backPos.x < xMin) backPos.x = xMin;

			if (backPos.y > yMax) backPos.y = yMax;
			else if (backPos.y < yMin) backPos.y = yMin;

			_gridVelocity[_INDEX(i, j)] = _velocityInterpolation(backPos);
		}
	}
	_setBoundary(_gridVelocity);
}

void EulerianSimulation::_diffuse(double timestep)
{

}

void EulerianSimulation::_project(double timestep)
{
	for (int j = 1; j < _gridCount.y - 1; j++)
	{
		for (int i = 1; i < _gridCount.x - 1; i++)
		{
			_gridDivergence[_INDEX(i, j)] =
				(_gridVelocity[_INDEX(i + 1, j)].x - _gridVelocity[_INDEX(i - 1, j)].x) / _gridSize
				+
				(_gridVelocity[_INDEX(i + 1, j)].y - _gridVelocity[_INDEX(i - 1, j)].y) / _gridSize;
			_gridPressure[_INDEX(i, j)] = 0.0f;
		}
	}
	_setBoundary(_gridDivergence);
	_setBoundary(_gridPressure);

	for (int iter = 0; iter < 20; iter++)
	{
		for (int j = 1; j < _gridCount.y - 1; j++)
		{
			for (int i = 1; i < _gridCount.x - 1; i++)
			{
				_gridPressure[_INDEX(i, j)] =
					(
						_gridDivergence[_INDEX(i, j)] -
						_gridPressure[_INDEX(i + 1, j)] + _gridPressure[_INDEX(i - 1, j)] +
						_gridPressure[_INDEX(i, j + 1)] + _gridPressure[_INDEX(i, j - 1)]
					) / -4.0f;
					
			}
		}
	}

	for (int j = 1; j < _gridCount.y - 1; j++)
	{
		for (int i = 1; i < _gridCount.x - 1; i++)
		{
			_gridVelocity[_INDEX(i, j)].x -= (_gridPressure[_INDEX(i + 1, j)] - _gridPressure[_INDEX(i - 1, j)]) / _gridSize;
			_gridVelocity[_INDEX(i, j)].y -= (_gridPressure[_INDEX(i + 1, j)] - _gridPressure[_INDEX(i - 1, j)]) / _gridSize;
		}
	}
	_setBoundary(_gridVelocity);
}

void EulerianSimulation::_setBoundary(std::vector<XMFLOAT2>& vec)
{
	int xN = _gridCount.x - 2;
	int yN = _gridCount.y - 2;

	// (x, 0) (x, yMax+1)
	for (int i = 1; i <= xN; i++)
	{
		vec[_INDEX(i, 0)].x = +vec[_INDEX(i, 1)].x;
		vec[_INDEX(i, 0)].y = -vec[_INDEX(i, 1)].y;

		vec[_INDEX(i, yN + 1)].x = +vec[_INDEX(i, yN)].x;
		vec[_INDEX(i, yN + 1)].y = -vec[_INDEX(i, yN)].y;
	}

	// (0, y) (xMax+1, y)
	for (int j = 1; j <= yN; j++)
	{
		vec[_INDEX(0, j)].x = -vec[_INDEX(1, j)].x;
		vec[_INDEX(0, j)].y = +vec[_INDEX(1, j)].y;

		vec[_INDEX(xN + 1, j)].x = -vec[_INDEX(j, xN)].x;
		vec[_INDEX(xN + 1, j)].y = +vec[_INDEX(j, xN)].y;
	}

	// (0, 0)
	vec[_INDEX(0, 0)].x = vec[_INDEX(0, 1)].x;
	vec[_INDEX(0, 0)].y = vec[_INDEX(1, 0)].y;
	// (0, yCount)
	vec[_INDEX(0, yN + 1)].x = vec[_INDEX(0, yN)].x;
	vec[_INDEX(0, yN + 1)].y = vec[_INDEX(1, yN + 1)].y ;
	// (xCount, 0)
	vec[_INDEX(xN + 1, 0)].x = vec[_INDEX(xN + 1, 1)].x;
	vec[_INDEX(xN + 1, 0)].y = vec[_INDEX(xN, 0)].y;
	// (xCount, yCount)
	vec[_INDEX(xN + 1, yN + 1)].x = vec[_INDEX(xN + 1, yN)].x;
	vec[_INDEX(xN + 1, yN + 1)].y = vec[_INDEX(xN, yN + 1)].y;
}

void EulerianSimulation::_setBoundary(std::vector<float>& scalar)
{
	int xN = _gridCount.x - 2;
	int yN = _gridCount.y - 2;

	// (x, 0) (x, yMax+1)
	for (int i = 1; i <= xN; i++)
	{
		scalar[_INDEX(i, 0)] = scalar[_INDEX(i, 1)];
		scalar[_INDEX(i, yN + 1)] = scalar[_INDEX(i, yN)];
	}

	// (0, y) (xMax+1, y)
	for (int j = 1; j <= yN; j++)
	{
		scalar[_INDEX(0, j)] = scalar[_INDEX(1, j)];
		scalar[_INDEX(xN + 1, j)] = scalar[_INDEX(j, xN)];
	}

	// (0, 0)
	scalar[_INDEX(0, 0)] = (scalar[_INDEX(0, 1)] + scalar[_INDEX(1, 0)]) / 2.0f;
	// (0, yCount)
	scalar[_INDEX(0, yN + 1)] = (scalar[_INDEX(0, yN)] + scalar[_INDEX(1, yN + 1)]) / 2.0f;
	// (xCount, 0)
	scalar[_INDEX(xN + 1, 0)] = (scalar[_INDEX(xN + 1, 1)] + scalar[_INDEX(xN, 0)]) / 2.0f;
	// (xCount, yCount)
	scalar[_INDEX(xN + 1, yN + 1)] = (scalar[_INDEX(xN + 1, yN)] + scalar[_INDEX(xN, yN + 1)]) / 2.0f;
}

void EulerianSimulation::_paintGrid()
{
	// Reset _grid
	for (int j = 1; j < _gridCount.y - 1; j++)
	{
		for (int i = 1; i < _gridCount.x - 1; i++)
		{
			_gridState[_INDEX(i, j)] = _STATE::AIR;
		}
	}


	for (int i = 0; i < _particlePosition.size(); i++)
	{
		int minXIndex = _computeFaceMinMaxIndex(_VALUE::MIN, _AXIS::X, _particlePosition[i]);
		int minYIndex = _computeFaceMinMaxIndex(_VALUE::MIN, _AXIS::Y, _particlePosition[i]);
		int maxXIndex = _computeFaceMinMaxIndex(_VALUE::MAX, _AXIS::X, _particlePosition[i]);
		int maxYIndex = _computeFaceMinMaxIndex(_VALUE::MAX, _AXIS::Y, _particlePosition[i]);

		// Painting
		_STATE& minMin = _gridState[_INDEX(minXIndex, minYIndex)];
		_STATE& minMax = _gridState[_INDEX(minXIndex, maxYIndex)];
		_STATE& maxMin = _gridState[_INDEX(maxXIndex, minYIndex)];
		_STATE& maxMax = _gridState[_INDEX(maxXIndex, maxYIndex)];

		// Boundary Checking
		if (minMin != _STATE::BOUNDARY) minMin = _STATE::FLUID;
		if (minMax != _STATE::BOUNDARY) minMax = _STATE::FLUID;
		if (maxMin != _STATE::BOUNDARY) maxMin = _STATE::FLUID;
		if (maxMax != _STATE::BOUNDARY) maxMax = _STATE::FLUID;
	}
}


void EulerianSimulation::_updateParticlePosition()
{
	for (int i = 0; i < _particlePosition.size(); i++)
	{
		// 2. 3.
		_particleVelocity[i] = _velocityInterpolation(_particlePosition[i]);

		_particlePosition[i].x += _particleVelocity[i].x;
		_particlePosition[i].y += _particleVelocity[i].y;
	}
}


// To calculate the grid index, the calculation result must not depend on the _gridScale.
// Therefore, the intermediate computed variable "should not be multiplied by the _gridScale".
// For example, if the scale is 1.0f, the result is (index * 1.0f).
// But if the scale is 0.5f, the result is (index * 0.5f).
// The index value should of course be immutable.
int EulerianSimulation::_computeFaceMinMaxIndex(_VALUE vState, _AXIS axis, XMFLOAT2 particlePos)
{
	float offset = (axis == _AXIS::X) ? _particleFaceOffset.x : _particleFaceOffset.y;
	float pos = (axis == _AXIS::X) ? particlePos.x : particlePos.y;
	float value;

	// Compute position by normalizing from (-N, N) to (0, N + 1)
	switch (vState)
	{
	case _VALUE::MIN:
		value = offset + (pos / _gridScale) - _particleStride;
		break;
	case _VALUE::MAX:
		value = offset + (pos / _gridScale) + _particleStride;
		break;
	default:
		value = 0.0f;
		break;
	}

	// Compute Index
	return static_cast<int>(floor(value));
}


// Different from _computeFaceMinMaxIndex().
// 1. Subtract the count of offset by 1.
// 2. Do not subtract particleStride from min, max calculation.
// 3. ceil maxIndex instead of floor.
// ------------------------------------------------------------------
// _PaintGrid() uses the face as the transition point.
// _updateParticlePosition() uses the center as the transition point.
int EulerianSimulation::_computeCenterMinMaxIndex(_VALUE vState, _AXIS axis, XMFLOAT2 particlePos)
{
	float offset = (axis == _AXIS::X) ? _particleFaceOffset.x : _particleFaceOffset.y;
	float pos = (axis == _AXIS::X) ? particlePos.x : particlePos.y;
	// 2.
	float value = offset + (pos / _gridScale);

	switch (vState)
	{
	case _VALUE::MIN:
		return static_cast<int>(floor(value));
		break;
	case _VALUE::MAX:
		// 3.
		return static_cast<int>(ceil(value));
		break;
	default:
		return -1;
		break;
	}
}

XMFLOAT2 EulerianSimulation::_velocityInterpolation(XMFLOAT2 pos)
{
	// 2. 3.
	int minXIndex = _computeCenterMinMaxIndex(_VALUE::MIN, _AXIS::X, pos);
	int minYIndex = _computeCenterMinMaxIndex(_VALUE::MIN, _AXIS::Y, pos);
	int maxXIndex = _computeCenterMinMaxIndex(_VALUE::MAX, _AXIS::X, pos);
	int maxYIndex = _computeCenterMinMaxIndex(_VALUE::MAX, _AXIS::Y, pos);

	float xRatio = (pos.x - _gridPosition[_INDEX(minXIndex, minYIndex)].x) / _gridStride;
	float yRatio = (pos.y - _gridPosition[_INDEX(minXIndex, minYIndex)].y) / _gridStride;

	XMFLOAT2 minVelocity = _gridVelocity[_INDEX(minXIndex, minYIndex)];
	XMFLOAT2 maxVelocity = _gridVelocity[_INDEX(maxXIndex, maxYIndex)];

	return XMFLOAT2(
		_interpolation(minVelocity.x, maxVelocity.x, xRatio),
		_interpolation(minVelocity.y, maxVelocity.y, yRatio)
	);
}

float EulerianSimulation::_interpolation(float value1, float value2, float ratio)
{
	return value1 * (1.0f - ratio) + value2 * ratio;
}

#pragma region Implementation
// ################################## Implementation ####################################
void EulerianSimulation::iUpdate(double timestep)
{
	_update(timestep);
}

vector<Vertex> EulerianSimulation::iGetVertice()
{
	return _vertices;
}

vector<unsigned int> EulerianSimulation::iGetIndice()
{
	return _indices;
}

XMFLOAT4 EulerianSimulation::iGetColor(int i)
{
	switch (_gridState[i])
	{
	case _STATE::FLUID:
		return XMFLOAT4(0.2f, 0.5f, 0.5f, 1.0f);
		break;

	case _STATE::BOUNDARY:
		return XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
		break;

	case _STATE::AIR:
		return XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
		break;

	default:
		return XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		break;
	}
}

XMINT2 EulerianSimulation::iGetObjectCountXY()
{
	return _gridCount;
}

XMFLOAT2 EulerianSimulation::iGetParticlePos(int i)
{
	return _particlePosition[i];
}

void EulerianSimulation::iCreateObjectParticle(vector<ConstantBuffer>& constantBuffer)
{

	// ###### Create Object ######
	for (int j = 0; j < _gridCount.y; j++)
	{
		for (int i = 0; i < _gridCount.x; i++)
		{
			// Position
			XMFLOAT2 pos = XMFLOAT2(
				-_gridOffset.x + (float)i * _gridStride,
				-_gridOffset.y + (float)j * _gridStride);

			_gridPosition.push_back(pos);

			struct ConstantBuffer objectCB;
			// Multiply by a specific value to make a stripe
			objectCB.world = transformMatrix(pos.x, pos.y, 0.0f, _gridScale * 0.95f);
			objectCB.worldViewProj = transformMatrix(0.0f, 0.0f, 0.0f);
			objectCB.color = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

			constantBuffer.push_back(objectCB);
		}
	}
	// ###### ###### ###### ######

	// ###### Create particle ######
	for (int j = 0; j < _gridCount.y; j++)
	{
		for (int i = 0; i < _gridCount.x; i++)
		{
			// Position
			XMFLOAT2 pos = XMFLOAT2(
				-_gridOffset.x + (float)i * _gridStride,
				-_gridOffset.y + (float)j * _gridStride);

			if (_gridState[_INDEX(i, j)] == _STATE::FLUID)
			{
				_particleVelocity.push_back(XMFLOAT2(0.0f, 0.0f));
				_particlePosition.push_back(pos);

				struct ConstantBuffer particleCB;
				particleCB.world = transformMatrix(pos.x, pos.y, -_gridStride, _gridScale * _particleScale);
				particleCB.worldViewProj = transformMatrix(0.0f, 0.0f, 0.0f);
				particleCB.color = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);

				constantBuffer.push_back(particleCB);
			}
		}
	}
	// ###### ###### ###### ######
}

// #######################################################################################
#pragma endregion