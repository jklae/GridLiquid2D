#include "FluidSimManager.h"

using namespace DirectX;
using namespace std;



FluidSimManager::FluidSimManager()
{
	_sim.push_back(new EulerLiquidSim(0.01f, 0));
	_sim.push_back(new EulerGasSim(0.1f, 10));
	_sim.push_back(new PICLiquidSim(0.01f, 0));

	for (auto& sim : _sim)
	{
		dynamic_cast<GridFluidSim*>(sim)->setGridDomain(30, 20);
		dynamic_cast<GridFluidSim*>(sim)->initialize();
	}
}

FluidSimManager::~FluidSimManager()
{
	for (auto& sim : _sim)
	{
		delete sim;
	}

}



#pragma region Implementation
// ################################## Implementation ####################################
void FluidSimManager::iUpdate()
{
	GridFluidSim* sim = _sim[_simIndex];
	sim->iUpdate();
}

void FluidSimManager::iResetSimulationState(vector<ConstantBuffer>& constantBuffer)
{
	GridFluidSim* sim = _sim[_simIndex];
	sim->iResetSimulationState(constantBuffer);
}

vector<Vertex> FluidSimManager::iGetVertice()
{
	GridFluidSim* sim = _sim[_simIndex];
	return sim->iGetVertice();;
}

vector<unsigned int> FluidSimManager::iGetIndice()
{
	GridFluidSim* sim = _sim[_simIndex];
	return sim->iGetIndice();
}

XMFLOAT4 FluidSimManager::iGetColor(int i)
{
	GridFluidSim* sim = _sim[_simIndex];
	return sim->iGetColor(i);
}

int FluidSimManager::iGetObjectCount()
{
	GridFluidSim* sim = _sim[_simIndex];
	return sim->iGetObjectCount();
}

XMFLOAT2 FluidSimManager::iGetParticlePos(int i)
{
	GridFluidSim* sim = _sim[_simIndex];
	return sim->iGetParticlePos(i);
}

void FluidSimManager::iCreateObjectParticle(vector<ConstantBuffer>& constantBuffer)
{
	GridFluidSim* sim = _sim[_simIndex];
	sim->iCreateObjectParticle(constantBuffer);
}

void FluidSimManager::iSubWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

}
int FluidSimManager::getI()
{
	return _simIndex;
}
void FluidSimManager::setI(int i)
{
	_simIndex = i;
}
// #######################################################################################
#pragma endregion