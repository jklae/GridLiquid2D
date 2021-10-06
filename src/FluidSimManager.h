#pragma once
#include <ctime>

#include "Win32App.h" // This includes ISimulation.h.
					  // Win32App is required in main().

#include "GridFluidSim.h"
#include "EulerGasSim.h" 
#include "EulerLiquidSim.h"
#include "PICFLIPSim.h"

class FluidSimManager : public ISimulation
{
public:
	FluidSimManager(std::vector<GridFluidSim*>& sim);
	~FluidSimManager() override;


#pragma region Implementation
	// ################################## Implementation ####################################
	void iUpdate() override;
	void iResetSimulationState(std::vector<ConstantBuffer>& constantBuffer) override;

	std::vector<Vertex> iGetVertice() override;
	std::vector<unsigned int> iGetIndice() override;
	int iGetObjectCount() override;

	void iCreateObjectParticle(std::vector<ConstantBuffer>& constantBuffer) override;
	void iWMCreate(HWND hwnd, HINSTANCE hInstance) override;
	void iWMCommand(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, HINSTANCE hInstance, bool& updateFlag, DX12App* dxapp) override;
	void iWMHScroll(HWND hwnd, WPARAM wParam, LPARAM lParam, HINSTANCE hInstance) override;

	void iUpdateConstantBuffer(std::vector<ConstantBuffer>& constantBuffer, int i) override;
	void iDraw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& mCommandList, int size, UINT indexCount, int i) override;
	// #######################################################################################
#pragma endregion


private:
	std::vector<GridFluidSim*> _sim;
	int _simIndex = 0;
	int _scrollPos = 99;

	enum class _COM
	{
		GRID_BTN, PARTICLE_BTN, VELOCITY_BTN,
		PLAY, STOP, NEXTSTEP,
		STATE_GROUP, LIQUID_RADIO, GAS_RADIO,
		SOLVER_GROUP, EULERIAN_RADIO, PICFLIP_RADIO,
		RATIO_BAR, PIC_TEXT, PIC_RATIO, FLIP_TEXT, FLIP_RATIO
	};

	bool _drawFlag[3] = { true, true, false };

	void _setDrawFlag(FLAG flagType, bool flag);
	bool _getDrawFlag(FLAG flagType);

	wchar_t* int2wchar(int value);
};

