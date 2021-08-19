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
	for (int i = 0; i < _gridCount; i++)
	{
		for (int j = 0; j < _gridCount; j++)
		{
			if (i == 0 || j == 0
				|| i == N + 1
				|| j == N + 1)
			{
				_gridState.push_back(STATE::BOUNDARY);
			}
			else if ( ((N + 1) / 2 - 4 < i) 
				&& (i < (N + 1) / 2 + 4)
				&& ((N + 1) / 2 - 4 < j) 
				&& (j < (N + 1) / 2 + 4)
				)
			{
				_gridState.push_back(STATE::FLUID);
			}
			else
			{
				_gridState.push_back(STATE::AIR);
			}

			_gridVelocity.push_back(XMFLOAT2(0.001f, 0.0f));
			_gridPressure.push_back(0.0f);
			_gridDivergence.push_back(0.0f);
		}
	}
	//_gridState[_INDEX(5, 5)] = STATE::FLUID;
	//_gridState[_INDEX(5, 6)] = STATE::FLUID;
	//_gridState[_INDEX(6, 5)] = STATE::FLUID;
	//_gridState[_INDEX(6, 6)] = STATE::FLUID;

}

void EulerianSimulation::setGridDomain(int xCount, int yCount)
{
	// 2 are boundaries.
	_gridCount = xCount + 2;
}

void EulerianSimulation::_printVelocity()
{
	static int step = 0;
	printf("============= step %d =============\n", step);
	for (int i = 0; i < _gridCount; i++)
	{
		for (int j = 0; j < _gridCount; j++)
		{
			printf("(%9f, %9f) ", _gridVelocity[_INDEX(i, j)].x, _gridVelocity[_INDEX(i, j)].y);
		}
		cout << endl;
	}
	step++;
}

void EulerianSimulation::_update(double timestep)
{
	//_force(timestep);

	_advect(timestep);
	//_printVelocity();
	//_diffuse(timestep);
	_project(timestep);
	
	_updateParticlePosition();
	_paintGrid();
}

void EulerianSimulation::_force(double timestep)
{
	int N = _gridCount - 2;
	float tstep = static_cast<float>(timestep);
	for (int i = 0; i < _gridCount; i++)
	{
		for (int j = 0; j < _gridCount; j++)
		{
													//0.0000005f
			//_gridVelocity[_INDEX(i, j)].x -= 2.8f * 1.0f * tstep;
			_gridVelocity[_INDEX(i, j)].y -= 0.0098f * tstep;
		}
	}
	_setBoundary(_gridVelocity);
}

void EulerianSimulation::_advect(double timestep)
{
	float tstep = static_cast<float>(timestep);
	int N = _gridCount - 2;
	float t0step = tstep * N;

	float yMax = _gridPosition[_INDEX(0, N + 1)].y - 0.5f;
	float yMin = _gridPosition[_INDEX(0, 0)].y + 0.5f;
	float xMax = _gridPosition[_INDEX(N + 1, 0)].x - 0.5f;
	float xMin = _gridPosition[_INDEX(0, 0)].x + 0.5f;

	vector<XMFLOAT2> oldVelocity = _gridVelocity;

	//printf("============== step ===========\n");
	for (int i = 1; i <= N; i++)
	{
		for (int j = 1; j <= N; j++)
		{
			XMFLOAT2 backPos =
				XMFLOAT2(
					_gridPosition[_INDEX(i, j)].x - t0step * oldVelocity[_INDEX(i, j)].x,
					_gridPosition[_INDEX(i, j)].y - t0step * oldVelocity[_INDEX(i, j)].y
				);
			if (backPos.x > xMax) backPos.x = xMax;
			else if (backPos.x < xMin) backPos.x = xMin;

			if (backPos.y > yMax) backPos.y = yMax;
			else if (backPos.y < yMin) backPos.y = yMin;

			//printf("(%9f, %9f) ", backPos.x, backPos.y);

			_gridVelocity[_INDEX(i, j)] = _velocityInterpolation(backPos, oldVelocity);
			//printf("interpolation = (%9f, %9f)\n", _gridVelocity[_INDEX(i, j)].x, _gridVelocity[_INDEX(i, j)].y);
			//printf("\n");
		}
		//cout << endl;
	}
	_setBoundary(_gridVelocity);
}

