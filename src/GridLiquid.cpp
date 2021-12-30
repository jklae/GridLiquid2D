#include "GridLiquid.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace std;
using namespace DXViewer::xmfloat2;
using namespace DXViewer::xmint2;

GridLiquid::GridLiquid(int x, int y, float timeStep)
{
	_gridCount.x = x + 2; // 2 are boundaries.
	_gridCount.y = y + 2;

	_particleCount = 3;
	_timeStep = timeStep;
}

GridLiquid::~GridLiquid()
{
}

void GridLiquid::_initialize(EX ex)
{
	// Set _fluid
	for (int j = 0; j < _gridCount.y; j++)
	{
		for (int i = 0; i < _gridCount.x; i++)
		{
			_computeGridState(ex, i, j);

			_gridDivergence.push_back(0.0f);
			_gridPressure.push_back(0.0f);
		}
	}
	_paintSurface();
}

void GridLiquid::_computeGridState(EX ex, int i, int j)
{
	XMINT2 N = _gridCount - 2;
	int offset = _gridCount.x / 10;

	switch (ex)
	{
	case EX::DAM:
		if (i == 0 || i == N.x + 1
		||  j == 0 || j == N.y + 1)
		{ 
			_gridState.push_back(STATE::BOUNDARY); 
		}
		else if ((2 < i)
			&& (i < (N.x + 1) / 2 - offset)
			&& (2 < j)  
			&& (j < ((N.y + 1) / 2 + offset * 2.0f)))
		{
			_gridState.push_back(STATE::LIQUID);
		}
		else 
		{ 
			_gridState.push_back(STATE::AIR); 
		}

		_gridVelocity.push_back(XMFLOAT2(0.0f, 0.0f));
		break;

	case EX::DROP:
		if (i == 0 || i == N.x + 1
		 || j == 0 || j == N.y + 1)
		{ 
			_gridState.push_back(STATE::BOUNDARY); 
		}
		else if ((N.x + 1) / 2 - offset * 2.3f < i
			&& (i < (N.x + 1) / 2 + offset * 2.3f)
			&& ((N.y + 1) / 2 + offset< j)
			&& (j < N.y - 2))
		{
			_gridState.push_back(STATE::LIQUID);
		}
		else
		{
			_gridState.push_back(STATE::AIR);
		}

		_gridVelocity.push_back(XMFLOAT2(0.0f, 0.0f));
		break;

	}
	
}

