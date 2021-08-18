#pragma once
#include <iostream>
#include "Win32App.h" // This includes ISimulation.h

class EulerianSimulation : public ISimulation
{
public:
	EulerianSimulation();
	~EulerianSimulation();

	void initialize();

	void setGridDomain(int xCount, int yCount);

#pragma region Implementation
	// ################################## Implementation ####################################
	void iUpdate(double timestep) override;

	std::vector<Vertex> iGetVertice() override;
	std::vector<unsigned int> iGetIndice() override;
	DirectX::XMFLOAT4 iGetColor(int i) override;
	std::vector<Vertex> iGetLineVertice() override;
	std::vector<unsigned int> iGetLineIndice() override;

	int iGetObjectCount() override;
	DirectX::XMFLOAT2 iGetParticlePos(int i) override;

	void iCreateObjectParticle(std::vector<ConstantBuffer>& constantBuffer) override;
	// #######################################################################################
#pragma endregion

private:
	inline int _INDEX(int i, int j) { return (i + _gridCount*j); };
	
	void _printVelocity();

	// Grid
	std::vector<STATE> _gridState;
	std::vector<DirectX::XMFLOAT2> _gridPosition;
	std::vector<DirectX::XMFLOAT2> _gridVelocity;
	std::vector<float> _gridPressure;
	std::vector<float> _gridDivergence;
	int _gridCount = 0;

	// Particle
	std::vector<DirectX::XMFLOAT2> _particlePosition;
	std::vector<DirectX::XMFLOAT2> _particleVelocity;
	float _particleScale = 0.2;

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

	int _computeFaceMinMaxIndex(VALUE vState, AXIS axis, DirectX::XMFLOAT2 particlePos);
	int _computeCenterMinMaxIndex(VALUE vState, AXIS axis, DirectX::XMFLOAT2 particlePos);
	DirectX::XMFLOAT2 _velocityInterpolation(DirectX::XMFLOAT2 pos, std::vector<DirectX::XMFLOAT2> oldvel);
	float _interpolation(float value1, float value2, float ratio);
};

