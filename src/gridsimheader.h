#pragma once
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

// This structure is shared by GridFluidSim and TimeIntegration.
struct GridData
{
	int gridCount = 0;
	int particleCount = 0;
	int operator()(int i, int j) { return (i + gridCount * j); };
};