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

	DirectX::XMINT2 iGetObjectCountXY() override;
	DirectX::XMFLOAT2 iGetParticlePos(int i) override;

	void iCreateObjectParticle(std::vector<ConstantBuffer>& constantBuffer) override;
	// #######################################################################################
#pragma endregion

private:
	inline int _INDEX(int j, int i) { return (_gridCount.x*i + j); };

	enum class _STATE {FLUID = 0, BOUNDARY, AIR};

	// Grid
	std::vector<_STATE> _grid;
	std::vector<DirectX::XMFLOAT2> _velocity;
	DirectX::XMINT2 _gridCount = { 0, 0 };
	float _gridScale = 0.0f;
	float _gridSize = 1.0f;

	// Particle
	std::vector<DirectX::XMFLOAT2> _particle;
	float _particleScale = 0.5f;

	// CreateObjectParticle
	float _stride = 0.0f;
	DirectX::XMFLOAT2 _offset;

	std::vector<Vertex> _vertices =
	{
		Vertex({ DirectX::XMFLOAT3(-0.5f, -0.5f, -0.0f) }),
		Vertex({ DirectX::XMFLOAT3(-0.5f, +0.5f, -0.0f) }),
		Vertex({ DirectX::XMFLOAT3(+0.5f, +0.5f, -0.0f) }),
		Vertex({ DirectX::XMFLOAT3(+0.5f, -0.5f, -0.0f) })
	};

	std::vector<unsigned int> _indices =
	{
		// front face
		0, 1, 2,
		0, 2, 3,
	};

	void _update(double timestep);
	void _checkGrid();
};

