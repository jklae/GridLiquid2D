#pragma once
#ifndef GRIDSIMHEADER_H
#define GRIDSIMHEADER_H

#include "dx12header.h"

enum class FLAG { GRID, PARTICLE, VELOCITY };
enum class STATE { LIQUID, BOUNDARY, AIR, SURFACE };
enum class EX { DAM, DROP };
enum class VALUE { MIN, MAX };

#endif