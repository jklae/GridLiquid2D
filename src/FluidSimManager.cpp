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

	CreateWindow(L"static", L"Delay", WS_CHILD | WS_VISIBLE,
		50, 220, 40, 20, hwnd, reinterpret_cast<HMENU>(-1), hInstance, NULL);
	HWND scroll =
		CreateWindow(L"scrollbar", NULL, WS_CHILD | WS_VISIBLE | SBS_HORZ,
			40, 250, 200, 20, hwnd, reinterpret_cast<HMENU>(_COM::DELAY_BAR), hInstance, NULL);

	CreateWindow(L"button", L"бл", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		65, 290, 50, 25, hwnd, reinterpret_cast<HMENU>(_COM::PLAY), hInstance, NULL);
	CreateWindow(L"button", L"бс", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		115, 290, 50, 25, hwnd, reinterpret_cast<HMENU>(_COM::STOP), hInstance, NULL);
	CreateWindow(L"button", L"в║l", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		175, 290, 50, 25, hwnd, reinterpret_cast<HMENU>(_COM::NEXTSTEP), hInstance, NULL);

	CheckRadioButton(hwnd, static_cast<int>(_COM::LIQUID_RADIO), static_cast<int>(_COM::GAS_RADIO), static_cast<int>(_COM::LIQUID_RADIO));
	CheckRadioButton(hwnd, static_cast<int>(_COM::EULERIAN_RADIO), static_cast<int>(_COM::PICFLIP_RADIO), static_cast<int>(_COM::EULERIAN_RADIO));

	EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::NEXTSTEP)), false);

	//EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::DELAY_BAR)), false);
	//EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::GAS_RADIO)), false);
	//EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::LIQUID_RADIO)), false);

	SetScrollRange(scroll, SB_CTL, 0, 100, TRUE);
	SetScrollPos(scroll, SB_CTL, 10, TRUE);
}


void FluidSimManager::iWMHScroll(HWND hwnd, WPARAM wParam, LPARAM lParam, HINSTANCE hInstance)
{
	switch (LOWORD(wParam))
	{
		case SB_THUMBTRACK:
		{
			//_sim[_simIndex]->
			_scrollPos = HIWORD(wParam);
		}
		break;

		case SB_LINELEFT:
		{
			_scrollPos = max(0, _scrollPos - 1);
		}
		break;

		case SB_LINERIGHT:
		{
			_scrollPos = min(100, _scrollPos + 1);
		}
		break;

		case SB_PAGELEFT:
		{
			_scrollPos = max(0, _scrollPos - 5);
		}
		break;

		case SB_PAGERIGHT:
		{
			_scrollPos = min(100, _scrollPos + 5);
		}
		break;
	}

	cout << _scrollPos << endl;
	SetScrollPos((HWND)lParam, SB_CTL, _scrollPos, TRUE);
}

void FluidSimManager::iWMCommand(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, HINSTANCE hInstance, bool& updateFlag, DX12App* dxapp)
{
	switch (LOWORD(wParam))
	{
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

		case static_cast<int>(_COM::PLAY) :
		{
			updateFlag = !updateFlag;
			SetDlgItemText(hwnd, static_cast<int>(_COM::PLAY), updateFlag ? L"бл" : L"в║");

			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::STOP)), true);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::NEXTSTEP)), !updateFlag);
		}
		break;

		case static_cast<int>(_COM::STOP) :
		{
			dxapp->resetSimulationState();
			dxapp->update();
			dxapp->draw();
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

		case static_cast<int>(_COM::LIQUID_RADIO) :
		{
			_simIndex = 0;
			dxapp->resetSimulationState();
			dxapp->update();
			dxapp->draw();
		}
		break;
		case static_cast<int>(_COM::GAS_RADIO) :
		{
			_simIndex = 1;
			dxapp->resetSimulationState();
			dxapp->update();
			dxapp->draw();
		}
		break;
		case static_cast<int>(_COM::EULERIAN_RADIO) :
		{
			if (_simIndex > 1) _simIndex = 0;
			dxapp->resetSimulationState();
			dxapp->update();
			dxapp->draw();

			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::LIQUID_RADIO)), true);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::GAS_RADIO)), true);
		}
		break;
		case static_cast<int>(_COM::PICFLIP_RADIO) :
		{
			_simIndex = 2;
			dxapp->resetSimulationState();
			dxapp->update();
			dxapp->draw();

			CheckRadioButton(hwnd, static_cast<int>(_COM::LIQUID_RADIO), static_cast<int>(_COM::GAS_RADIO), static_cast<int>(_COM::LIQUID_RADIO));
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::GAS_RADIO)), false);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::LIQUID_RADIO)), false);
		}
		break;
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