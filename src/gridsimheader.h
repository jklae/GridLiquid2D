#pragma once
#ifndef GRIDSIMHEADER_H
#define GRIDSIMHEADER_H

#include "dx12header.h"

constexpr float FPS_30 = 0.033333f;
constexpr float FPS_60 = 0.016667f;
constexpr float FPS_90 = 0.011111f;
constexpr float FPS_120 = 0.008333f;

enum class FLAG { GRID, PARTICLE, VELOCITY };
enum class STATE { LIQUID, BOUNDARY, AIR, SURFACE };
enum class EX { DAM, DROP };
enum class VALUE { MIN, MAX };

#endif