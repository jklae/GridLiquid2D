#pragma once
#include "Win32App.h" // This includes ISimulation.h

class FluidSimulation : public ISimulation
{
public:
	FluidSimulation(float timeStep);
	~FluidSimulation() override;

	int getDelayTime();

	void setGridDomain(int xCount, int yCount);
	void setDelayTime(int delayTime);

	void initialize();

#pragma region Implementation
	// ################################## Implementation ####################################
	void iUpdate() override;
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

	float _timeStep = 0.0f;
	int _delayTime = 0;


	virtual void _update() = 0;

	virtual void _force() = 0;
	virtual void _advect() = 0;
	virtual void _diffuse() = 0;
	virtual void _project() = 0;

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

