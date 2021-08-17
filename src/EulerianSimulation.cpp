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
				_gridVelocity.push_back(XMFLOAT2(0.0f, 0.0f * _gridScale));
			}

		}
	}

	_gridState[_INDEX(10, 10)] = _STATE::FLUID;
	_gridState[_INDEX(10, 11)] = _STATE::FLUID;
	_gridState[_INDEX(11, 10)] = _STATE::FLUID;
	_gridState[_INDEX(11, 11)] = _STATE::FLUID;

	// Compute stride and offset
	_stride = (_gridSize * _gridScale);
	_offset = XMFLOAT2(
		//		radius    *     count
		(_stride / 2.0f) * static_cast<float>(_gridCount.x - 1),
		(_stride / 2.0f) * static_cast<float>(_gridCount.y - 1));
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

	//_force(timestep);
	_advect(timestep);
	//_diffuse(timestep);
	_project(timestep);

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
			_gridVelocity[_INDEX(i, j)].x += 9.8f * 0.000005f * tstep * _gridScale;
			_gridVelocity[_INDEX(i, j)].y += 1.8f * 0.000005f * tstep * _gridScale;
		}
	}
	_setBoundary();
}

void EulerianSimulation::_advect(double timestep)
{
	float tstep = static_cast<float>(timestep);
	static bool a = true;
	if (a)
	{
		_particlePosition[0].x -= tstep ;
		a = false;
	}

	for (int j = 1; j < _gridCount.y - 1; j++)
	{
		for (int i = 1; i < _gridCount.x - 1; i++)
		{

		}
	}
	_setBoundary();
}

void EulerianSimulation::_diffuse(double timestep)
{

}

void EulerianSimulation::_project(double timestep)
{

}

