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
	assert((_gridCount[0] != 0) && (_gridCount[1] != 0));
	assert(_gridScale != 0.0f);

	// Set _fluid
	for (int j = 0; j < _gridCount[1]; j++)
	{
		for (int i = 0; i < _gridCount[0]; i++)
		{
			if (i == 0 || j == 0 
				|| i == _gridCount[0] - 1
				|| j == _gridCount[1] - 1)
				_grid.push_back(_STATE::BOUNDARY);
			else
				_grid.push_back(_STATE::AIR);
		}
	}
	
	_grid[_INDEX(9, 9)] = _STATE::FLUID;
	_grid[_INDEX(9, 10)] = _STATE::FLUID;
	_grid[_INDEX(10, 9)] = _STATE::FLUID;
	_grid[_INDEX(10, 10)] = _STATE::FLUID;

	_stride = (_gridSize * _gridScale) * 1.1f;
	_offset = XMFLOAT2(
		//		radius    *     count
		-((_stride / 2.0f) * static_cast<float>(_gridCount[0] - 1)),
		-((_stride / 2.0f) * static_cast<float>(_gridCount[1] - 1)));
}

void EulerianSimulation::setGridCountXY(int xCount, int yCount)
{
	// 2 are boundaries.
	_gridCount[0] = xCount + 2;
	_gridCount[1] = yCount + 2;
}

void EulerianSimulation::setGridScale(float gridScale)
{
	_gridScale = gridScale;
}

void EulerianSimulation::_update(double timestep)
{
	_particle[0].x += 0.001f;
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

int* EulerianSimulation::iGetObjectCountXY()
{
	return _gridCount;
}

XMFLOAT2 EulerianSimulation::iGetParticlePos(int i)
{
	return _particle[i];
}

void EulerianSimulation::iCreateObjectParticle(vector<ConstantBuffer>& constantBuffer)
{
	for (int j = 0; j < _gridCount[1]; j++)
	{
		for (int i = 0; i < _gridCount[0]; i++)
		{
			// Position
			XMFLOAT2 pos = XMFLOAT2(
				_offset.x + (float)i * _stride,
				_offset.y + (float)j * _stride);

			// ###### Create Object ######
			struct ConstantBuffer objectCB;
			objectCB.world = transformMatrix(pos.x, pos.y, 0.0f, _gridScale);
			objectCB.worldViewProj = transformMatrix(0.0f, 0.0f, 0.0f);
			objectCB.color = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

			constantBuffer.push_back(objectCB);
			// ###### ###### ###### ######
		}
	}

	for (int j = 0; j < _gridCount[1]; j++)
	{
		for (int i = 0; i < _gridCount[0]; i++)
		{
			// Position
			XMFLOAT2 pos = XMFLOAT2(
				_offset.x + (float)i * _stride,
				_offset.y + (float)j * _stride);

			// ###### Create particle ######
			if (_grid[_INDEX(i, j)] == _STATE::FLUID)
			{
				_particle.push_back(pos);

				struct ConstantBuffer particleCB;
				particleCB.world = transformMatrix(pos.x, pos.y, -_stride, _gridScale * _particleScale);
				particleCB.worldViewProj = transformMatrix(0.0f, 0.0f, 0.0f);
				particleCB.color = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);

				constantBuffer.push_back(particleCB);
			}
			// ###### ###### ###### ######
		}
	}
}

// #######################################################################################
#pragma endregion