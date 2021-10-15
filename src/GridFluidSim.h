#pragma once
#include "gridsimheader.h"
#include "TimeIntegration.h"

class GridFluidSim
{
public:
	GridFluidSim(GridData& index);
	virtual ~GridFluidSim();

	void setTimeInteg(TimeIntegration* timeInteg);

#pragma region Implementation
	// ################################## Implementation ####################################
	void iUpdate();
	void iResetSimulationState(std::vector<ConstantBuffer>& constantBuffer);

	std::vector<Vertex> iGetVertice();
	std::vector<unsigned int> iGetIndice();

	int iGetObjectCount();

	void iUpdateConstantBuffer(std::vector<ConstantBuffer>& constantBuffer, int i);
	void iDraw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& mCommandList, int size, UINT indexCount, bool* drawFlag, int i);

	void iCreateObjectParticle(std::vector<ConstantBuffer>& constantBuffer);
	// #######################################################################################

protected:
	enum class _VALUE { MIN, MAX };

	GridData& _INDEX;

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
	float _particleScale = 0.2f;
	int _particleCount = 0;

	TimeIntegration* _timeInteg = nullptr;

	DirectX::XMFLOAT4 _getColor(int i);

	void _setBoundary(std::vector<DirectX::XMFLOAT2>& vec);
	void _setBoundary(std::vector<float>& scalar);
	void _setFreeSurface(std::vector<DirectX::XMFLOAT2>& vec);

	virtual void _updateParticlePos() = 0;
	void _paintGrid();

	virtual void _update() = 0;

	virtual void _initialize();

	// ---

	DirectX::XMINT2 _computeFaceMinMaxIndex(_VALUE vState, DirectX::XMFLOAT2 particlePos);
	DirectX::XMINT2 _computeCenterMinMaxIndex(_VALUE vState, DirectX::XMFLOAT2 particlePos);
	DirectX::XMFLOAT2 _velocityInterpolation(DirectX::XMFLOAT2 pos, const std::vector<DirectX::XMFLOAT2>& oldvel);
	float _interpolation(float value1, float value2, float ratio);
};

