#include "GridFluidSim.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace std;

GridFluidSim::GridFluidSim(float timeStep, int delayTime)
	:_timeStep(timeStep), _delayTime(delayTime)
{
}

GridFluidSim::~GridFluidSim()
{
	
}



void GridFluidSim::setGridDomain(int xCount, int yCount)
{
	// 2 are boundaries.
	_gridCount = xCount + 2;
}


void GridFluidSim::initialize()
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
				_gridState.push_back(_STATE::BOUNDARY);
			}
			else if (((N + 1) / 2 - 9 < i)
				&& (i < (N + 1) / 2 + 9)
				&& ((N + 1) / 2 - 9 < j)
				&& (j < (N + 1) / 2 + 9)
				)
			{
				_gridState.push_back(_STATE::FLUID);
			}
			else
			{
				_gridState.push_back(_STATE::AIR);
			}

			_gridVelocity.push_back(XMFLOAT2(0.0f, 0.01f));
			_gridDivergence.push_back(0.0f);
			_gridPressure.push_back(0.0f);
		}
	}

}


void GridFluidSim::_setBoundary(std::vector<XMFLOAT2>& vec)
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

void GridFluidSim::_setBoundary(std::vector<float>& scalar)
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

void GridFluidSim::_paintGrid()
{
	int N = _gridCount - 2;
	// Reset _grid
	for (int i = 1; i <= N; i++)
	{
		for (int j = 1; j <= N; j++)
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


void GridFluidSim::_updateParticlePosition()
{
	for (int i = 0; i < _particlePosition.size(); i++)
	{
		// 2. 3.
		_particleVelocity[i] = _velocityInterpolation(_particlePosition[i], _gridVelocity);
		_particlePosition[i] += _particleVelocity[i] * _timeStep * 1.0f;
	}
}


// To calculate the grid index, the calculation result must not depend on the _gridScale.
// Therefore, the intermediate computed variable "should not be multiplied by the _gridScale".
// For example, if the scale is 1.0f, the result is (index * 1.0f).
// But if the scale is 0.5f, the result is (index * 0.5f).
// The index value should of course be immutable.
int GridFluidSim::_computeFaceMinMaxIndex(_VALUE vState, _AXIS axis, XMFLOAT2 particlePos)
{
	float pos = (axis == _AXIS::X) ? particlePos.x : particlePos.y;
	float value;

	// Compute position by normalizing from (-N, N) to (0, N + 1)
	switch (vState)
	{
	case _VALUE::MIN:
		value = pos + 0.5f - 0.5f * _particleScale;
		break;
	case _VALUE::MAX:
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
int GridFluidSim::_computeCenterMinMaxIndex(_VALUE vState, _AXIS axis, XMFLOAT2 particlePos)
{
	float pos = (axis == _AXIS::X) ? particlePos.x : particlePos.y;
	// 2.
	float value = pos;

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

XMFLOAT2 GridFluidSim::_velocityInterpolation(XMFLOAT2 pos, vector<XMFLOAT2> oldvel)
{
	// 2. 3.
	int minXIndex = _computeCenterMinMaxIndex(_VALUE::MIN, _AXIS::X, pos);
	int minYIndex = _computeCenterMinMaxIndex(_VALUE::MIN, _AXIS::Y, pos);
	int maxXIndex = _computeCenterMinMaxIndex(_VALUE::MAX, _AXIS::X, pos);
	int maxYIndex = _computeCenterMinMaxIndex(_VALUE::MAX, _AXIS::Y, pos);

	float xRatio = (pos.x - _gridPosition[_INDEX(minXIndex, minYIndex)].x);
	float yRatio = (pos.y - _gridPosition[_INDEX(minXIndex, minYIndex)].y);

	XMFLOAT2 minMinVelocity = oldvel[_INDEX(minXIndex, minYIndex)];
	XMFLOAT2 minMaxVelocity = oldvel[_INDEX(minXIndex, maxYIndex)];
	XMFLOAT2 maxMinVelocity = oldvel[_INDEX(maxXIndex, minYIndex)];
	XMFLOAT2 maxMaxVelocity = oldvel[_INDEX(maxXIndex, maxYIndex)];

	// s0* (t0 * d0[IX(i0, j0)] + t1 * d0[IX(i0, j1)]) +
	//	s1 * (t0 * d0[IX(i1, j0)] + t1 * d0[IX(i1, j1)]);
	// minMinVelocity.x and minMinVelocity.x ​​can be different because they are "velocity", not position.
	return XMFLOAT2(
		_interpolation(_interpolation(minMinVelocity.x, minMaxVelocity.x, yRatio), _interpolation(maxMinVelocity.x, maxMaxVelocity.x, yRatio), xRatio),
		_interpolation(_interpolation(minMinVelocity.y, minMaxVelocity.y, yRatio), _interpolation(maxMinVelocity.y, maxMaxVelocity.y, yRatio), xRatio)
	);
}

float GridFluidSim::_interpolation(float value1, float value2, float ratio)
{
	return value1 * (1.0f - ratio) + value2 * ratio;
}



#pragma region Implementation
// ################################## Implementation ####################################
void GridFluidSim::iUpdate()
{
	float standardTimeStep = 0.1f;
	int maxSize = static_cast<int>(standardTimeStep / _timeStep);
	for (int i = 0; i < maxSize; i++)
	{
		update();
	}

	Sleep(_delayTime);
}

void GridFluidSim::iResetSimulationState(vector<ConstantBuffer>& constantBuffer)
{
	_gridState.clear();
	_gridPressure.clear();
	_gridVelocity.clear();
	_gridDivergence.clear();

	constantBuffer.clear();
	_gridPosition.clear();
	_particlePosition.clear();
	_particleVelocity.clear();

	initialize();
	iCreateObjectParticle(constantBuffer);
}

vector<Vertex> GridFluidSim::iGetVertice()
{
	vector<Vertex> vertices;

	vertices.push_back(Vertex({ DirectX::XMFLOAT3(-0.5f, -0.5f, -0.0f) }));
	vertices.push_back(Vertex({ DirectX::XMFLOAT3(-0.5f, +0.5f, -0.0f) }));
	vertices.push_back(Vertex({ DirectX::XMFLOAT3(+0.5f, +0.5f, -0.0f) }));
	vertices.push_back(Vertex({ DirectX::XMFLOAT3(+0.5f, -0.5f, -0.0f) }));

	int N = _gridCount - 2;
	for (int i = 1; i <= N; i++)
	{
		for (int j = 1; j <= N; j++)
		{
			XMFLOAT2 x = { static_cast<float>(i), static_cast<float>(j) };
			XMFLOAT2 v = { x.x + _gridVelocity[_INDEX(i, j)].x * 5.0f , x.y + _gridVelocity[_INDEX(i, j)].y * 5.0f };
			vertices.push_back(Vertex({ XMFLOAT3(x.x, x.y, -2.0f) }));
			vertices.push_back(Vertex({ XMFLOAT3(v.x, v.y, -2.0f) }));
		}
	}

	return vertices;
}

vector<unsigned int> GridFluidSim::iGetIndice()
{
	std::vector<unsigned int> indices;
	indices.push_back(0); indices.push_back(1); indices.push_back(2);
	indices.push_back(0); indices.push_back(2); indices.push_back(3);

	int N = _gridCount - 2;
	for (int i = 0; i <= N * N * 2; i++)
	{
		indices.push_back(i);
	}

	return indices;
}

XMFLOAT4 GridFluidSim::_getColor(int i)
{
	float magnitude;
	switch (_gridState[i])
	{
	case _STATE::FLUID:
		magnitude = sqrtf(powf(_gridVelocity[i].x, 2.0f) + powf(_gridVelocity[i].y, 2.0f));

		/*if (magnitude > 0.05f && magnitude < 0.2f)
			return XMFLOAT4(0.8f, 1.0f, 0.0f, 1.0f);
		else if (magnitude >= 0.2f)
			return XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
		else
			return XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);*/

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

int GridFluidSim::iGetObjectCount()
{
	return _gridCount;
}


void GridFluidSim::iUpdateConstantBuffer(std::vector<ConstantBuffer>& constantBuffer, int i)
{
	int objectEndIndex = _gridCount * _gridCount;
	int size = constantBuffer.size();

	// Set object color					
	if (i < objectEndIndex)
	{
		constantBuffer[i].color = _getColor(i);
	}
	// Set particle position
	else if (i >= objectEndIndex && i < size - 1)
	{												// Due to velocity field
		int particleIndex = i - objectEndIndex;
		XMFLOAT2 pos = _particlePosition[particleIndex];

		constantBuffer[i].world._41 = pos.x;
		constantBuffer[i].world._42 = pos.y;

	}// Set velocity
	else
	{

	}

}

void GridFluidSim::iDraw(ComPtr<ID3D12GraphicsCommandList>& mCommandList, int size, UINT indexCount, bool* drawFlag, int i)
{
	int objectEndIndex = _gridCount * _gridCount;

	if (i < objectEndIndex)
	{
		if (drawFlag[static_cast<int>(FLAG::GRID)])
		{
			mCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			mCommandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
		}
	}
	else if (i >= objectEndIndex && i < size - 1)
	{
		if (drawFlag[static_cast<int>(FLAG::PARTICLE)])
		{
			mCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			mCommandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
		}
	}
	else
	{
		if (drawFlag[static_cast<int>(FLAG::VELOCITY)])
		{
			mCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
			mCommandList->DrawIndexedInstanced(
				indexCount - 6, //count
				1,
				6, //  index start 
				4, //  vertex start
				0);
		}
	}
}

void GridFluidSim::iCreateObjectParticle(vector<ConstantBuffer>& constantBuffer)
{
	// ###### Create Object ######
	for (int i = 0; i < _gridCount; i++)
	{
		for (int j = 0; j < _gridCount; j++)
		{
			// Position
			XMFLOAT2 pos = XMFLOAT2(
				(float)j,    // "j"
				(float)i);   // "i"

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
				(float)j,    // "j"
				(float)i);   // "i"

			if (_gridState[_INDEX(i, j)] == _STATE::FLUID)
			{
				for (int k = 0; k < _particleCount; k++)
				{
					for (int m = 0; m < _particleCount; m++)
					{
						_particleVelocity.push_back(XMFLOAT2(0.0f, 0.0f));
						_particlePosition.push_back(
							{ -0.3f + pos.x + k * _particleScale * 1.1f,
							  -0.3f + pos.y + m * _particleScale * 1.1f });

						struct ConstantBuffer particleCB;
						particleCB.world = transformMatrix(pos.x, pos.y, -1.0f, _particleScale);
						particleCB.worldViewProj = transformMatrix(0.0f, 0.0f, 0.0f);
						particleCB.color = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);

						constantBuffer.push_back(particleCB);
					}

				}

			}
		}
	}
	// ###### ###### ###### ######


	// ###### Create Velocity ######
	struct ConstantBuffer velocityCB;
	velocityCB.world = transformMatrix(0.0f, 0.0f, 0.0f, 1.0f);
	velocityCB.worldViewProj = transformMatrix(0.0f, 0.0f, 0.0f);
	velocityCB.color = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	constantBuffer.push_back(velocityCB);
	// ###### ###### ###### ######
}
// #######################################################################################
#pragma endregion