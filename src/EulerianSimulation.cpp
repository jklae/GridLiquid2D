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
				_grid.push_back(_STATE::BOUNDARY);
			else
				_grid.push_back(_STATE::AIR);
		}
	}
	
	_grid[_INDEX(1, 1)] = _STATE::FLUID;
	_grid[_INDEX(8, 6)] = _STATE::FLUID;

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


	_particle[0].x += 0.00005f;
	_particle[0].y += 0.00002f;

	_particle[1].x -= 0.00003f;
	_particle[1].y -= 0.00004f;

	_paintGrid();

}

void EulerianSimulation::_paintGrid()
{
	// Reset _grid
	for (int j = 1; j < _gridCount.y - 1; j++)
	{
		for (int i = 1; i < _gridCount.x - 1; i++)
		{

			_grid[_INDEX(i, j)] = _STATE::AIR;
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

	for (int i = 0; i < _particle.size(); i++)
	{
		XMFLOAT2 min = { particleOffset.x + (_particle[i].x / _gridScale) - particleStride,
					 particleOffset.y + (_particle[i].y / _gridScale) - particleStride };

		XMFLOAT2 max = { particleOffset.x + (_particle[i].x / _gridScale) + particleStride,
						 particleOffset.y + (_particle[i].y / _gridScale) + particleStride };

		XMINT2 minIndex = { static_cast<int>(floor(min.x)) , static_cast<int>(floor(min.y)) };
		XMINT2 maxIndex = { static_cast<int>(floor(max.x)) , static_cast<int>(floor(max.y)) };

		_grid[_INDEX(minIndex.x, minIndex.y)] = _STATE::FLUID;
		_grid[_INDEX(minIndex.x, maxIndex.y)] = _STATE::FLUID;
		_grid[_INDEX(maxIndex.x, minIndex.y)] = _STATE::FLUID;
		_grid[_INDEX(maxIndex.x, maxIndex.y)] = _STATE::FLUID;
	}
	

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
	switch (_grid[i])
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
	return _particle[i];
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

			struct ConstantBuffer objectCB;
			objectCB.world = transformMatrix(pos.x, pos.y, 0.0f, _gridScale*0.95f);
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

			if (_grid[_INDEX(i, j)] == _STATE::FLUID)
			{
				_particle.push_back(pos);

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