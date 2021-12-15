#pragma once
#include <fstream>
#include <sstream>
#include <algorithm>

#include "gridsimheader.h"

class GridLiquid
{
public:
	GridLiquid(int x, float timeStep);
	virtual ~GridLiquid();

#pragma region Implementation
	// ################################## Implementation ####################################
	void iUpdate();
	void iResetSimulationState(std::vector<ConstantBuffer>& constantBuffer, EX ex);

	std::vector<Vertex>& iGetVertice();
	std::vector<unsigned int>& iGetIndice();

	int iGetObjectCount();

	void iUpdateConstantBuffer(std::vector<ConstantBuffer>& constantBuffer, int i);
	void iDraw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& mCommandList, int size, UINT indexCount, bool* drawFlag, int i);

	void iCreateObjectParticle(std::vector<ConstantBuffer>& constantBuffer);
	// #######################################################################################

protected:
	std::vector<Vertex> _vertices; 
	std::vector<unsigned int> _indices;


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
	float _particleScale = 0.3f;
	int _particleCount = 0;

	float _timeStep = 0.0f;

	DirectX::XMFLOAT4 _getGridColor(int i);

	void _setBoundary(std::vector<DirectX::XMFLOAT2>& vec);
	void _setBoundary(std::vector<float>& scalar);
	void _setFreeSurface(std::vector<DirectX::XMFLOAT2>& vec);

	void _paintLiquid();
	void _paintSurface();

	virtual void _update() = 0;

	virtual void _initialize(EX ex);
	void _computeGridState(EX ex, int i, int j);

	// ---

	DirectX::XMINT2 _computeFaceMinMaxIndex(VALUE vState, DirectX::XMFLOAT2 particlePos);
	DirectX::XMINT2 _computeCenterMinMaxIndex(VALUE vState, DirectX::XMFLOAT2 particlePos);

	DirectX::XMFLOAT2 gridToParticle(DirectX::XMFLOAT2 particlePos, std::vector<DirectX::XMFLOAT2>& oldvel);
};

