#include "KoikeIntegration.h"

KoikeIntegration::KoikeIntegration(float timeStep, GridData& index)
	:TimeIntegration(timeStep, index)
{
}

KoikeIntegration::~KoikeIntegration()
{
}

int KoikeIntegration::getGroup()
{
	return 1;
}

float KoikeIntegration::computeGridTimeStep(DirectX::XMFLOAT2 vel, int i, int j)
{
	return 0.0f;
}

float KoikeIntegration::computeParticleTimeStep(DirectX::XMFLOAT2 vel, int i)
{
	return 0.0f;
}

void KoikeIntegration::computeGlobalTimeStep(std::vector<DirectX::XMFLOAT2>& vel, std::vector<STATE>& state)
{
}

void KoikeIntegration::computeAdvectTimeStep(std::vector<float>& pCount, int i, int j)
{
}

void KoikeIntegration::reInterpTimeStep(DirectX::XMFLOAT2 ratio, DirectX::XMINT2 minIndex, DirectX::XMINT2 maxIndex, int i)
{
}