void GridLiquid::_setFreeSurface(std::vector<XMFLOAT2>& vec)
{
	XMINT2 N = _gridCount - 2;

	// Free surface boundary
	for (int j = 1; j <= N.y; j++)
	{
		for (int i = 1; i <= N.x; i++)
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
	XMINT2 N = _gridCount - 2;

	// (x, 0) (x, yMax+1)
	for (int i = 1; i <= N.x; i++)
	{
		if (vec[_INDEX(i, 1)].y < EPS_FLOAT) { vec[_INDEX(i, 1)].y = 0.0f; }
		if (vec[_INDEX(i, N.y)].y > EPS_FLOAT) { vec[_INDEX(i, N.y)].y = 0.0f; }

		vec[_INDEX(i, 0)].x = vec[_INDEX(i, 1)].x;
		vec[_INDEX(i, 0)].y = vec[_INDEX(i, 1)].y;

		vec[_INDEX(i, N.y + 1)].x = vec[_INDEX(i, N.y)].x;
		vec[_INDEX(i, N.y + 1)].y = vec[_INDEX(i, N.y)].y;
	}

	// (0, y) (xMax+1, y)
	for (int j = 1; j <= N.y; j++)
	{
		if (vec[_INDEX(1, j)].x < EPS_FLOAT) { vec[_INDEX(1, j)].x = 0.0f; }
		if (vec[_INDEX(N.x, j)].x > EPS_FLOAT) { vec[_INDEX(N.x, j)].x = 0.0f; }

		vec[_INDEX(0, j)].x = vec[_INDEX(1, j)].x;
		vec[_INDEX(0, j)].y = vec[_INDEX(1, j)].y;

		vec[_INDEX(N.x + 1, j)].x = vec[_INDEX(N.x, j)].x;
		vec[_INDEX(N.x + 1, j)].y = vec[_INDEX(N.x, j)].y;
	}

	// (0, 0)
	vec[_INDEX(0, 0)].x = vec[_INDEX(0, 1)].x;
	vec[_INDEX(0, 0)].y = vec[_INDEX(1, 0)].y;
	// (0, yCount)
	vec[_INDEX(0, N.y + 1)].x = vec[_INDEX(0, N.y)].x;
	vec[_INDEX(0, N.y + 1)].y = vec[_INDEX(1, N.y + 1)].y ;
	// (xCount, 0)
	vec[_INDEX(N.x + 1, 0)].x = vec[_INDEX(N.x + 1, 1)].x;
	vec[_INDEX(N.x + 1, 0)].y = vec[_INDEX(N.x, 0)].y;
	// (xCount, yCount)
	vec[_INDEX(N.x + 1, N.y + 1)].x = vec[_INDEX(N.x + 1, N.y)].x;
	vec[_INDEX(N.x + 1, N.y + 1)].y = vec[_INDEX(N.x, N.y + 1)].y;

	//// (0, 0)
	//vec[_INDEX(0, 0)] = (vec[_INDEX(0, 1)] + vec[_INDEX(1, 0)]) / 2.0f;
	//// (0, yCount)
	//vec[_INDEX(0, N + 1)] = (vec[_INDEX(0, N)] + vec[_INDEX(1, N + 1)]) / 2.0f;
	//// (xCount, 0)
	//vec[_INDEX(N + 1, 0)] = (vec[_INDEX(N + 1, 1)] + vec[_INDEX(N, 0)]) / 2.0f;
	//// (xCount, yCount)
	//vec[_INDEX(N + 1, N + 1)] = (vec[_INDEX(N + 1, N)] + vec[_INDEX(N, N + 1)]) / 2.0f;
}

void GridLiquid::_setBoundary(std::vector<float>& scalar)
{
	XMINT2 N = _gridCount - 2;

	// (x, 0) (x, yMax+1)
	for (int i = 1; i <= N.x; i++)
	{
		scalar[_INDEX(i, 0)] = scalar[_INDEX(i, 1)];
		scalar[_INDEX(i, N.y + 1)] = scalar[_INDEX(i, N.y)];
	}

	// (0, y) (xMax+1, y)
	for (int j = 1; j <= N.y; j++)
	{
		scalar[_INDEX(0, j)] = scalar[_INDEX(1, j)];
		scalar[_INDEX(N.x + 1, j)] = scalar[_INDEX(N.x, j)];
	}

	// (0, 0)
	scalar[_INDEX(0, 0)] = (scalar[_INDEX(0, 1)] + scalar[_INDEX(1, 0)]) / 2.0f;
	// (0, yCount)
	scalar[_INDEX(0, N.y + 1)] = (scalar[_INDEX(0, N.y)] + scalar[_INDEX(1, N.y + 1)]) / 2.0f;
	// (xCount, 0)
	scalar[_INDEX(N.x + 1, 0)] = (scalar[_INDEX(N.x + 1, 1)] + scalar[_INDEX(N.x, 0)]) / 2.0f;
	// (xCount, yCount)
	scalar[_INDEX(N.x + 1, N.y + 1)] = (scalar[_INDEX(N.x + 1, N.y)] + scalar[_INDEX(N.x, N.y + 1)]) / 2.0f;
}

void GridLiquid::_paintLiquid()
{
	XMINT2 N = _gridCount - 2;

	// Reset grid
	for (int j = 1; j <= N.y; j++)
	{
		for (int i = 1; i <= N.x; i++)
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
	XMINT2 N = _gridCount - 2;
	for (int j = 1; j <= N.y; j++)
	{
		for (int i = 1; i <= N.x; i++)
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
		value = particlePos + 0.5f - 0.5f * _particleScale;
		break;
	case VALUE::MAX:
		value = particlePos + 0.5f + 0.5f * _particleScale;
		break;
	default:
		value = { 0.0f, 0.0f };
		break;
	}

	// Compute Index
	return { static_cast<int>(floor(value.x)), static_cast<int>(floor(value.y)) };
}

// Different from _computeFaceMinMaxIndex().
// 1. Subtract the count of offset by 1.
// 2. Do not subtract particleStride from min, max calculation.
// 3. ceil maxIndex instead of floor.
// ------------------------------------------------------------------
// _PaintGrid() uses the face as the transition point.
// _updateParticlePosition() uses the center as the transition point.
XMINT2 GridLiquid::_computeCenterMinMaxIndex(VALUE vState, XMFLOAT2 particlePos)
{
	// 2.
	switch (vState)
	{
	case VALUE::MIN:
		return { static_cast<int>(floor(particlePos.x)), static_cast<int>(floor(particlePos.y)) };
		break;
	case VALUE::MAX:
		// 3.
		return { static_cast<int>(ceil(particlePos.x)), static_cast<int>(ceil(particlePos.y)) };
		break;
	default:
		return { -1, -1 };
		break;
	}
}

																	// for semi-Lagrangian and FLIP
XMFLOAT2 GridLiquid::gridToParticle(XMFLOAT2 particlePos, vector<XMFLOAT2>& oldVel)
{
	XMFLOAT2 pos = particlePos;

	// 2. 3.
	XMINT2 minIndex = _computeCenterMinMaxIndex(VALUE::MIN, pos);
	XMINT2 maxIndex = _computeCenterMinMaxIndex(VALUE::MAX, pos);

	XMFLOAT2 ratio = (pos - _gridPosition[_INDEX(minIndex.x, minIndex.y)]);

	float minMinRatio = _gridState[_INDEX(minIndex.x, minIndex.y)] == STATE::LIQUID ? (1.0f - ratio.x) * (1.0f - ratio.y) : 0.0f;
	float minMaxRatio = _gridState[_INDEX(minIndex.x, maxIndex.y)] == STATE::LIQUID ? (1.0f - ratio.x) * ratio.y : 0.0f;
	float maxMinRatio = _gridState[_INDEX(maxIndex.x, minIndex.y)] == STATE::LIQUID ? ratio.x * (1.0f - ratio.y) : 0.0f;
	float maxMaxRatio = _gridState[_INDEX(maxIndex.x, maxIndex.y)] == STATE::LIQUID ? ratio.x * ratio.y : 0.0f;

	// Normalization
	float totalRatio = minMinRatio + minMaxRatio + maxMinRatio + maxMaxRatio;
	if (totalRatio > EPS_FLOAT)
	{
		minMinRatio /= totalRatio;
		minMaxRatio /= totalRatio;
		maxMinRatio /= totalRatio;
		maxMaxRatio /= totalRatio;
	}

	XMFLOAT2 minMinVel = oldVel[_INDEX(minIndex.x, minIndex.y)];
	XMFLOAT2 minMaxVel = oldVel[_INDEX(minIndex.x, maxIndex.y)];
	XMFLOAT2 maxMinVel = oldVel[_INDEX(maxIndex.x, minIndex.y)];
	XMFLOAT2 maxMaxVel = oldVel[_INDEX(maxIndex.x, maxIndex.y)];

	return
		minMinVel * minMinRatio + minMaxVel * minMaxRatio
		+ maxMinVel * maxMinRatio + maxMaxVel * maxMaxRatio;

}



XMFLOAT4 GridLiquid::_getGridColor(int i)
{
	switch (_gridState[i])
	{
	case STATE::LIQUID:
		return XMFLOAT4(0.2f, 0.5f, 0.5f, 1.0f);
		break;

	case STATE::BOUNDARY:
		return XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
		break;

	case STATE::AIR:
		return XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
		break;

	case STATE::SURFACE:
		return XMFLOAT4(0.2f, 0.4f, 0.4f, 1.0f);
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
	int iter = 0;
	
	if (fabsf(_timeStep - FPS_30) < EPS_FLOAT)
		iter = 3;
	else if (fabsf(_timeStep - FPS_60) < EPS_FLOAT)
		iter = 6;
	else if (fabsf(_timeStep - FPS_90) < EPS_FLOAT)
		iter = 9;
	else if (fabsf(_timeStep - FPS_120) < EPS_FLOAT)
		iter = 12;
	else
		iter = 1;

	//float timeStep;
	for (int i = 0; i < iter; i++)
	{
		_update();
	}
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
	iCreateObject(constantBuffer);
}

vector<Vertex>& GridLiquid::iGetVertice()
{
	_vertices.clear();

	_vertices.push_back(Vertex({ DirectX::XMFLOAT3(-0.5f, -0.5f, -0.0f) }));
	_vertices.push_back(Vertex({ DirectX::XMFLOAT3(-0.5f, +0.5f, -0.0f) }));
	_vertices.push_back(Vertex({ DirectX::XMFLOAT3(+0.5f, +0.5f, -0.0f) }));
	_vertices.push_back(Vertex({ DirectX::XMFLOAT3(+0.5f, -0.5f, -0.0f) }));

	for (int j = 0; j < _gridCount.y; j++)
	{
		for (int i = 0; i < _gridCount.x; i++)
		{
			XMFLOAT2 x = { static_cast<float>(i), static_cast<float>(j) };
			XMFLOAT2 v = { x.x + _gridVelocity[_INDEX(i, j)].x * 0.1f , x.y + _gridVelocity[_INDEX(i, j)].y * 0.1f };
			_vertices.push_back(Vertex({ XMFLOAT3(x.x, x.y, -2.0f) }));
			_vertices.push_back(Vertex({ XMFLOAT3(v.x, v.y, -2.0f) }));
		}
	}

	return _vertices;
}

vector<unsigned int>& GridLiquid::iGetIndice()
{
	_indices.clear();

	_indices.push_back(0); _indices.push_back(1); _indices.push_back(2);
	_indices.push_back(0); _indices.push_back(2); _indices.push_back(3);

									// The number of lines needs to be doubled because it needs "position" and "direction".
	for (int i = 0; i <= _gridCount.x * _gridCount.y * 2; i++)
	{
		_indices.push_back(i);
	}

	return _indices;
}

UINT GridLiquid::iGetVertexBufferSize()
{
	return
		4 +											  // cell, particle
		(_gridCount.x * 2) * (_gridCount.y * 2);	  // velocity
}

UINT GridLiquid::iGetIndexBufferSize()
{
	return 6 +							  // cell, paricle
		(_gridCount.x * _gridCount.y * 2 + 1);  // velocity
}

XMINT3 GridLiquid::iGetObjectCount()
{
	return { _gridCount.x, _gridCount.y, 0 };
}

XMFLOAT3 GridLiquid::iGetObjectSize()
{
	return { 1.0f, 1.0f, 0.0f };
}

XMFLOAT3 GridLiquid::iGetObjectPositionOffset()
{
	return { 0.0f, 0.0f, 0.0f };
}

void GridLiquid::iCreateObject(vector<ConstantBuffer>& constantBuffer)
{
	// ###### Create Object ######
	for (int j = 0; j < _gridCount.y; j++)
	{
		for (int i = 0; i < _gridCount.x; i++)
		{
			// Position
			XMFLOAT2 pos = XMFLOAT2(
				(float)i,    
				(float)j);   

			_gridPosition.push_back(pos);

			struct ConstantBuffer objectCB;
			// Multiply by a specific value to make a stripe
			objectCB.world = DXViewer::util::transformMatrix(pos.x, pos.y, 0.0f, 1.0f);
			objectCB.worldViewProj = DXViewer::util::transformMatrix(0.0f, 0.0f, 0.0f);
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
				(float)i,   
				(float)j);  

			if (_gridState[_INDEX(i, j)] == STATE::LIQUID)
			{
				for (int k = 0; k < _particleCount; k++)
				{
					for (int m = 0; m < _particleCount; m++)
					{
						XMFLOAT2 gridVel = _gridVelocity[_INDEX(i, j)];

						_particleVelocity.push_back(gridVel);
						_particlePosition.push_back(
							{ -0.33f +  pos.x + k * _particleScale * 1.1f,   
							  -0.33f +  pos.y + m * _particleScale * 1.1f });

						struct ConstantBuffer particleCB;
						particleCB.world = DXViewer::util::transformMatrix(pos.x, pos.y, -1.0f, _particleScale);
						particleCB.worldViewProj = DXViewer::util::transformMatrix(0.0f, 0.0f, 0.0f);
						particleCB.color = XMFLOAT4(0.5f, 0.9f, 0.9f, 1.0f);

						constantBuffer.push_back(particleCB);
					}

				}

			}
		}
	}
	// ###### ###### ###### ######


	// ###### Create Velocity ######
	struct ConstantBuffer velocityCB;
	velocityCB.world = DXViewer::util::transformMatrix(0.0f, 0.0f, 0.0f, 1.0f);
	velocityCB.worldViewProj = DXViewer::util::transformMatrix(0.0f, 0.0f, 0.0f);
	velocityCB.color = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	constantBuffer.push_back(velocityCB);
	// ###### ###### ###### ######
}

void GridLiquid::iUpdateConstantBuffer(std::vector<ConstantBuffer>& constantBuffer, int i)
{
	int objectEndIndex = _gridCount.x * _gridCount.y;
	int size = constantBuffer.size();

	// Set object color					
	if (i < objectEndIndex)
	{
		constantBuffer[i].color = _getGridColor(i);
	}
	// Set particle position
	else if (i >= objectEndIndex && i < size - 1)
	{							// Due to velocity field
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
	int objectEndIndex = _gridCount.x * _gridCount.y;

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

UINT GridLiquid::iGetConstantBufferSize()
{
			
	return (_gridCount.x * _gridCount.y)									 // cell
		+ (_gridCount.x * _gridCount.y)										 // velocity
		+ (_gridCount.x * _gridCount.y) * _particleCount * _particleCount;   // particle
}
// #######################################################################################
#pragma endregion