#pragma once
#include "dx12header.h"

enum class FLAG
{
	GRID,
	PARTICLE,
	VELOCITY
};

struct GridIndex
{
	int gridCount = 0;
	int operator()(int i, int j) { return (i + gridCount * j); };
};