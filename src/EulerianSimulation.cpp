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
			_fluid.push_back(false);
		}
	}
	
	_fluid[INDEX(3, 2)] = true;
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
	if (_fluid[index])
		return XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	else
		return XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
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
	_objectCount[0] = xCount;
	_objectCount[1] = yCount;
}

void EulerianSimulation::iSetObjectScale(float objectScale)
{
	_objectScale = objectScale;
}
// #######################################################################################
#pragma endregion