#pragma once
#include "gridsimheader.h"

class Interpolation
{
public:
	Interpolation(GridData& index);
	virtual ~Interpolation();

	//virtual DirectX::XMFLOAT2 particleToGrid(DirectX::XMFLOAT2 particlePos, std::vector<DirectX::XMFLOAT2>& oldvel, std::vector<DirectX::XMFLOAT2>& gridPos) = 0;
	virtual DirectX::XMFLOAT2 gridToParticle(DirectX::XMFLOAT2 particlePos, std::vector<DirectX::XMFLOAT2>& oldvel, std::vector<DirectX::XMFLOAT2>& gridPos) = 0;

protected:
	GridData& _INDEX;
};

