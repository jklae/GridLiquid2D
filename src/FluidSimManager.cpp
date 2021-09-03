#include "FluidSimManager.h"

using Microsoft::WRL::ComPtr;
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

int FluidSimManager::iGetObjectCount()
{
	return _sim[_simIndex]->iGetObjectCount();
}

void FluidSimManager::iCreateObjectParticle(vector<ConstantBuffer>& constantBuffer)
{
	_sim[_simIndex]->iCreateObjectParticle(constantBuffer);
}

void FluidSimManager::iSubWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

}


void FluidSimManager::iUpdateConstantBuffer(vector<ConstantBuffer>& constantBuffer, int i)
{
	_sim[_simIndex]->iUpdateConstantBuffer(constantBuffer, i);
}

void FluidSimManager::iDraw(ComPtr<ID3D12GraphicsCommandList>& mCommandList, vector<ConstantBuffer>& constantBuffer, UINT indexCount, int i)
{
	_sim[_simIndex]->iDraw(mCommandList, constantBuffer, indexCount, _drawFlag, i);
}


int FluidSimManager::getI()
{
	return _simIndex;
}
void FluidSimManager::setI(int i)
{
	_simIndex = i;
}

void FluidSimManager::setDrawFlag(FLAG flagType, bool flag)
{
	int i = static_cast<int>(flagType);
	_drawFlag[i] = flag;
}

bool FluidSimManager::getDrawFlag(FLAG flagType)
{
	int i = static_cast<int>(flagType);
	return _drawFlag[i];
}
// #######################################################################################
#pragma endregion