void EulerianSimulation::_setBoundary()
{
	int xN = _gridCount.x - 2;
	int yN = _gridCount.y - 2;

	// (x, 0) (x, yMax+1)
	for (int i = 1; i <= xN; i++)
	{
		_gridVelocity[_INDEX(i, 0)].x = +_gridVelocity[_INDEX(i, 1)].x;
		_gridVelocity[_INDEX(i, 0)].y = -_gridVelocity[_INDEX(i, 1)].y;

		_gridVelocity[_INDEX(i, yN + 1)].x = +_gridVelocity[_INDEX(i, yN)].x;
		_gridVelocity[_INDEX(i, yN + 1)].y = -_gridVelocity[_INDEX(i, yN)].y;
	}

	// (0, y) (xMax+1, y)
	for (int j = 1; j <= yN; j++)
	{
		_gridVelocity[_INDEX(0, j)].x = -_gridVelocity[_INDEX(1, j)].x;
		_gridVelocity[_INDEX(0, j)].y = +_gridVelocity[_INDEX(1, j)].y;

		_gridVelocity[_INDEX(xN + 1, j)].x = -_gridVelocity[_INDEX(j, xN)].x;
		_gridVelocity[_INDEX(xN + 1, j)].y = +_gridVelocity[_INDEX(j, xN)].y;
	}

	// (0, 0)
	_gridVelocity[_INDEX(0, 0)].x = _gridVelocity[_INDEX(0, 1)].x;
	_gridVelocity[_INDEX(0, 0)].y = _gridVelocity[_INDEX(1, 0)].y;
	// (0, yCount)
	_gridVelocity[_INDEX(0, yN + 1)].x = _gridVelocity[_INDEX(0, yN)].x;
	_gridVelocity[_INDEX(0, yN + 1)].y = _gridVelocity[_INDEX(1, yN + 1)].y ;
	// (xCount, 0)
	_gridVelocity[_INDEX(xN + 1, 0)].x = _gridVelocity[_INDEX(xN + 1, 1)].x;
	_gridVelocity[_INDEX(xN + 1, 0)].y = _gridVelocity[_INDEX(xN, 0)].y;
	// (xCount, yCount)
	_gridVelocity[_INDEX(xN + 1, yN + 1)].x = _gridVelocity[_INDEX(xN + 1, yN)].x;
	_gridVelocity[_INDEX(xN + 1, yN + 1)].y = _gridVelocity[_INDEX(xN, yN + 1)].y;


	//cout << _velocity[_INDEX(1, 0)].x << ", " << _velocity[_INDEX(0, 1)].x << endl;
	//cout << _velocity[_INDEX(1, 0)].x + _velocity[_INDEX(0, 1)].x << endl;
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

	// To calculate the grid index, the calculation result must not depend on the _gridScale.
	// Therefore, the intermediate computed variable "should not be multiplied by the _gridScale".
	// For example, if the scale is 1.0f, the result is (index * 1.0f).
	// But if the scale is 0.5f, the result is (index * 0.5f).
	// The index value should of course be immutable.
	float particleStride = (_gridSize / 2.0f) * _particleScale;
	XMFLOAT2 particleOffset = { (_gridSize / 2.0f) * static_cast<float>(_gridCount.x),
								(_gridSize / 2.0f) * static_cast<float>(_gridCount.y) };

	for (int i = 0; i < _particlePosition.size(); i++)
	{
		int minXIndex = _computeFaceMinMaxIndex(_VALUE::MIN, _particlePosition[i].x, particleOffset.x, particleStride);
		int minYIndex = _computeFaceMinMaxIndex(_VALUE::MIN, _particlePosition[i].y, particleOffset.y, particleStride);
		int maxXIndex = _computeFaceMinMaxIndex(_VALUE::MAX, _particlePosition[i].x, particleOffset.x, particleStride);
		int maxYIndex = _computeFaceMinMaxIndex(_VALUE::MAX, _particlePosition[i].y, particleOffset.y, particleStride);

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
	// Different from _paintGrid().
	// 1. Subtract the count of offset by 1.
	// 2. Do not subtract particleStride from min, max calculation.
	// 3. ceil maxIndex instead of floor.
	// ------------------------------------------------------------------
	// _PaintGrid() uses the face as the transition point.
	// _updateParticlePosition() uses the center as the transition point.
																					// 1.
	XMFLOAT2 particleOffset = { (_gridSize / 2.0f) * static_cast<float>(_gridCount.x - 1),
								(_gridSize / 2.0f) * static_cast<float>(_gridCount.y - 1) };

	for (int i = 0; i < _particlePosition.size(); i++)
	{
		// 2. 3.
		_particleVelocity[i] = _velocityInterpolation(_particlePosition[i], particleOffset);

		_particlePosition[i].x += _particleVelocity[i].x;
		_particlePosition[i].y += _particleVelocity[i].y;
	}
}

int EulerianSimulation::_computeFaceMinMaxIndex(_VALUE vState, float pos, float offset, float stride)
{
	float value;

	// Compute position by normalizing from (-N, N) to (0, N + 1)
	switch (vState)
	{
	case _VALUE::MIN:
		value = offset + (pos / _gridScale) - stride;
		break;
	case _VALUE::MAX:
		value = offset + (pos / _gridScale) + stride;
		break;
	default:
		value = 0.0f;
		break;
	}

	// Compute Index
	return static_cast<int>(floor(value));
}

int EulerianSimulation::_computeCenterMinMaxIndex(_VALUE vState, float pos, float offset)
{
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

XMFLOAT2 EulerianSimulation::_velocityInterpolation(XMFLOAT2 pos, XMFLOAT2 offset)
{
	// 2. 3.
	int minXIndex = _computeCenterMinMaxIndex(_VALUE::MIN, pos.x, offset.x);
	int minYIndex = _computeCenterMinMaxIndex(_VALUE::MIN, pos.y, offset.y);
	int maxXIndex = _computeCenterMinMaxIndex(_VALUE::MAX, pos.x, offset.x);
	int maxYIndex = _computeCenterMinMaxIndex(_VALUE::MAX, pos.y, offset.y);

	float xRatio = (pos.x - _gridPosition[_INDEX(minXIndex, minYIndex)].x) / _stride;
	float yRatio = (pos.y - _gridPosition[_INDEX(minXIndex, minYIndex)].y) / _stride;

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
				-_offset.x + (float)i * _stride,
				-_offset.y + (float)j * _stride);

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
				-_offset.x + (float)i * _stride,
				-_offset.y + (float)j * _stride);

			if (_gridState[_INDEX(i, j)] == _STATE::FLUID)
			{
				_particleVelocity.push_back(XMFLOAT2(0.0f, 0.0f));
				_particlePosition.push_back(pos);

				struct ConstantBuffer particleCB;
				particleCB.world = transformMatrix(pos.x, pos.y, -_stride, _gridScale * _particleScale);
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