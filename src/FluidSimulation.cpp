#include "FluidSimulation.h"

using namespace DirectX;
using namespace std;

FluidSimulation::FluidSimulation(float timeStep)
{
	_timeStep = timeStep;
}

FluidSimulation::~FluidSimulation()
{
	
}

void FluidSimulation::setGridDomain(int xCount, int yCount)
{
	// 2 are boundaries.
	_gridCount = xCount + 2;
}

#pragma region Implementation
// ################################## Implementation ####################################
void FluidSimulation::iUpdate()
{
	float standardTimeStep = 0.1f;
	int maxSize = static_cast<int>(standardTimeStep / _timeStep);
	for (int i = 0; i < maxSize; i++)
	{
		_update();
	}

	//Sleep(10);
}

void FluidSimulation::iResetSimulationState(vector<ConstantBuffer>& constantBuffer)
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

vector<Vertex> FluidSimulation::iGetVertice()
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
			XMFLOAT2 v = { x.x + _gridVelocity[_INDEX(i, j)].x * 30.0f , x.y + _gridVelocity[_INDEX(i, j)].y * 30.0f };
			vertices.push_back(Vertex({ XMFLOAT3(x.x, x.y, -2.0f) }));
			vertices.push_back(Vertex({ XMFLOAT3(v.x, v.y, -2.0f) }));
		}
	}

	return vertices;
}

vector<unsigned int> FluidSimulation::iGetIndice()
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

XMFLOAT4 FluidSimulation::iGetColor(int i)
{
	float magnitude;
	switch (_gridState[i])
	{
	case STATE::FLUID:
		magnitude = sqrtf(powf(_gridVelocity[i].x, 2.0f) + powf(_gridVelocity[i].y, 2.0f));

		/*if (magnitude > 0.05f && magnitude < 0.2f)
			return XMFLOAT4(0.8f, 1.0f, 0.0f, 1.0f);
		else if (magnitude >= 0.2f)
			return XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
		else
			return XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);*/

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

int FluidSimulation::iGetObjectCount()
{
	return _gridCount;
}

XMFLOAT2 FluidSimulation::iGetParticlePos(int i)
{
	return _particlePosition[i];
}

void FluidSimulation::iCreateObjectParticle(vector<ConstantBuffer>& constantBuffer)
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

			if (_gridState[_INDEX(i, j)] == STATE::FLUID)
			{
				for (int k = 0; k < 4; k++)
				{
					for (int m = 0; m < 4; m++)
					{
						_particleVelocity.push_back(XMFLOAT2(0.0f, 0.0f));
						_particlePosition.push_back({ -0.3f + pos.x + k * _particleScale * 1.1f, -0.3f + pos.y + m * _particleScale * 1.1f });

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