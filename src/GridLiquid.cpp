#include "GridLiquid.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace std;

GridLiquid::GridLiquid(GridData& index, float timeStep)
	:_INDEX(index)
{
	_gridCount = _INDEX.gridCount;
	_particleCount = _INDEX.particleCount;
	_timeStep = timeStep;
}

GridLiquid::~GridLiquid()
{
	
}

void GridLiquid::setInterp(Interpolation* interp)
{
	_interp = interp;
}

void GridLiquid::_initialize(EX ex)
{
	// Set _fluid
	for (int i = 0; i < _gridCount; i++)
	{
		for (int j = 0; j < _gridCount; j++)
		{
			_computeGridState(ex, i, j);

			_gridVelocity.push_back(XMFLOAT2(0.0f, 0.0f));
			_gridDivergence.push_back(0.0f);
			_gridPressure.push_back(0.0f);
		}
	}
	//_gridState[_INDEX(4, 4)] = STATE::LIQUID;

	_paintSurface();
}

void GridLiquid::_computeGridState(EX ex, int i, int j)
{
	int N = _gridCount - 2;
	int offset = _gridCount / 4;

	switch (ex)
	{
	case EX::DAM:
		if (i == 0 || j == 0 
		|| i == N + 1 || j == N + 1) 
		{ 
			_gridState.push_back(STATE::BOUNDARY); 
		}
		else if ((N + 1) / 3 - offset < i 
			&& (i < (N + 1) / 2 + 1)
			&& (2 < j)  //((N + 1) / 2 - offset < j)     
			&& (j < (N) - offset))            
		{
			_gridState.push_back(STATE::LIQUID);
		}
		else 
		{ 
			_gridState.push_back(STATE::AIR); 
		}
		break;

	case EX::DROP1:
		if (i == 0 || j == 0 
		|| i == N + 1 || j == N + 1) 
		{ 
			_gridState.push_back(STATE::BOUNDARY); 
		}
		else if ((N + 1) / 2 - offset < i 
			&& (i < (N - 4))
			&& ((N + 1) / 2 < j)  //((N + 1) / 2 - offset < j)     
			&& (j < (N)))
		//else if ((N + 1) / 2 - offset < i 
		//	&& (i < (N))
		//	&& ((N + 1) / 2 - offset < j)  //((N + 1) / 2 - offset < j)     
		//	&& (j < (N + 1) / 2 + offset))
		{
			_gridState.push_back(STATE::LIQUID);
		}
		else
		{
			_gridState.push_back(STATE::AIR);
		}
		break;

	case EX::DROP2:
		if (i == 0 || j == 0 
		|| i == N + 1 || j == N + 1)
		{
			_gridState.push_back(STATE::BOUNDARY);
		}
		else if
			(
				(
					((N + 1) / 2 - offset > i)
					&& (i > 0)

					||

					(i > (N + 1) / 2 + offset + 1)
					&&
					(i < N + 1)
					)

				&& ((N + 1) / 2 < j)
				&& (j < N)

				)
		{
			_gridState.push_back(STATE::LIQUID);
		}
		else
		{
			_gridState.push_back(STATE::AIR);
		}
		break;

	}
	
}

void GridLiquid::_setFreeSurface(std::vector<XMFLOAT2>& vec)
{
	int N = _gridCount - 2;

	// Free surface boundary
	for (int i = 1; i <= N; i++)
	{
		for (int j = 1; j <= N; j++)
		{
			if (_gridState[_INDEX(i, j)] == STATE::SURFACE)
			{
				XMFLOAT2 temp = { 0.0f, 0.0f };
				int count = 0;

				if (_gridState[_INDEX(i + 1, j)] == STATE::LIQUID)
				{
					temp += vec[_INDEX(i + 1, j)];
					count++;
				}

				if (_gridState[_INDEX(i - 1, j)] == STATE::LIQUID)
				{
					temp += vec[_INDEX(i - 1, j)];
					count++;
				}

				if (_gridState[_INDEX(i, j + 1)] == STATE::LIQUID)
				{
					temp += vec[_INDEX(i, j + 1)];
					count++;
				}

				if (_gridState[_INDEX(i, j - 1)] == STATE::LIQUID)
				{
					temp += vec[_INDEX(i, j - 1)];
					count++;
				}

				if (count > 0)
				{
					vec[_INDEX(i, j)] = temp / static_cast<float>(count);
				}
			}


		}
	}
}

