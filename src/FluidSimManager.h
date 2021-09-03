#pragma once
#include "Win32App.h" // This includes ISimulation.h.
					  // Win32App is required in main().

#include "GridFluidSim.h"
#include "EulerGasSim.h" 
#include "EulerLiquidSim.h"
#include "PICLiquidSim.h"

class FluidSimManager : public ISimulation
{
public:
	FluidSimManager(std::vector<GridFluidSim*>& sim);
	~FluidSimManager() override;

	int _simIndex = 2;

#pragma region Implementation
	// ################################## Implementation ####################################
	void iUpdate() override;
	void iResetSimulationState(std::vector<ConstantBuffer>& constantBuffer) override;

	std::vector<Vertex> iGetVertice() override;
	std::vector<unsigned int> iGetIndice() override;
	DirectX::XMFLOAT4 iGetColor(int i) override;


	int iGetObjectCount() override;
	DirectX::XMFLOAT2 iGetParticlePos(int i) override;

	void iCreateObjectParticle(std::vector<ConstantBuffer>& constantBuffer) override;
	void iSubWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

	int getI() override;
	void setI(int i) override;
	// #######################################################################################
#pragma endregion


private:
	std::vector<GridFluidSim*> _sim;


};