void EulerianSimulation::_diffuse(double timestep)
{

}

void EulerianSimulation::_project(double timestep)
{
	vector<XMFLOAT2> oldVelocity = _gridVelocity;
	static int step = 0;
	//printf("============= step %d =============\n", step);
	//printf("div = \n");
	int N = _gridCount - 2;
	for (int i = 1; i <= N; i++)
	{
		for (int j = 1; j <= N; j++)
		{
			_gridDivergence[_INDEX(i, j)] =
				0.5f * (oldVelocity[_INDEX(i + 1, j)].x - oldVelocity[_INDEX(i - 1, j)].x
					+ oldVelocity[_INDEX(i, j + 1)].y - oldVelocity[_INDEX(i, j - 1)].y) / N ;
			_gridPressure[_INDEX(i, j)] = 0.0f;
			//printf("%f  ", _gridDivergence[_INDEX(i, j)]);
		}
	}

	//printf("\n");

	/*_gridPressure[_INDEX(10, 10)] = 2.0f;
	_gridPressure[_INDEX(10, 11)] = 2.0f;
	_gridPressure[_INDEX(11, 10)] = 2.0f;
	_gridPressure[_INDEX(11, 11)] = 2.0f;*/

	_setBoundary(_gridDivergence);
	_setBoundary(_gridPressure);

	/*for (int i = 0; i < N + 2; i++)
	{
		for (int j = 0; j < N + 2; j++)
		{
			printf("%9f  ", _gridDivergence[_INDEX(i, j)]);
		}
		printf("\n");
	}*/


	//printf("pressure = \n");
	for (int iter = 0; iter < 20; iter++)
	{
		//printf("==iter%d==\n", iter);
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
				//printf("%f ", _gridPressure[_INDEX(i, j)]);
			}

			//printf("\n");
		}
		_setBoundary(_gridPressure);
	}

	/*for (int i = 0; i < N + 2; i++)
	{
		for (int j = 0; j < N + 2; j++)
		{
			printf("%9f  ", _gridPressure[_INDEX(i, j)]);
		}
		printf("\n");
	}*/

	for (int i = 1; i <= N; i++)
	{
		for (int j = 1; j <= N; j++)
		{
			_gridVelocity[_INDEX(i, j)].x -= (_gridPressure[_INDEX(i + 1, j)] - _gridPressure[_INDEX(i - 1, j)]) * 0.5f * N;
			_gridVelocity[_INDEX(i, j)].y -= (_gridPressure[_INDEX(i, j + 1)] - _gridPressure[_INDEX(i, j - 1)]) * 0.5f * N;
		}
	}
	_setBoundary(_gridVelocity); 
	//_printVelocity();

	step++;
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

		vec[_INDEX(N + 1, j)].x = -vec[_INDEX(N, j)].x;
		vec[_INDEX(N + 1, j)].y = +vec[_INDEX(N, j)].y;
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
		scalar[_INDEX(N + 1, j)] = scalar[_INDEX(N, j)];
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
	for (int i = 1; i <= N; i++)
	{
		for (int j = 1; j <= N; j++)
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
		_particleVelocity[i] = _velocityInterpolation(_particlePosition[i], _gridVelocity);

		_particlePosition[i].x += _particleVelocity[i].x * 10.0f;
		_particlePosition[i].y += _particleVelocity[i].y * 10.0f;
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

XMFLOAT2 EulerianSimulation::_velocityInterpolation(XMFLOAT2 pos, vector<XMFLOAT2> oldvel)
{
	// 2. 3.
	int minXIndex = _computeCenterMinMaxIndex(VALUE::MIN, AXIS::X, pos);
	int minYIndex = _computeCenterMinMaxIndex(VALUE::MIN, AXIS::Y, pos);
	int maxXIndex = _computeCenterMinMaxIndex(VALUE::MAX, AXIS::X, pos);
	int maxYIndex = _computeCenterMinMaxIndex(VALUE::MAX, AXIS::Y, pos);

	float xRatio = (pos.x - _gridPosition[_INDEX(minXIndex, minYIndex)].x);
	float yRatio = (pos.y - _gridPosition[_INDEX(minXIndex, minYIndex)].y);

	//printf("  ratio = %f %f  ", xRatio, yRatio);
	//printf("  index = %d %d %d %d   ", minXIndex, maxXIndex, minYIndex, maxYIndex);

	XMFLOAT2 minMinVelocity = oldvel[_INDEX(minXIndex, minYIndex)];
	XMFLOAT2 minMaxVelocity = oldvel[_INDEX(minXIndex, maxYIndex)];
	XMFLOAT2 maxMinVelocity = oldvel[_INDEX(maxXIndex, minYIndex)];
	XMFLOAT2 maxMaxVelocity = oldvel[_INDEX(maxXIndex, maxYIndex)];


	//printf("  indexX = %9f %9f %9f %9f   ", minMinVelocity.x, minMaxVelocity.x, maxMinVelocity.x, maxMaxVelocity.x);
	//printf("  indexY = %9f %9f %9f %9f   ", minMinVelocity.y, minMaxVelocity.y, maxMinVelocity.y, maxMaxVelocity.y);

	//s0* (t0 * d0[IX(i0, j0)] + t1 * d0[IX(i0, j1)]) +
	//	s1 * (t0 * d0[IX(i1, j0)] + t1 * d0[IX(i1, j1)]);
	return XMFLOAT2(
		_interpolation(_interpolation(minMinVelocity.x, minMaxVelocity.x, yRatio), _interpolation(maxMinVelocity.x, maxMaxVelocity.x, yRatio), xRatio),
		_interpolation(_interpolation(minMinVelocity.y, minMaxVelocity.y, yRatio), _interpolation(maxMinVelocity.y, maxMaxVelocity.y, yRatio), xRatio)
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

vector<Vertex> EulerianSimulation::iGetLineVertice()
{
	std::vector<Vertex> vertices;
	int N = _gridCount - 2;
	for (int i = 1; i <= N; i++)
	{
		for (int j = 1; j <= N; j++)
		{
			XMFLOAT2 x = { static_cast<float>(i), static_cast<float>(j) };
			XMFLOAT2 v = { x.x + _gridVelocity[_INDEX(i, j)].x * 300.0f , x.y + _gridVelocity[_INDEX(i, j)].y * 300.0f };
			vertices.push_back(Vertex({ XMFLOAT3(x.x, x.y, -0.0f) }));
			vertices.push_back(Vertex({ XMFLOAT3(v.x, v.y, -0.0f) }));
		}
	}

	return vertices;
}

vector<unsigned int> EulerianSimulation::iGetLineIndice()
{
	std::vector<unsigned int> indices;

	int N = _gridCount - 2;
	for (int i = 0; i <= N * N * 2; i++)
	{
		indices.push_back(i);
	}
	return indices;
}

std::vector<DirectX::XMFLOAT2>& EulerianSimulation::iGetVel()
{
	return _gridVelocity;
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
	for (int i = 0; i < _gridCount; i++)
	{
		for (int j = 0; j < _gridCount; j++)
		{
			// Position
			XMFLOAT2 pos = XMFLOAT2(
				(float)j ,    // "j"
				(float)i );   // "i"

			_gridPosition.push_back(pos);

			struct ConstantBuffer objectCB;
			// Multiply by a specific value to make a stripe
			objectCB.world = transformMatrix(pos.x, pos.y, 0.0f, 0.8f);
			objectCB.worldViewProj = transformMatrix(0.0f, 0.0f, 0.0f);
			objectCB.color = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

			constantBuffer.push_back(objectCB);
		}
	}
	// ###### ###### ###### ######

	// ###### Create particle ######
	for (int i = 0; i < _gridCount; i++)
	{
		for (int j = 0; j < _gridCount; j++)
		{
			// Position
			XMFLOAT2 pos = XMFLOAT2(
				(float)j ,    // "j"
				(float)i );   // "i"

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