void GridLiquid::_setBoundary(std::vector<XMFLOAT2>& vec)
{
	int N = _gridCount - 2;
	for (int i = 1; i <= N; i++)
	{
		for (int j = 1; j <= N; j++)
		{
			if ( ((_gridState[_INDEX(i - 1, j)] == STATE::BOUNDARY) && (vec[_INDEX(i, j)].x < EPS_FLOAT))
				||
				((_gridState[_INDEX(i + 1, j)] == STATE::BOUNDARY) && (vec[_INDEX(i, j)].x > EPS_FLOAT)) )
			{
				vec[_INDEX(i, j)].x = 0.0f;
			}

			if (((_gridState[_INDEX(i, j - 1)] == STATE::BOUNDARY) && (vec[_INDEX(i, j)].y < EPS_FLOAT))
				||
				((_gridState[_INDEX(i, j + 1)] == STATE::BOUNDARY) && (vec[_INDEX(i, j)].y > EPS_FLOAT)))
			{
				vec[_INDEX(i, j)].y = 0.0f;
			}
		}
	}


	//// (x, 0) (x, yMax+1)
	//for (int i = 1; i <= N; i++)
	//{
	//	vec[_INDEX(i, 0)].x = +vec[_INDEX(i, 1)].x;
	//	vec[_INDEX(i, 0)].y = -vec[_INDEX(i, 1)].y;

	//	vec[_INDEX(i, N + 1)].x = +vec[_INDEX(i, N)].x;
	//	vec[_INDEX(i, N + 1)].y = -vec[_INDEX(i, N)].y;
	//}

	//// (0, y) (xMax+1, y)
	//for (int j = 1; j <= N; j++)
	//{
	//	vec[_INDEX(0, j)].x = -vec[_INDEX(1, j)].x;
	//	vec[_INDEX(0, j)].y = +vec[_INDEX(1, j)].y;

	//	vec[_INDEX(N + 1, j)].x = -vec[_INDEX(N, j)].x;
	//	vec[_INDEX(N + 1, j)].y = +vec[_INDEX(N, j)].y;
	//}

	//// (0, 0)
	//vec[_INDEX(0, 0)].x = vec[_INDEX(0, 1)].x;
	//vec[_INDEX(0, 0)].y = vec[_INDEX(1, 0)].y;
	//// (0, yCount)
	//vec[_INDEX(0, N + 1)].x = vec[_INDEX(0, N)].x;
	//vec[_INDEX(0, N + 1)].y = vec[_INDEX(1, N + 1)].y ;
	//// (xCount, 0)
	//vec[_INDEX(N + 1, 0)].x = vec[_INDEX(N + 1, 1)].x;
	//vec[_INDEX(N + 1, 0)].y = vec[_INDEX(N, 0)].y;
	//// (xCount, yCount)
	//vec[_INDEX(N + 1, N + 1)].x = vec[_INDEX(N + 1, N)].x;
	//vec[_INDEX(N + 1, N + 1)].y = vec[_INDEX(N, N + 1)].y;
}

void GridLiquid::_setBoundary(std::vector<float>& scalar)
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

void GridLiquid::_paintLiquid()
{
	int N = _gridCount - 2;

	// Reset grid
	for (int i = 1; i <= N; i++)
	{
		for (int j = 1; j <= N; j++)
		{
			_gridState[_INDEX(i, j)] = STATE::AIR;
		}
	}


	for (int i = 0; i < _particlePosition.size(); i++)
	{
		XMINT2 minIndex = _computeFaceMinMaxIndex(VALUE::MIN, _particlePosition[i]);
		XMINT2 maxIndex = _computeFaceMinMaxIndex(VALUE::MAX, _particlePosition[i]);

		// Painting
		STATE& minMin = _gridState[_INDEX(minIndex.x, minIndex.y)];
		STATE& minMax = _gridState[_INDEX(minIndex.x, maxIndex.y)];
		STATE& maxMin = _gridState[_INDEX(maxIndex.x, minIndex.y)];
		STATE& maxMax = _gridState[_INDEX(maxIndex.x, maxIndex.y)];

		// Boundary Checking
		if (minMin != STATE::BOUNDARY) minMin = STATE::LIQUID;
		if (minMax != STATE::BOUNDARY) minMax = STATE::LIQUID;
		if (maxMin != STATE::BOUNDARY) maxMin = STATE::LIQUID;
		if (maxMax != STATE::BOUNDARY) maxMax = STATE::LIQUID;
	}


	// Surface painting 
	_paintSurface();
}

void GridLiquid::_paintSurface()
{
	int N = _gridCount - 2;
	for (int i = 1; i <= N; i++)
	{
		for (int j = 1; j <= N; j++)
		{
			if (_gridState[_INDEX(i, j)] == STATE::LIQUID)
			{
				if (_gridState[_INDEX(i + 1, j)] == STATE::AIR)
					_gridState[_INDEX(i + 1, j)] = STATE::SURFACE;

				if (_gridState[_INDEX(i - 1, j)] == STATE::AIR)
					_gridState[_INDEX(i - 1, j)] = STATE::SURFACE;

				if (_gridState[_INDEX(i, j + 1)] == STATE::AIR)
					_gridState[_INDEX(i, j + 1)] = STATE::SURFACE;

				if (_gridState[_INDEX(i, j - 1)] == STATE::AIR)
					_gridState[_INDEX(i, j - 1)] = STATE::SURFACE;
			}
		}
	}
}

