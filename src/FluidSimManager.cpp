#include "FluidSimManager.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace std;


FluidSimManager::FluidSimManager(int x, int y, float timeStep)
{
	// 2 are boundaries.
	_index.gridCount = x + 2;
	_index.particleCount = 4;

	_sim.push_back(new EulerLiquidSim(_index));
	_sim.push_back(new EulerGasSim(_index));
	_sim.push_back(new PICFLIPSim(_index));

	_timeInteg.push_back(new FixedIntegration(0.01f, _index));
	_timeInteg.push_back(new GlobalIntegration(0.02f, _index));
	_timeInteg.push_back(new KoikeIntegration(0.01f, _index));
	_timeInteg.push_back(new OursIntegration(0.02f, _index));

	_setSimTimeInteg(0);
}

FluidSimManager::~FluidSimManager()
{
	for (auto& sim : _sim)
	{
		delete sim;
	}

	for (auto& tInteg : _timeInteg)
	{
		delete tInteg;
	}

}

wchar_t* FluidSimManager::_int2wchar(int value)
{
	_itow(value, wBuffer, 10);
	return wBuffer;
}

void FluidSimManager::_setDrawFlag(FLAG flagType, bool flag)
{
	int i = static_cast<int>(flagType);
	_drawFlag[i] = flag;
}

bool FluidSimManager::_getDrawFlag(FLAG flagType)
{
	int i = static_cast<int>(flagType);
	return _drawFlag[i];
}

void FluidSimManager::_setSimTimeInteg(int timeIntegIndex)
{
	for (auto& sim : _sim)
	{
		sim->setTimeInteg(_timeInteg[timeIntegIndex]);
	}
}

void FluidSimManager::_resetSim(DX12App* dxapp)
{
	dxapp->resetSimulationState();
	dxapp->update();
	dxapp->draw();
	_simTime = 0;
	_simFrame = 0;
}

