#include "LiquidManager.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace std;


LiquidManager::LiquidManager(int x, int y, float timeStep)
{
	_index.gridCount = x + 2; // 2 are boundaries.
	_index.particleCount = 1;

	_sim.push_back(new Eulerian(_index, _ex, FPS_60 / 2.0f));
	_sim.push_back(new PICFLIP(_index, _ex, FPS_60 / 2.0f));

	_interp.push_back(new Trilinear(_index));

	// 0 is Eulerian, 1 is PIC/FLIP
	_simIndex = 0;

	// 0 is Trilinear
	_setSimInterp(0);

}

LiquidManager::~LiquidManager()
{
	for (auto& sim : _sim)
	{
		delete sim;
	}

	for (auto& interp : _interp)
	{
		delete interp;
	}
}

wchar_t* LiquidManager::_int2wchar(int value)
{
	_itow(value, wBuffer, 10);
	return wBuffer;
}

void LiquidManager::_setSimInterp(int interpIndex)
{
	for (auto& sim : _sim)
	{
		sim->setInterp(_interp[interpIndex]);
	}
}

void LiquidManager::_setDrawFlag(FLAG flagType, bool flag)
{
	int i = static_cast<int>(flagType);
	_drawFlag[i] = flag;
}

bool LiquidManager::_getDrawFlag(FLAG flagType)
{
	int i = static_cast<int>(flagType);
	return _drawFlag[i];
}

void LiquidManager::_resetSim(DX12App* dxapp)
{
	dxapp->resetSimulationState();
	dxapp->update();
	dxapp->draw();
	_simTime = 0;
	_simFrame = 0;
}

#pragma region Implementation
// ################################## Implementation ####################################
void LiquidManager::iUpdate()
{
	clock_t startTime = clock();
	_sim[_simIndex]->iUpdate();
	clock_t endTime = clock();
	
	_simTime += endTime - startTime; // ms
	_simFrame++;
}

void LiquidManager::iResetSimulationState(vector<ConstantBuffer>& constantBuffer)
{
	_sim[_simIndex]->iResetSimulationState(constantBuffer, _ex);
}


vector<Vertex> LiquidManager::iGetVertice()
{
	return _sim[_simIndex]->iGetVertice();;
}

vector<unsigned int> LiquidManager::iGetIndice()
{
	return _sim[_simIndex]->iGetIndice();
}

int LiquidManager::iGetObjectCount()
{
	return _sim[_simIndex]->iGetObjectCount();
}


void LiquidManager::iCreateObjectParticle(vector<ConstantBuffer>& constantBuffer)
{
	_sim[_simIndex]->iCreateObjectParticle(constantBuffer);
}

void LiquidManager::iUpdateConstantBuffer(vector<ConstantBuffer>& constantBuffer, int i)
{
	_sim[_simIndex]->iUpdateConstantBuffer(constantBuffer, i);
}

void LiquidManager::iDraw(ComPtr<ID3D12GraphicsCommandList>& mCommandList, int size, UINT indexCount, int i)
{
	_sim[_simIndex]->iDraw(mCommandList, size, indexCount, _drawFlag, i);
}


void LiquidManager::iWMCreate(HWND hwnd, HINSTANCE hInstance)
{
	CreateWindow(L"button", L"Grid : ON ", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		90, 30, 100, 25, hwnd, reinterpret_cast<HMENU>(_COM::GRID_BTN), hInstance, NULL);
	CreateWindow(L"button", L"Particle : ON ", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		30, 60, 100, 25, hwnd, reinterpret_cast<HMENU>(_COM::PARTICLE_BTN), hInstance, NULL);
	CreateWindow(L"button", L"Velcoity : OFF ", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		150, 60, 100, 25, hwnd, reinterpret_cast<HMENU>(_COM::VELOCITY_BTN), hInstance, NULL);

	CreateWindow(L"button", L"Example", WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
		30, 100, 220, 50, hwnd, reinterpret_cast<HMENU>(_COM::EX_GROUP), hInstance, NULL);
	CreateWindow(L"button", L"Dam", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
		50, 117, 50, 25, hwnd, reinterpret_cast<HMENU>(_COM::DAM_RADIO), hInstance, NULL);
	CreateWindow(L"button", L"Drop1", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		105, 117, 60, 25, hwnd, reinterpret_cast<HMENU>(_COM::DROP1_RADIO), hInstance, NULL);
	CreateWindow(L"button", L"Drop2", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		170, 117, 60, 25, hwnd, reinterpret_cast<HMENU>(_COM::DROP2_RADIO), hInstance, NULL);
	
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

	CreateWindow(L"button", L"Interpolation", WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
		30, 290, 220, 79, hwnd, reinterpret_cast<HMENU>(_COM::INTERP_GROUP), hInstance, NULL);
	CreateWindow(L"button", L"Trilinear", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
		42, 309, 80, 25, hwnd, reinterpret_cast<HMENU>(_COM::TRILINEAR_RADIO), hInstance, NULL);
	CreateWindow(L"button", L"Cubic Spline", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		130, 309, 100, 25, hwnd, reinterpret_cast<HMENU>(_COM::CUBIC_RADIO), hInstance, NULL);
	CreateWindow(L"button", L"Ours", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		42, 335, 60, 25, hwnd, reinterpret_cast<HMENU>(_COM::OURS_RADIO), hInstance, NULL);

	CreateWindow(L"button", _updateFlag ? L"¡«" : L"¢º", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		65, 385, 50, 25, hwnd, reinterpret_cast<HMENU>(_COM::PLAY), hInstance, NULL);
	CreateWindow(L"button", L"¡á", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		115, 385, 50, 25, hwnd, reinterpret_cast<HMENU>(_COM::STOP), hInstance, NULL);
	CreateWindow(L"button", L"¢ºl", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		165, 385, 50, 25, hwnd, reinterpret_cast<HMENU>(_COM::NEXTSTEP), hInstance, NULL);

	CreateWindow(L"static", L"time :", WS_CHILD | WS_VISIBLE,
		60, 480, 40, 20, hwnd, reinterpret_cast<HMENU>(-1), hInstance, NULL);
	CreateWindow(L"static", _int2wchar(_simTime), WS_CHILD | WS_VISIBLE,
		110, 480, 40, 20, hwnd, reinterpret_cast<HMENU>(_COM::TIME_TEXT), hInstance, NULL);
	CreateWindow(L"static", L"frame :", WS_CHILD | WS_VISIBLE,
		60, 500, 40, 20, hwnd, reinterpret_cast<HMENU>(-1), hInstance, NULL);
	CreateWindow(L"static", _int2wchar(_simFrame), WS_CHILD | WS_VISIBLE,
		110, 500, 40, 20, hwnd, reinterpret_cast<HMENU>(_COM::FRAME_TEXT), hInstance, NULL);


	CheckRadioButton(hwnd, static_cast<int>(_COM::DAM_RADIO), static_cast<int>(_COM::DROP2_RADIO), static_cast<int>(_COM::DROP2_RADIO));
	CheckRadioButton(hwnd, static_cast<int>(_COM::EULERIAN_RADIO), static_cast<int>(_COM::PICFLIP_RADIO), (_simIndex == 0) ? static_cast<int>(_COM::EULERIAN_RADIO) : static_cast<int>(_COM::PICFLIP_RADIO));
	CheckRadioButton(hwnd, static_cast<int>(_COM::TRILINEAR_RADIO), static_cast<int>(_COM::OURS_RADIO), static_cast<int>(_COM::TRILINEAR_RADIO));

	if (_updateFlag)
	{
		EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::NEXTSTEP)), false);
	}

	if (_simIndex == 0)
	{
		EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::RATIO_BAR)), false);
		EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::PIC_TEXT)), false);
		EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::PIC_RATIO)), false);
		EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::FLIP_TEXT)), false);
		EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::FLIP_RATIO)), false);
	}

	SetScrollRange(scroll, SB_CTL, 0, 100, TRUE);
	SetScrollPos(scroll, SB_CTL, _scrollPos, TRUE);
	dynamic_cast<PICFLIP*>(_sim[1])->setFlipRatio(_scrollPos);

	SetTimer(hwnd, 1, 10, NULL);
}

