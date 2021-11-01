#include "CubicSpline.h"

CubicSpline::CubicSpline(GridData& index)
	:Interpolation(index)
{
}

CubicSpline::~CubicSpline()
{
}

DirectX::XMFLOAT2 CubicSpline::gridToParticle(DirectX::XMFLOAT2 particlePos, std::vector<DirectX::XMFLOAT2>& oldvel, std::vector<DirectX::XMFLOAT2>& gridPos, std::vector<STATE>& gridState)
{
	return DirectX::XMFLOAT2();
}

void CubicSpline::particleToGrid(DirectX::XMFLOAT2 particlePos, DirectX::XMFLOAT2 particleVel, std::vector<DirectX::XMFLOAT2>& gridPos, std::vector<STATE>& gridState)
{
}

void CubicSpline::setGridVelocity(std::vector<DirectX::XMFLOAT2>& gridVel, std::vector<DirectX::XMFLOAT2>& oldVel, int i, int j)
{
}
