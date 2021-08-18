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
	assert(_gridCount != 0);

	// Set _fluid
	int N = _gridCount - 2;
	for (int j = 0; j < _gridCount; j++)
	{
		for (int i = 0; i < _gridCount; i++)
		{
			if (i == 0 || j == 0
				|| i == N + 1
				|| j == N + 1)
			{
				_gridState.push_back(STATE::BOUNDARY);
				_gridVelocity.push_back(XMFLOAT2(0.0f, 0.0f));
			}
			else
			{
				_gridState.push_back(STATE::AIR);
				_gridVelocity.push_back(XMFLOAT2(0.0f, -0.0f));
			}

			_gridPressure.push_back(0.0f);
			_gridDivergence.push_back(0.0f);
		}
	}

	_gridState[_INDEX(6, 7)] = STATE::FLUID;
	/*_gridState[_INDEX(10, 11)] = STATE::FLUID;
	_gridState[_INDEX(11, 10)] = STATE::FLUID;
	_gridState[_INDEX(11, 11)] = STATE::FLUID;*/


}

void EulerianSimulation::setGridDomain(int xCount, int yCount)
{
	// 2 are boundaries.
	_gridCount = xCount + 2;
}


void EulerianSimulation::_update(double timestep)
{
	//_particle[0].x += 0.00005f;
	//_particle[0].y += 0.00002f;

	//_particle[1].x -= 0.00003f;
	//_particle[1].y -= 0.00004f;

	_force(timestep);
	//_advect(timestep);
	//_diffuse(timestep);
	//_project(timestep);
	
	_updateParticlePosition();
	_paintGrid();
}

void EulerianSimulation::_force(double timestep)
{
	int N = _gridCount - 2;
	float tstep = static_cast<float>(timestep);
	for (int j = 1; j <= N; j++)
	{
		for (int i = 1; i <= N; i++)
		{
			_gridVelocity[_INDEX(i, j)].x -= 2.8f * 0.00000005f * tstep;
			_gridVelocity[_INDEX(i, j)].y -= 9.8f * 0.00000005f * tstep;
		}
	}
	_setBoundary(_gridVelocity);
}