void LiquidManager::iWMTimer(HWND hwnd)
{
	SetDlgItemText(hwnd, static_cast<int>(_COM::TIME_TEXT), _int2wchar(_simTime));
	SetDlgItemText(hwnd, static_cast<int>(_COM::FRAME_TEXT), _int2wchar(_simFrame));
}

void LiquidManager::iWMDestory(HWND hwnd)
{
	KillTimer(hwnd, 1);
}

void LiquidManager::iWMHScroll(HWND hwnd, WPARAM wParam, LPARAM lParam, HINSTANCE hInstance, DX12App* dxapp)
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

	dynamic_cast<PICFLIP*>(_sim[_simIndex])->setFlipRatio(_scrollPos);
	_resetSim(dxapp);
}

void LiquidManager::iWMCommand(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, HINSTANCE hInstance, DX12App* dxapp)
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
			_updateFlag = !_updateFlag;
			SetDlgItemText(hwnd, static_cast<int>(_COM::PLAY), _updateFlag ? L"¡«" : L"¢º");

			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::STOP)), true);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::NEXTSTEP)), !_updateFlag);
		}
		break;
		case static_cast<int>(_COM::STOP) :
		{
			_resetSim(dxapp);
		}
		break;
		case static_cast<int>(_COM::NEXTSTEP) :
		{
			if (!_updateFlag)
			{
				dxapp->update();
				dxapp->draw();
			}
		}
		break;
		// #####################

		// ### State radio buttons ###
		case static_cast<int>(_COM::DAM_RADIO) :
		{
			_ex = EX::DAM;
			_resetSim(dxapp);
		}
		break;
		case static_cast<int>(_COM::DROP1_RADIO) :
		{
			_ex = EX::DROP1;
			_resetSim(dxapp);
		}
		break; 
		case static_cast<int>(_COM::DROP2_RADIO) :
		{
			_ex = EX::DROP2;
			_resetSim(dxapp);
		}
		break;
		// #####################

		// ### Solver radio buttons ###
		case static_cast<int>(_COM::EULERIAN_RADIO) :
		{
			_simIndex = 0;
			_resetSim(dxapp);

			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::RATIO_BAR)), false);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::PIC_TEXT)), false);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::PIC_RATIO)), false);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::FLIP_TEXT)), false);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::FLIP_RATIO)), false);
		}
		break;
		case static_cast<int>(_COM::PICFLIP_RADIO) :
		{
			_simIndex = 1;
			_resetSim(dxapp);

			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::RATIO_BAR)), true);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::PIC_TEXT)), true);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::PIC_RATIO)), true);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::FLIP_TEXT)), true);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(_COM::FLIP_RATIO)), true);
		}
		break;
		// #####################

		// ###  radio buttons ###
		case static_cast<int>(_COM::TRILINEAR_RADIO) :
		{
			_resetSim(dxapp);
			_setSimInterp(0);
		}
		break;
		case static_cast<int>(_COM::CUBIC_RADIO) :
		{
			_resetSim(dxapp);
		}
		break;
		case static_cast<int>(_COM::OURS_RADIO) :
		{
			_resetSim(dxapp);
		}
		break;
		// #####################
	}
}


bool LiquidManager::iGetUpdateFlag()
{
	return _updateFlag;
}
// #######################################################################################
#pragma endregion