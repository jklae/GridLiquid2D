#pragma once
#ifndef GRIDSIMHEADER_H
#define GRIDSIMHEADER_H

#include "dx12header.h"

enum class FLAG
{
	GRID,
	PARTICLE,
	VELOCITY
};

enum class STATE 
{ 
	FLUID, 
	BOUNDARY, 
	AIR, 
	SURFACE 
};

// These structures are shared by GridFluidSim and TimeIntegration.
struct GridData
{
	int gridCount = 0;
	int particleCount = 0;
	int operator()(int i, int j) { return (i + gridCount * j); };
};

struct Advect
{
	void operator()(DirectX::XMFLOAT2 data, std::vector<DirectX::XMFLOAT2>& temp, DirectX::XMFLOAT2 ratio, DirectX::XMINT2 minIndex, DirectX::XMINT2 maxIndex, GridData& _INDEX)
	{
		DirectX::XMFLOAT2 minMin_minMax = data * (1.0f - ratio.x);
		DirectX::XMFLOAT2 maxMin_maxMax = data * ratio.x;
		DirectX::XMFLOAT2 minMin = minMin_minMax * (1.0f - ratio.y);
		DirectX::XMFLOAT2 minMax = minMin_minMax * ratio.y;
		DirectX::XMFLOAT2 maxMin = maxMin_maxMax * (1.0f - ratio.y);
		DirectX::XMFLOAT2 maxMax = maxMin_maxMax * ratio.y;

		temp[_INDEX(minIndex.x, minIndex.y)] += minMin;
		temp[_INDEX(minIndex.x, maxIndex.y)] += minMax;
		temp[_INDEX(maxIndex.x, minIndex.y)] += maxMin;
		temp[_INDEX(maxIndex.x, maxIndex.y)] += maxMax;
	}
};

#endif