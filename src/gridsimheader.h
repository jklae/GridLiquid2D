#pragma once
#ifndef GRIDSIMHEADER_H
#define GRIDSIMHEADER_H

#include "dx12header.h"

constexpr float FPS_60 = 0.0167f;

enum class FLAG { GRID, PARTICLE, VELOCITY };
enum class STATE { FLUID, BOUNDARY, AIR, SURFACE };
enum class EX { DAM, DROP1, DROP2 };

// These structures are shared by GridFluidSim and TimeIntegration.
struct GridData
{
	int gridCount = 0;
	int particleCount = 0;
	int operator()(int i, int j) { return (i + gridCount * j); };
};

#endif