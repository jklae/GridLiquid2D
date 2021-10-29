#pragma once
#include <ctime>

#include "Win32App.h" // This includes ISimulation.h.
					  // Win32App is required in main().

// These include GridFluidSim.h.
#include "EulerLiquidSim.h"
#include "PICFLIPSim.h"

// These include TimeIntegration.h.
#include "FixedIntegration.h"
#include "GlobalIntegration.h"
#include "KoikeIntegration.h"
#include "OursIntegration.h"

class FluidSimManager : public ISimulation
{
public:
	FluidSimManager(int x, int y, float timeStep);
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
	void iWMHScroll(HWND hwnd, WPARAM wParam, LPARAM lParam, HINSTANCE hInstance, DX12App* dxapp) override;
	void iWMTimer(HWND hwnd) override;
	void iWMDestory(HWND hwnd) override;

	void iUpdateConstantBuffer(std::vector<ConstantBuffer>& constantBuffer, int i) override;
	void iDraw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& mCommandList, int size, UINT indexCount, int i) override;
	// #######################################################################################
#pragma endregion


private:
	std::vector<GridFluidSim*> _sim;
	std::vector<TimeIntegration*> _timeInteg;
	int _simIndex = 1;
	int _scrollPos = 99;

	enum class _COM
	{
		GRID_BTN, PARTICLE_BTN, VELOCITY_BTN,
		PLAY, STOP, NEXTSTEP, RECORD,
		EX_GROUP, DAM_RADIO, DROP1_RADIO, DROP2_RADIO,
		SOLVER_GROUP, EULERIAN_RADIO, PICFLIP_RADIO,
		RATIO_BAR, PIC_TEXT, PIC_RATIO, FLIP_TEXT, FLIP_RATIO,
		INTEG_GROUP, FIXED_RADIO, GLOBAL_RADIO, REINHARDT_RADIO, KOIKE_RADIO, OURS_RADIO,
		TIME_TEXT, FRAME_TEXT
	};

	bool _drawFlag[3] = { true, true, false };

	wchar_t wBuffer[5];
	wchar_t* _int2wchar(int value);

	clock_t _simTime = 0;
	int _simFrame = 0;

	GridData _index;
	EX _ex = EX::DROP2;

	void _setDrawFlag(FLAG flagType, bool flag);
	bool _getDrawFlag(FLAG flagType);

	void _setSimTimeInteg(int timeIntegIndex);

	void _resetSim(DX12App* dxapp);
};

