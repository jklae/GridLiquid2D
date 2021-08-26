#pragma once
#include "Win32App.h" // This includes ISimulation.h

class FluidSimulation : public ISimulation
{
public:
	FluidSimulation();
	~FluidSimulation();

	virtual void initialize() = 0;

#pragma region Implementation
	// ################################## Implementation ####################################
	void iUpdate(double timestep) override;
	void iResetSimulationState(std::vector<ConstantBuffer>& constantBuffer) override;

	std::vector<Vertex> iGetVertice() override;
	std::vector<unsigned int> iGetIndice() override;
	DirectX::XMFLOAT4 iGetColor(int i) override;


	int iGetObjectCount() override;
	DirectX::XMFLOAT2 iGetParticlePos(int i) override;

	void iCreateObjectParticle(std::vector<ConstantBuffer>& constantBuffer) override;
	// #######################################################################################
#pragma endregion

protected:
	inline int _INDEX(int i, int j) { return (i + _gridCount * j); };

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

	virtual void _update(double timestep) = 0;
};

