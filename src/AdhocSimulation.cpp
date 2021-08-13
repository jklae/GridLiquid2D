#include "AdhocSimulation.h"

using namespace DirectX;
using namespace std;

AdhocSimulation::AdhocSimulation()
{
}

AdhocSimulation::~AdhocSimulation()
{
}

void AdhocSimulation::iUpdate(double timestep)
{
	//cout << "timestep : " << timestep << endl;
}

vector<Vertex> AdhocSimulation::iGetVertice()
{
	return _vertices;
}

vector<unsigned int> AdhocSimulation::iGetIndice()
{
	return _indices;
}

vector<XMFLOAT4> AdhocSimulation::iGetColor()
{
	vector<XMFLOAT4> color;

	for (int k = 0; k < _objectCount[2]; k++)
	{
		for (int j = 0; j < _objectCount[1]; j++)
		{
			for (int i = 0; i < _objectCount[0]; i++)
			{
				if (i == 1 && j == 0 && k == 0)
					color.push_back(XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
				else
					color.push_back(XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
			}
		}
	}

	return color;
}

int* AdhocSimulation::iGetObjectCountXYZ()
{
	return _objectCount;
}

float AdhocSimulation::iGetObjectScale()
{
	return _objectScale;
}

float AdhocSimulation::iGetObjectSize()
{
	return _objectSize;
}

void AdhocSimulation::iSetObjectCountXYZ(int xCount, int yCount, int zCount)
{
	// 0 is not allowed.
	assert((xCount != 0) && (yCount != 0) && (zCount != 0));

	_objectCount[0] = xCount;
	_objectCount[1] = yCount;
	_objectCount[2] = zCount;
}

void AdhocSimulation::iSetObjectScale(float objectScale)
{
	// 0 is not allowed.
	assert(objectScale != 0.0f);

	_objectScale = objectScale;
}
