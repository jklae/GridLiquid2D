#pragma once
#include <iostream>
#include "Win32App.h" // This includes ISimulation.h

class EulerianSimulation : public ISimulation
{
public:
	EulerianSimulation();
	~EulerianSimulation();

	void initialize();

	void setGridCountXY(int xCount, int yCount);
	void setGridScale(float gridScale);

#pragma region Implementation
	// ################################## Implementation ####################################
	void iUpdate(double timestep) override;

	std::vector<Vertex> iGetVertice() override;
	std::vector<unsigned int> iGetIndice() override;
	DirectX::XMFLOAT4 iGetColor(int i) override;

	int* iGetObjectCountXY() override;
	DirectX::XMFLOAT2 iGetParticlePos(int i) override;

	void iCreateObjectParticle(std::vector<ConstantBuffer>& constantBuffer) override;
	// #######################################################################################
#pragma endregion

private:
	inline int _INDEX(int i, int j) { return (_gridCount[1]*j + i); };

	enum class _STATE {FLUID = 0, BOUNDARY, AIR};

	std::vector<_STATE> _grid;
	std::vector<DirectX::XMFLOAT2> _velocity;
	int _gridCount[2] = { 0, 0 };
	float _gridScale = 0.0f;
	float _gridSize = 2.0f;

	std::vector<DirectX::XMFLOAT2> _particle;
	float _particleScale = 0.3f;


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

	void _update(double timestep);
};

