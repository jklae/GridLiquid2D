#pragma once
#include "Interpolation.h"

class Trilinear : public Interpolation
{
public:
	Trilinear(GridData& index);
	~Trilinear() override;

	DirectX::XMFLOAT2 gridToParticle(DirectX::XMFLOAT2 particlePos, std::vector<DirectX::XMFLOAT2>& oldvel, std::vector<DirectX::XMFLOAT2>& gridPos) override;

private:
	DirectX::XMINT2 _computeCenterMinMaxIndex(_VALUE vState, DirectX::XMFLOAT2 particlePos);
	float _interpolation(float value1, float value2, float ratio);
};

