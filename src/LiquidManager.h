#pragma once
#include <ctime>

#include "Win32App.h" // This includes ISimulation.h.
					  // Win32App is required in main().

// These include GridLiquid.h.
#include "Eulerian.h"
#include "PICFLIP.h"

class LiquidManager : public ISimulation
{
public:
	LiquidManager(int x, int y, float timeStep);
	~LiquidManager() override;


#pragma region Implementation
	// ################################## Implementation ####################################
	void iUpdate() override;
	void iResetSimulationState(std::vector<ConstantBuffer>& constantBuffer) override;

	std::vector<Vertex>& iGetVertice() override;
	std::vector<unsigned int>& iGetIndice() override;
	int iGetObjectCount() override;

	void iCreateObjectParticle(std::vector<ConstantBuffer>& constantBuffer) override;
	void iUpdateConstantBuffer(std::vector<ConstantBuffer>& constantBuffer, int i) override;
	void iDraw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& mCommandList, int size, UINT indexCount, int i) override;
	void iSetDXApp(DX12App* dxApp) override;

	void iWMCreate(HWND hwnd, HINSTANCE hInstance) override;
	void iWMCommand(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, HINSTANCE hInstance) override;
	void iWMHScroll(HWND hwnd, WPARAM wParam, LPARAM lParam, HINSTANCE hInstance) override;
	void iWMTimer(HWND hwnd) override;
	void iWMDestory(HWND hwnd) override;

	bool iGetUpdateFlag() override;
	// #######################################################################################
#pragma endregion


private:
	std::vector<GridLiquid*> _sim;

	// 0 is Eulerian, 1 is PIC/FLIP
	int _simIndex = 1;
	int _scrollPos = 99;

	DX12App* _dxapp;

	enum class _COM
	{
		GRID_BTN, PARTICLE_BTN, VELOCITY_BTN,
		PLAY, STOP, NEXTSTEP,
		EX_GROUP, DAM_RADIO, DROP_RADIO,
		SOLVER_GROUP, EULERIAN_RADIO, PICFLIP_RADIO,
		RATIO_BAR, PIC_TEXT, PIC_RATIO, FLIP_TEXT, FLIP_RATIO,
		TIME_TEXT, FRAME_TEXT
	};

					// grid, particle, velocity
	bool _drawFlag[3] = { true, true, false };
	bool _updateFlag = true;

	wchar_t wBuffer[5];
	wchar_t* _int2wchar(int value);

	clock_t _simTime = 0;
	int _simFrame = 0;

	EX _ex = EX::DAM;

	void _setDrawFlag(FLAG flagType, bool flag);
	bool _getDrawFlag(FLAG flagType);

	void _resetSim();
};

