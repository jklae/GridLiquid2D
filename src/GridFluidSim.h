#pragma once
#include "dx12header.h"

class GridFluidSim
{
public:
	GridFluidSim(float timeStep, int delayTime);
	virtual ~GridFluidSim();

	void setGridDomain(int xCount, int yCount);
	void initialize();

	virtual void update() = 0;

	void iUpdate();
	void iResetSimulationState(std::vector<ConstantBuffer>& constantBuffer);

	std::vector<Vertex> iGetVertice();
	std::vector<unsigned int> iGetIndice();
	DirectX::XMFLOAT4 iGetColor(int i);


	int iGetObjectCount();

	void iUpdateConstantBuffer(std::vector<ConstantBuffer>& constantBuffer, int i);

	void iCreateObjectParticle(std::vector<ConstantBuffer>& constantBuffer);

protected:
	enum class _STATE { FLUID, BOUNDARY, AIR };
	enum class _VALUE { MIN, MAX };
	enum class _AXIS { X, Y };

	inline int _INDEX(int i, int j) { return (i + _gridCount * j); };

	// Grid
	std::vector<_STATE> _gridState;
	std::vector<DirectX::XMFLOAT2> _gridPosition;
	std::vector<DirectX::XMFLOAT2> _gridVelocity;
	std::vector<float> _gridPressure;
	std::vector<float> _gridDivergence;
	int _gridCount = 0;

	// Particle
	std::vector<DirectX::XMFLOAT2> _particlePosition;
	std::vector<DirectX::XMFLOAT2> _particleVelocity;
	float _particleScale = 0.2;
	int _particleCount = 4;

	float _timeStep = 0.0f;
	int _delayTime = 0;
	

	virtual void _force() = 0;
	virtual void _advect() = 0;
	virtual void _diffuse() = 0;
	virtual void _project() = 0;

	void _setBoundary(std::vector<DirectX::XMFLOAT2>& vec);
	void _setBoundary(std::vector<float>& scalar);
	void _paintGrid();
	void _updateParticlePosition();

	// ---

	int _computeFaceMinMaxIndex(_VALUE vState, _AXIS axis, DirectX::XMFLOAT2 particlePos);
	int _computeCenterMinMaxIndex(_VALUE vState, _AXIS axis, DirectX::XMFLOAT2 particlePos);
	DirectX::XMFLOAT2 _velocityInterpolation(DirectX::XMFLOAT2 pos, std::vector<DirectX::XMFLOAT2> oldvel);
	float _interpolation(float value1, float value2, float ratio);
};