#pragma region Implementation
// ################################## Implementation ####################################
void FluidSimManager::iUpdate()
{
	clock_t startTime = clock();
	_sim[_simIndex]->iUpdate();
	clock_t endTime = clock();
	
	_simTime += endTime - startTime; // ms
	_simFrame++;
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

void FluidSimManager::iWMCreate(HWND hwnd, HINSTANCE hInstance)
{
	CreateWindow(L"button", L"Grid : ON ", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		90, 30, 100, 25, hwnd, reinterpret_cast<HMENU>(_COM::GRID_BTN), hInstance, NULL);
	CreateWindow(L"button", L"Particle : ON ", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		30, 60, 100, 25, hwnd, reinterpret_cast<HMENU>(_COM::PARTICLE_BTN), hInstance, NULL);
	CreateWindow(L"button", L"Velcoity : OFF ", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		150, 60, 100, 25, hwnd, reinterpret_cast<HMENU>(_COM::VELOCITY_BTN), hInstance, NULL);

	CreateWindow(L"button", L"State", WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
		30, 100, 220, 50, hwnd, reinterpret_cast<HMENU>(_COM::STATE_GROUP), hInstance, NULL);
	CreateWindow(L"button", L"Liquid", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
		60, 117, 70, 25, hwnd, reinterpret_cast<HMENU>(_COM::LIQUID_RADIO), hInstance, NULL);
	CreateWindow(L"button", L"Gas", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		140, 117, 70, 25, hwnd, reinterpret_cast<HMENU>(_COM::GAS_RADIO), hInstance, NULL);
	
	CreateWindow(L"button", L"Solver", WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
		30, 160, 220, 50, hwnd, reinterpret_cast<HMENU>(_COM::SOLVER_GROUP), hInstance, NULL);
	CreateWindow(L"button", L"Eulerian", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
		60, 177, 75, 25, hwnd, reinterpret_cast<HMENU>(_COM::EULERIAN_RADIO), hInstance, NULL);
	CreateWindow(L"button", L"PIC/FLIP", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		140, 177, 80, 25, hwnd, reinterpret_cast<HMENU>(_COM::PICFLIP_RADIO), hInstance, NULL);

	CreateWindow(L"static", L"PIC  :", WS_CHILD | WS_VISIBLE,
		60, 220, 40, 20, hwnd, reinterpret_cast<HMENU>(_COM::PIC_TEXT), hInstance, NULL);
	CreateWindow(L"static", _int2wchar(100 - _scrollPos), WS_CHILD | WS_VISIBLE,
		100, 220, 30, 20, hwnd, reinterpret_cast<HMENU>(_COM::PIC_RATIO), hInstance, NULL);
	CreateWindow(L"static", L"FLIP :", WS_CHILD | WS_VISIBLE,
		155, 220, 40, 20, hwnd, reinterpret_cast<HMENU>(_COM::FLIP_TEXT), hInstance, NULL);
	CreateWindow(L"static", _int2wchar(_scrollPos), WS_CHILD | WS_VISIBLE,
		195, 220, 30, 20, hwnd, reinterpret_cast<HMENU>(_COM::FLIP_RATIO), hInstance, NULL);
	HWND scroll =
		CreateWindow(L"scrollbar", NULL, WS_CHILD | WS_VISIBLE | SBS_HORZ,
			40, 250, 200, 20, hwnd, reinterpret_cast<HMENU>(_COM::RATIO_BAR), hInstance, NULL);

	CreateWindow(L"button", L"Time Integration", WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
		30, 290, 220, 79, hwnd, reinterpret_cast<HMENU>(_COM::INTEG_GROUP), hInstance, NULL);
	CreateWindow(L"button", L"Fixed", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
		42, 309, 60, 25, hwnd, reinterpret_cast<HMENU>(_COM::FIXED_RADIO), hInstance, NULL);
	CreateWindow(L"button", L"Global", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		107, 309, 65, 25, hwnd, reinterpret_cast<HMENU>(_COM::GLOBAL_RADIO), hInstance, NULL);
	CreateWindow(L"button", L"Koike", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		177, 309, 60, 25, hwnd, reinterpret_cast<HMENU>(_COM::KOIKE_RADIO), hInstance, NULL);
	CreateWindow(L"button", L"Reinhardt", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		42, 335, 90, 25, hwnd, reinterpret_cast<HMENU>(_COM::REINHARDT_RADIO), hInstance, NULL);
	CreateWindow(L"button", L"Ours", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		140, 335, 60, 25, hwnd, reinterpret_cast<HMENU>(_COM::OURS_RADIO), hInstance, NULL);

	CreateWindow(L"button", L"¡«", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		65, 385, 50, 25, hwnd, reinterpret_cast<HMENU>(_COM::PLAY), hInstance, NULL);
	CreateWindow(L"button", L"¡á", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		115, 385, 50, 25, hwnd, reinterpret_cast<HMENU>(_COM::STOP), hInstance, NULL);
	CreateWindow(L"button", L"¢ºl", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		165, 385, 50, 25, hwnd, reinterpret_cast<HMENU>(_COM::NEXTSTEP), hInstance, NULL);

	CreateWindow(L"button", L"¢Ã Record", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		88, 430, 100, 25, hwnd, reinterpret_cast<HMENU>(_COM::RECORD), hInstance, NULL);

	CreateWindow(L"static", L"time :", WS_CHILD | WS_VISIBLE,
		60, 480, 40, 20, hwnd, reinterpret_cast<HMENU>(-1), hInstance, NULL);
	CreateWindow(L"static", _int2wchar(_simTime), WS_CHILD | WS_VISIBLE,
		110, 480, 40, 20, hwnd, reinterpret_cast<HMENU>(_COM::TIME_TEXT), hInstance, NULL);
	CreateWindow(L"static", L"frame :", WS_CHILD | WS_VISIBLE,
		60, 500, 40, 20, hwnd, reinterpret_cast<HMENU>(-1), hInstance, NULL);
	CreateWindow(L"static", _int2wchar(_simFrame), WS_CHILD | WS_VISIBLE,
		110, 500, 40, 20, hwnd, reinterpret_cast<HMENU>(_COM::FRAME_TEXT), hInstance, NULL);


	CheckRadioButton(hwnd, static_cast<int>(_COM::LIQUID_RADIO), static_cast<int>(_COM::GAS_RADIO), static_cast<int>(_COM::LIQUID_RADIO));
	CheckRadioButton(hwnd, static_cast<int>(_COM::EULERIAN_RADIO), static_cast<int>(_COM::PICFLIP_RADIO), static_cast<int>(_COM::PICFLIP_RADIO));
	CheckRadioButton(hwnd, static_cast<int>(_COM::FIXED_RADIO), static_cast<int>(_COM::OURS_RADIO), static_cast<int>(_COM::FIXED_RADIO));

	EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::NEXTSTEP)), false);

	//EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::RATIO_BAR)), false);
	//EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::PIC_TEXT)), false);
	//EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::PIC_RATIO)), false);
	//EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::FLIP_TEXT)), false);
	//EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::FLIP_RATIO)), false);

	EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::GAS_RADIO)), false);
	EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::LIQUID_RADIO)), false);

	SetScrollRange(scroll, SB_CTL, 0, 100, TRUE);
	SetScrollPos(scroll, SB_CTL, _scrollPos, TRUE);

	SetTimer(hwnd, 1, 10, NULL);
}

