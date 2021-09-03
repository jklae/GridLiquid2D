#include "FluidSimManager.h"

using namespace DirectX;
using namespace std;



FluidSimManager::FluidSimManager(vector<GridFluidSim*>& sim)
{
	_sim = sim;
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
	_sim[_simIndex]->iUpdate();
}

void FluidSimManager::iResetSimulationState(vector<ConstantBuffer>& constantBuffer)
{
	_sim[_simIndex]->iResetSimulationState(constantBuffer);
}

vector<Vertex> FluidSimManager::iGetVertice()
{
	return _sim[_simIndex]->iGetVertice();;
}

vector<unsigned int> FluidSimManager::iGetIndice()
{
	return _sim[_simIndex]->iGetIndice();
}

XMFLOAT4 FluidSimManager::iGetColor(int i)
{
	return _sim[_simIndex]->iGetColor(i);
}

int FluidSimManager::iGetObjectCount()
{
	return _sim[_simIndex]->iGetObjectCount();
}

XMFLOAT2 FluidSimManager::iGetParticlePos(int i)
{
	return _sim[_simIndex]->iGetParticlePos(i);
}

void FluidSimManager::iCreateObjectParticle(vector<ConstantBuffer>& constantBuffer)
{
	_sim[_simIndex]->iCreateObjectParticle(constantBuffer);
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