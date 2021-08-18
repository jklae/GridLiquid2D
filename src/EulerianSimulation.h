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

	enum class _STATE {FLUID, BOUNDARY, AIR};
	enum class _VALUE { MIN, MAX };
	enum class _AXIS { X, Y };

	// Grid
	std::vector<_STATE> _gridState;
	std::vector<DirectX::XMFLOAT2> _gridPosition;
	std::vector<DirectX::XMFLOAT2> _gridVelocity;
	std::vector<float> _gridPressure;
	std::vector<float> _gridDivergence;
	DirectX::XMINT2 _gridCount = { 0, 0 };
	float _gridScale = 0.0f;
	float _gridSize = 1.0f;
	float _gridStride;
	DirectX::XMFLOAT2 _gridOffset;

	// Particle
	std::vector<DirectX::XMFLOAT2> _particlePosition;
	std::vector<DirectX::XMFLOAT2> _particleVelocity;
	float _particleScale = 0.2;
	float _particleStride;
	DirectX::XMFLOAT2 _particleFaceOffset;
	DirectX::XMFLOAT2 _particleCenterOffset;

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

	void _force(double timestep);
	void _advect(double timestep);
	void _diffuse(double timestep);
	void _project(double timestep);

	void _setBoundary(std::vector<DirectX::XMFLOAT2>& vec);
	void _setBoundary(std::vector<float>& scalar);
	void _paintGrid();
	void _updateParticlePosition();

	// ---

	int _computeFaceMinMaxIndex(_VALUE vState, _AXIS axis, DirectX::XMFLOAT2 particlePos);
	int _computeCenterMinMaxIndex(_VALUE vState, _AXIS axis, DirectX::XMFLOAT2 particlePos);
	DirectX::XMFLOAT2 _velocityInterpolation(DirectX::XMFLOAT2 pos);
	float _interpolation(float value1, float value2, float ratio);
};