// To calculate the grid index, the calculation result must not depend on the _gridScale.
// Therefore, the intermediate computed variable "should not be multiplied by the _gridScale".
// For example, if the scale is 1.0f, the result is (index * 1.0f).
// But if the scale is 0.5f, the result is (index * 0.5f).
// The index value should of course be immutable.
XMINT2 GridLiquid::_computeFaceMinMaxIndex(VALUE vState, XMFLOAT2 particlePos)
{
	XMFLOAT2 value;

	// Compute position by normalizing from (-N, N) to (0, N + 1)
	switch (vState)
	{
	case VALUE::MIN:
		value = particlePos + 0.5f; //- 0.5f * _particleScale;
		break;
	case VALUE::MAX:
		value = particlePos + 0.5f; //+ 0.5f * _particleScale;
		break;
	default:
		value = { 0.0f, 0.0f };
		break;
	}

	// Compute Index
	return { static_cast<int>(floor(value.x)), static_cast<int>(floor(value.y)) };
}


XMFLOAT4 GridLiquid::_getColor(int i)
{

	switch (_gridState[i])
	{
	case STATE::LIQUID:
		return XMFLOAT4(0.2f, 0.5f, 0.5f, 1.0f);
		break;

	case STATE::BOUNDARY:
		return XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
		break;

	case STATE::AIR:
		return XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
		break;

	case STATE::SURFACE:
		return XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
		break;

	default:
		return XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		break;
	}
}



#pragma region Implementation
// ################################## Implementation ####################################
void GridLiquid::iUpdate()
{
	//float timeStep;
	//for (float i = 0; i <= FPS_60; i += timeStep)
	//{
		_update();
	//}

	//Sleep(_delayTime);
}

void GridLiquid::iResetSimulationState(vector<ConstantBuffer>& constantBuffer, EX ex)
{
	_gridState.clear();
	_gridPressure.clear();
	_gridVelocity.clear();
	_gridDivergence.clear();

	constantBuffer.clear();
	_gridPosition.clear();
	_particlePosition.clear();
	_particleVelocity.clear();

	_initialize(ex);
	iCreateObjectParticle(constantBuffer);
}

vector<Vertex> GridLiquid::iGetVertice()
{
	vector<Vertex> vertices;

	vertices.push_back(Vertex({ DirectX::XMFLOAT3(-0.5f, -0.5f, -0.0f) }));
	vertices.push_back(Vertex({ DirectX::XMFLOAT3(-0.5f, +0.5f, -0.0f) }));
	vertices.push_back(Vertex({ DirectX::XMFLOAT3(+0.5f, +0.5f, -0.0f) }));
	vertices.push_back(Vertex({ DirectX::XMFLOAT3(+0.5f, -0.5f, -0.0f) }));

	int N = _gridCount - 2;
	for (int i = 0; i < _gridCount; i++)
	{
		for (int j = 0; j < _gridCount; j++)
		{
			XMFLOAT2 x = { static_cast<float>(i), static_cast<float>(j) };
			XMFLOAT2 v = { x.x + _gridVelocity[_INDEX(i, j)].x * 0.5f , x.y + _gridVelocity[_INDEX(i, j)].y * 0.5f };
			vertices.push_back(Vertex({ XMFLOAT3(x.x, x.y, -2.0f) }));
			vertices.push_back(Vertex({ XMFLOAT3(v.x, v.y, -2.0f) }));
		}
	}

	return vertices;
}

vector<unsigned int> GridLiquid::iGetIndice()
{
	std::vector<unsigned int> indices;
	indices.push_back(0); indices.push_back(1); indices.push_back(2);
	indices.push_back(0); indices.push_back(2); indices.push_back(3);

	int N = _gridCount - 2;
									// The number of lines needs to be doubled because it needs "position" and "direction".
	for (int i = 0; i <= _gridCount * _gridCount * 2; i++)
	{
		indices.push_back(i);
	}

	return indices;
}

int GridLiquid::iGetObjectCount()
{
	return _gridCount;
}

void GridLiquid::iCreateObjectParticle(vector<ConstantBuffer>& constantBuffer)
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

			if (_gridState[_INDEX(i, j)] == STATE::LIQUID)
			{
				for (int k = 0; k < _particleCount; k++)
				{
					for (int m = 0; m < _particleCount; m++)
					{
						_particleVelocity.push_back(XMFLOAT2(0.0f, 0.0f));
						_particlePosition.push_back(
							{ -0.3f + pos.y + k * _particleScale * 1.1f,    // y
							  -0.3f + pos.x + m * _particleScale * 1.1f }); // x

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

void GridLiquid::iUpdateConstantBuffer(std::vector<ConstantBuffer>& constantBuffer, int i)
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

void GridLiquid::iDraw(ComPtr<ID3D12GraphicsCommandList>& mCommandList, int size, UINT indexCount, bool* drawFlag, int i)
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

// #######################################################################################
#pragma endregion