#pragma once
#ifndef GRIDSIMHEADER_H
#define GRIDSIMHEADER_H

#include "dx12header.h"

constexpr float FPS_60 = 0.0167f;

enum class FLAG { GRID, PARTICLE, VELOCITY };
enum class STATE { FLUID, BOUNDARY, AIR, SURFACE };
enum class EX { DROP1, DROP2, DAM };

// These structures are shared by GridFluidSim and TimeIntegration.
struct GridData
{
	int gridCount = 0;
	int particleCount = 0;
	int operator()(int i, int j) { return (i + gridCount * j); };
};

struct ReverseInterpolation
{
	template <typename T>
	void operator()(T data, std::vector<T>& temp, DirectX::XMFLOAT2 ratio, DirectX::XMINT2 minIndex, DirectX::XMINT2 maxIndex, GridData& _INDEX)
	{
		T minMin_minMax = data * (1.0f - ratio.x);
		T maxMin_maxMax = data * ratio.x;
		T minMin = minMin_minMax * (1.0f - ratio.y);
		T minMax = minMin_minMax * ratio.y;
		T maxMin = maxMin_maxMax * (1.0f - ratio.y);
		T maxMax = maxMin_maxMax * ratio.y;

		temp[_INDEX(minIndex.x, minIndex.y)] += minMin;
		temp[_INDEX(minIndex.x, maxIndex.y)] += minMax;
		temp[_INDEX(maxIndex.x, minIndex.y)] += maxMin;
		temp[_INDEX(maxIndex.x, maxIndex.y)] += maxMax;
	}
};

#endif