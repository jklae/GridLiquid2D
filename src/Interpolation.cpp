#include "Interpolation.h"

Interpolation::Interpolation(GridData& index)
	:_INDEX(index)
{
	size_t vSize = static_cast<size_t>(index.gridCount) * static_cast<size_t>(index.gridCount);

	_tempVel.assign(vSize, { 0.0f, 0.0f });
	_pCount.assign(vSize, 0.0f);
}

Interpolation::~Interpolation()
{
}
