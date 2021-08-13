#pragma once
#include <iostream>
#include "Win32App.h" // This includes ISimulation.h

class EulerianSimulation : public ISimulation
{
public:
	EulerianSimulation();
	~EulerianSimulation();

	void initialize();

#pragma region Implementation
	// ################################## Implementation ####################################
	void iUpdate(double timestep) override;

	std::vector<Vertex> iGetVertice() override;
	std::vector<unsigned int> iGetIndice() override;
	std::vector<DirectX::XMFLOAT4> iGetColor() override;

	int* iGetObjectCountXYZ() override;
	float iGetObjectScale() override;
	float iGetObjectSize() override;

	void iSetObjectCountXYZ(int xCount, int yCount, int zCount) override;
	void iSetObjectScale(float objectScale) override;
	// #######################################################################################
#pragma endregion

private:
	int _objectCount[3] = { 0, 0, 0 };
	float _objectScale = 0.0f;
	float _objectSize = 2.0f;

	std::vector<Vertex> _vertices =
	{
		Vertex({ DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f) }),
		Vertex({ DirectX::XMFLOAT3(-1.0f, +1.0f, -1.0f) }),
		Vertex({ DirectX::XMFLOAT3(+1.0f, +1.0f, -1.0f) }),
		Vertex({ DirectX::XMFLOAT3(+1.0f, -1.0f, -1.0f) }),
		Vertex({ DirectX::XMFLOAT3(-1.0f, -1.0f, +1.0f) }),
		Vertex({ DirectX::XMFLOAT3(-1.0f, +1.0f, +1.0f) }),
		Vertex({ DirectX::XMFLOAT3(+1.0f, +1.0f, +1.0f) }),
		Vertex({ DirectX::XMFLOAT3(+1.0f, -1.0f, +1.0f) })
	};

	std::vector<unsigned int> _indices =
	{
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};
};

