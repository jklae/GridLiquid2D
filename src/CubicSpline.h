#pragma once
#include "Interpolation.h"

class CubicSpline : public Interpolation
{
public:
	CubicSpline(GridData& index);
	~CubicSpline() override;

	DirectX::XMFLOAT2 gridToParticle(DirectX::XMFLOAT2 particlePos, std::vector<DirectX::XMFLOAT2>& oldvel, std::vector<DirectX::XMFLOAT2>& gridPos, std::vector<STATE>& gridState) override;

	void particleToGrid(DirectX::XMFLOAT2 particlePos, DirectX::XMFLOAT2 particleVel, std::vector<DirectX::XMFLOAT2>& gridPos, std::vector<STATE>& gridState) override;
	void setGridVelocity(std::vector<DirectX::XMFLOAT2>& gridVel, std::vector<DirectX::XMFLOAT2>& oldVel, int i, int j) override;
};

