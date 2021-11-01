#pragma once
#include "gridsimheader.h"

class Interpolation
{
public:
	Interpolation(GridData& index);
	virtual ~Interpolation();

	virtual DirectX::XMFLOAT2 gridToParticle(DirectX::XMFLOAT2 particlePos, std::vector<DirectX::XMFLOAT2>& oldvel, std::vector<DirectX::XMFLOAT2>& gridPos) = 0;

	virtual void particleToGrid(DirectX::XMFLOAT2 particlePos, DirectX::XMFLOAT2 particleVel, std::vector<DirectX::XMFLOAT2>& gridPos, std::vector<STATE>& gridState) = 0;
	virtual void setGridVelocity(std::vector<DirectX::XMFLOAT2>& gridVel, std::vector<DirectX::XMFLOAT2>& oldVel, int i, int j) = 0;

protected:
	GridData& _INDEX;

	std::vector<DirectX::XMFLOAT2> _tempVel;
	std::vector<float> _pCount;
};