void FluidSimManager::iWMTimer(HWND hwnd)
{
	SetDlgItemText(hwnd, static_cast<int>(_COM::TIME_TEXT), _int2wchar(_simTime));
	SetDlgItemText(hwnd, static_cast<int>(_COM::FRAME_TEXT), _int2wchar(_simFrame));
}

void FluidSimManager::iWMDestory(HWND hwnd)
{
	KillTimer(hwnd, 1);
}

void FluidSimManager::iWMHScroll(HWND hwnd, WPARAM wParam, LPARAM lParam, HINSTANCE hInstance, DX12App* dxapp)
{
	switch (LOWORD(wParam))
	{
		case SB_THUMBTRACK:
			_scrollPos = HIWORD(wParam);
			break;

		case SB_LINELEFT:
			_scrollPos = max(0, _scrollPos - 1);
			break;

		case SB_LINERIGHT:
			_scrollPos = min(100, _scrollPos + 1);
			break;

		case SB_PAGELEFT:
			_scrollPos = max(0, _scrollPos - 5);
			break;

		case SB_PAGERIGHT:
			_scrollPos = min(100, _scrollPos + 5);
			break;
	}

	SetScrollPos((HWND)lParam, SB_CTL, _scrollPos, TRUE);
	SetDlgItemText(hwnd, static_cast<int>(_COM::PIC_RATIO), _int2wchar(100 - _scrollPos));
	SetDlgItemText(hwnd, static_cast<int>(_COM::FLIP_RATIO), _int2wchar(_scrollPos));

	dynamic_cast<PICFLIPSim*>(_sim[_simIndex])->setFlipRatio(_scrollPos);
	_resetSim(dxapp);
}

