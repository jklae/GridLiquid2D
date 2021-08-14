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
	assert((_objectCount[0] != 0) && (_objectCount[1] != 0));
	assert(_objectScale != 0.0f);

	// Set _fluid
	for (int j = 0; j < _objectCount[1]; j++)
	{
		for (int i = 0; i < _objectCount[0]; i++)
		{
			if (i == 0 || j == 0 || i == _objectCount[0]-1 || j == _objectCount[1] - 1)
				_fluid.push_back(_STATE::BOUNDARY);
			else
				_fluid.push_back(_STATE::AIR);
		}
	}
	
	_fluid[_INDEX(3, 2)] = _STATE::FLUID;
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
	switch (_fluid[index])
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

int* EulerianSimulation::iGetObjectCountXYZ()
{
	return _objectCount;
}

float EulerianSimulation::iGetObjectScale()
{
	return _objectScale;
}

float EulerianSimulation::iGetObjectSize()
{
	return _objectSize;
}

void EulerianSimulation::iSetObjectCountXY(int xCount, int yCount)
{
	// 2 are boundaries.
	_objectCount[0] = xCount + 2;
	_objectCount[1] = yCount + 2;
}

void EulerianSimulation::iSetObjectScale(float objectScale)
{
	_objectScale = objectScale;
}
// #######################################################################################
#pragma endregion