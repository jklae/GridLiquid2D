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
	
	_grid[_INDEX(3, 2)] = _STATE::FLUID;
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

XMFLOAT4 EulerianSimulation::iGetColor(int index)
{
	switch (_grid[index])
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

float EulerianSimulation::iGetObjectScale()
{
	return _gridScale;
}

float EulerianSimulation::iGetObjectSize()
{
	return _gridSize;
}
// #######################################################################################
#pragma endregion