void EulerianSimulation::_advect(double timestep)
{
	float tstep = static_cast<float>(timestep);

	float yMax = _gridPosition[_INDEX(0, _gridCount - 1)].y - 0.5f;
	float yMin = _gridPosition[_INDEX(0, 0)].y + 0.5f;
	float xMax = _gridPosition[_INDEX(_gridCount - 1, 0)].x - 0.5f;
	float xMin = _gridPosition[_INDEX(0, 0)].x + 0.5f;

	int N = _gridCount - 2;
	for (int j = 1; j <= N; j++)
	{
		for (int i = 1; i <= N; i++)
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
	int N = _gridCount - 2;
	for (int j = 1; j <= N; j++)
	{
		for (int i = 1; i <= N; i++)
		{
			_gridDivergence[_INDEX(i, j)] =
				0.5f * (_gridVelocity[_INDEX(i + 1, j)].x - _gridVelocity[_INDEX(i - 1, j)].x)  +
				0.5f * (_gridVelocity[_INDEX(i, j + 1)].y - _gridVelocity[_INDEX(i, j - 1)].y) ;
			_gridPressure[_INDEX(i, j)] = 0.0f;
		}
	}

	/*_gridPressure[_INDEX(10, 10)] = 2.0f;
	_gridPressure[_INDEX(10, 11)] = 2.0f;
	_gridPressure[_INDEX(11, 10)] = 2.0f;
	_gridPressure[_INDEX(11, 11)] = 2.0f;*/

	_setBoundary(_gridDivergence);
	_setBoundary(_gridPressure);

	for (int iter = 0; iter < 20; iter++)
	{
		for (int j = 1; j <= N; j++)
		{
			for (int i = 1; i <= N; i++)
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

	for (int j = 1; j <= N; j++)
	{
		for (int i = 1; i <= N; i++)
		{
			_gridVelocity[_INDEX(i, j)].x -= (_gridPressure[_INDEX(i + 1, j)] - _gridPressure[_INDEX(i - 1, j)]) * 0.5f;
			_gridVelocity[_INDEX(i, j)].y -= (_gridPressure[_INDEX(i, j + 1)] - _gridPressure[_INDEX(i, j - 1)]) * 0.5f;
		}
	}
	_setBoundary(_gridVelocity);
}

void EulerianSimulation::_setBoundary(std::vector<XMFLOAT2>& vec)
{
	int N = _gridCount - 2;

	// (x, 0) (x, yMax+1)
	for (int i = 1; i <= N; i++)
	{
		vec[_INDEX(i, 0)].x = +vec[_INDEX(i, 1)].x;
		vec[_INDEX(i, 0)].y = -vec[_INDEX(i, 1)].y;

		vec[_INDEX(i, N + 1)].x = +vec[_INDEX(i, N)].x;
		vec[_INDEX(i, N + 1)].y = -vec[_INDEX(i, N)].y;
	}

	// (0, y) (xMax+1, y)
	for (int j = 1; j <= N; j++)
	{
		vec[_INDEX(0, j)].x = -vec[_INDEX(1, j)].x;
		vec[_INDEX(0, j)].y = +vec[_INDEX(1, j)].y;

		vec[_INDEX(N + 1, j)].x = -vec[_INDEX(j, N)].x;
		vec[_INDEX(N + 1, j)].y = +vec[_INDEX(j, N)].y;
	}

	// (0, 0)
	vec[_INDEX(0, 0)].x = vec[_INDEX(0, 1)].x;
	vec[_INDEX(0, 0)].y = vec[_INDEX(1, 0)].y;
	// (0, yCount)
	vec[_INDEX(0, N + 1)].x = vec[_INDEX(0, N)].x;
	vec[_INDEX(0, N + 1)].y = vec[_INDEX(1, N + 1)].y ;
	// (xCount, 0)
	vec[_INDEX(N + 1, 0)].x = vec[_INDEX(N + 1, 1)].x;
	vec[_INDEX(N + 1, 0)].y = vec[_INDEX(N, 0)].y;
	// (xCount, yCount)
	vec[_INDEX(N + 1, N + 1)].x = vec[_INDEX(N + 1, N)].x;
	vec[_INDEX(N + 1, N + 1)].y = vec[_INDEX(N, N + 1)].y;
}

void EulerianSimulation::_setBoundary(std::vector<float>& scalar)
{
	int N = _gridCount - 2;

	// (x, 0) (x, yMax+1)
	for (int i = 1; i <= N; i++)
	{
		scalar[_INDEX(i, 0)] = scalar[_INDEX(i, 1)];
		scalar[_INDEX(i, N + 1)] = scalar[_INDEX(i, N)];
	}

	// (0, y) (xMax+1, y)
	for (int j = 1; j <= N; j++)
	{
		scalar[_INDEX(0, j)] = scalar[_INDEX(1, j)];
		scalar[_INDEX(N + 1, j)] = scalar[_INDEX(j, N)];
	}

	// (0, 0)
	scalar[_INDEX(0, 0)] = (scalar[_INDEX(0, 1)] + scalar[_INDEX(1, 0)]) / 2.0f;
	// (0, yCount)
	scalar[_INDEX(0, N + 1)] = (scalar[_INDEX(0, N)] + scalar[_INDEX(1, N + 1)]) / 2.0f;
	// (xCount, 0)
	scalar[_INDEX(N + 1, 0)] = (scalar[_INDEX(N + 1, 1)] + scalar[_INDEX(N, 0)]) / 2.0f;
	// (xCount, yCount)
	scalar[_INDEX(N + 1, N + 1)] = (scalar[_INDEX(N + 1, N)] + scalar[_INDEX(N, N + 1)]) / 2.0f;
}

void EulerianSimulation::_paintGrid()
{
	int N = _gridCount - 2;
	// Reset _grid
	for (int j = 1; j <= N; j++)
	{
		for (int i = 1; i <= N; i++)
		{
			_gridState[_INDEX(i, j)] = STATE::AIR;
		}
	}


	for (int i = 0; i < _particlePosition.size(); i++)
	{
		int minXIndex = _computeFaceMinMaxIndex(VALUE::MIN, AXIS::X, _particlePosition[i]);
		int minYIndex = _computeFaceMinMaxIndex(VALUE::MIN, AXIS::Y, _particlePosition[i]);
		int maxXIndex = _computeFaceMinMaxIndex(VALUE::MAX, AXIS::X, _particlePosition[i]);
		int maxYIndex = _computeFaceMinMaxIndex(VALUE::MAX, AXIS::Y, _particlePosition[i]);

		// Painting
		STATE& minMin = _gridState[_INDEX(minXIndex, minYIndex)];
		STATE& minMax = _gridState[_INDEX(minXIndex, maxYIndex)];
		STATE& maxMin = _gridState[_INDEX(maxXIndex, minYIndex)];
		STATE& maxMax = _gridState[_INDEX(maxXIndex, maxYIndex)];

		// Boundary Checking
		if (minMin != STATE::BOUNDARY) minMin = STATE::FLUID;
		if (minMax != STATE::BOUNDARY) minMax = STATE::FLUID;
		if (maxMin != STATE::BOUNDARY) maxMin = STATE::FLUID;
		if (maxMax != STATE::BOUNDARY) maxMax = STATE::FLUID;
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
int EulerianSimulation::_computeFaceMinMaxIndex(VALUE vState, AXIS axis, XMFLOAT2 particlePos)
{
	float pos = (axis == AXIS::X) ? particlePos.x : particlePos.y;
	float value;

	// Compute position by normalizing from (-N, N) to (0, N + 1)
	switch (vState)
	{
	case VALUE::MIN:
		value = pos + 0.5f - 0.5f * _particleScale;
		break;
	case VALUE::MAX:
		value = pos + 0.5f + 0.5f * _particleScale;
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
int EulerianSimulation::_computeCenterMinMaxIndex(VALUE vState, AXIS axis, XMFLOAT2 particlePos)
{
	float pos = (axis == AXIS::X) ? particlePos.x : particlePos.y;
	// 2.
	float value = pos;

	switch (vState)
	{
	case VALUE::MIN:
		return static_cast<int>(floor(value));
		break;
	case VALUE::MAX:
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
	int minXIndex = _computeCenterMinMaxIndex(VALUE::MIN, AXIS::X, pos);
	int minYIndex = _computeCenterMinMaxIndex(VALUE::MIN, AXIS::Y, pos);
	int maxXIndex = _computeCenterMinMaxIndex(VALUE::MAX, AXIS::X, pos);
	int maxYIndex = _computeCenterMinMaxIndex(VALUE::MAX, AXIS::Y, pos);

	float xRatio = (pos.x - _gridPosition[_INDEX(minXIndex, minYIndex)].x);
	float yRatio = (pos.y - _gridPosition[_INDEX(minXIndex, minYIndex)].y);

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
	case STATE::FLUID:
		return XMFLOAT4(0.2f, 0.5f, 0.5f, 1.0f);
		break;

	case STATE::BOUNDARY:
		return XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
		break;

	case STATE::AIR:
		return XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
		break;

	default:
		return XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		break;
	}
}

int EulerianSimulation::iGetObjectCount()
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
	for (int j = 0; j < _gridCount; j++)
	{
		for (int i = 0; i < _gridCount; i++)
		{
			// Position
			XMFLOAT2 pos = XMFLOAT2(
				(float)i ,
				(float)j );

			_gridPosition.push_back(pos);

			struct ConstantBuffer objectCB;
			// Multiply by a specific value to make a stripe
			objectCB.world = transformMatrix(pos.x, pos.y, 0.0f, 0.95f);
			objectCB.worldViewProj = transformMatrix(0.0f, 0.0f, 0.0f);
			objectCB.color = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

			constantBuffer.push_back(objectCB);
		}
	}
	// ###### ###### ###### ######

	// ###### Create particle ######
	for (int j = 0; j < _gridCount; j++)
	{
		for (int i = 0; i < _gridCount; i++)
		{
			// Position
			XMFLOAT2 pos = XMFLOAT2(
				(float)i ,
				(float)j );

			if (_gridState[_INDEX(i, j)] == STATE::FLUID)
			{
				_particleVelocity.push_back(XMFLOAT2(0.0f, 0.0f));
				_particlePosition.push_back(pos);

				struct ConstantBuffer particleCB;
				particleCB.world = transformMatrix(pos.x, pos.y, -1.0f, _particleScale);
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