void FluidSimManager::iWMCommand(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, HINSTANCE hInstance, bool& updateFlag, DX12App* dxapp)
{
	switch (LOWORD(wParam))
	{
		// ### Top 3 buttons ###
		case static_cast<int>(_COM::GRID_BTN) :
		{
			bool flag = !_getDrawFlag(FLAG::GRID);
			SetDlgItemText(hwnd, static_cast<int>(_COM::GRID_BTN), flag ? L"Grid : ON " : L"Grid : OFF");
			_setDrawFlag(FLAG::GRID, flag);
			dxapp->draw();
		}
		break;

		case static_cast<int>(_COM::PARTICLE_BTN) :
		{
			bool flag = !_getDrawFlag(FLAG::PARTICLE);
			SetDlgItemText(hwnd, static_cast<int>(_COM::PARTICLE_BTN), flag ? L"Particle : ON " : L"Particle : OFF");
			_setDrawFlag(FLAG::PARTICLE, flag);
			dxapp->draw();

		}
		break;

		case static_cast<int>(_COM::VELOCITY_BTN) :
		{
			bool flag = !_getDrawFlag(FLAG::VELOCITY);
			SetDlgItemText(hwnd, static_cast<int>(_COM::VELOCITY_BTN), flag ? L"Velocity : ON " : L"Velocity : OFF");
			_setDrawFlag(FLAG::VELOCITY, flag);
			dxapp->draw();
		}
		break;
		// #####################

		// ### Execution buttons ###
		case static_cast<int>(_COM::PLAY) :
		{
			updateFlag = !updateFlag;
			SetDlgItemText(hwnd, static_cast<int>(_COM::PLAY), updateFlag ? L"¡«" : L"¢º");

			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::STOP)), true);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::NEXTSTEP)), !updateFlag);
		}
		break;
		case static_cast<int>(_COM::STOP) :
		{
			_resetSim(dxapp);
		}
		break;
		case static_cast<int>(_COM::NEXTSTEP) :
		{
			if (!updateFlag)
			{
				dxapp->update();
				dxapp->draw();
			}
		}
		break;
		// #####################

		// ### State radio buttons ###
		case static_cast<int>(_COM::LIQUID_RADIO) :
		{
			_simIndex = 0;
			_resetSim(dxapp);

			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::INTEG_GROUP)), true);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::FIXED_RADIO)), true);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::GLOBAL_RADIO)), true);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::KOIKE_RADIO)), true);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::REINHARDT_RADIO)), true);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::OURS_RADIO)), true);
		}
		break;
		case static_cast<int>(_COM::GAS_RADIO) :
		{
			_simIndex = 1;
			_resetSim(dxapp);

			CheckRadioButton(hwnd, static_cast<int>(_COM::FIXED_RADIO), static_cast<int>(_COM::OURS_RADIO), static_cast<int>(_COM::FIXED_RADIO));
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::INTEG_GROUP)), false);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::FIXED_RADIO)), false);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::GLOBAL_RADIO)), false);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::KOIKE_RADIO)), false);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::REINHARDT_RADIO)), false);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::OURS_RADIO)), false);

			_setSimTimeInteg(0);
		}
		break;
		// #####################

		// ### Solver radio buttons ###
		case static_cast<int>(_COM::EULERIAN_RADIO) :
		{
			if (_simIndex > 1) _simIndex = 0;
			_resetSim(dxapp);

			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::STATE_GROUP)), true);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::LIQUID_RADIO)), true);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::GAS_RADIO)), true);

			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::RATIO_BAR)), false);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::PIC_TEXT)), false);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::PIC_RATIO)), false);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::FLIP_TEXT)), false);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::FLIP_RATIO)), false);
		}
		break;
		case static_cast<int>(_COM::PICFLIP_RADIO) :
		{
			_simIndex = 2;
			_resetSim(dxapp);

			CheckRadioButton(hwnd, static_cast<int>(_COM::LIQUID_RADIO), static_cast<int>(_COM::GAS_RADIO), static_cast<int>(_COM::LIQUID_RADIO));
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::STATE_GROUP)), false);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::LIQUID_RADIO)), false);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::GAS_RADIO)), false);

			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::RATIO_BAR)), true);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::PIC_TEXT)), true);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::PIC_RATIO)), true);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::FLIP_TEXT)), true);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::FLIP_RATIO)), true);

			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::INTEG_GROUP)), true);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::FIXED_RADIO)), true);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::GLOBAL_RADIO)), true);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::KOIKE_RADIO)), true);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::REINHARDT_RADIO)), true);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::OURS_RADIO)), true);
		}
		break;
		// #####################

		// ### Time integration radio buttons ###
		case static_cast<int>(_COM::FIXED_RADIO) :
		{
			_setSimTimeInteg(0);
			_resetSim(dxapp);
		}
		break;
		case static_cast<int>(_COM::GLOBAL_RADIO) :
		{
			_setSimTimeInteg(1);
			_resetSim(dxapp);
		}
		break;
		case static_cast<int>(_COM::REINHARDT_RADIO) :
		{
		}
		break;
		case static_cast<int>(_COM::KOIKE_RADIO) :
		{
			_setSimTimeInteg(2);
			_resetSim(dxapp);
		}
		break;
		case static_cast<int>(_COM::OURS_RADIO) :
		{
			_setSimTimeInteg(3);
			_resetSim(dxapp);
		}
		break;
		// #####################
	}
}


void FluidSimManager::iUpdateConstantBuffer(vector<ConstantBuffer>& constantBuffer, int i)
{
	_sim[_simIndex]->iUpdateConstantBuffer(constantBuffer, i);
}

void FluidSimManager::iDraw(ComPtr<ID3D12GraphicsCommandList>& mCommandList, int size, UINT indexCount, int i)
{
	_sim[_simIndex]->iDraw(mCommandList, size, indexCount, _drawFlag, i);
}
// #######################################################################################
#pragma endregion