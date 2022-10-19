#include "LiquidManager.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace std;


LiquidManager::LiquidManager(int x, int y, float timeStep)
{
	_sim.push_back(new Eulerian(x, y, _ex, timeStep));
	_sim.push_back(new PICFLIP(x, y, _ex, timeStep));
}

LiquidManager::~LiquidManager()
{
	for (auto& sim : _sim)
	{
		delete sim;
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

#pragma region Implementation
// ################################## Implementation ####################################
// Simulation methods
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

	_dxapp->update();
	_dxapp->draw();
	_simTime = 0;
	_simFrame = 0;
}


// Mesh methods
vector<Vertex>& LiquidManager::iGetVertices()
{
	return _sim[_simIndex]->iGetVertices();
}

vector<unsigned int>& LiquidManager::iGetIndices()
{
	return _sim[_simIndex]->iGetIndices();
}

UINT LiquidManager::iGetVertexBufferSize()
{
	return _sim[_simIndex]->iGetVertexBufferSize();
}

UINT LiquidManager::iGetIndexBufferSize()
{
	return _sim[_simIndex]->iGetIndexBufferSize();
}


// DirectX methods
void LiquidManager::iCreateObject(vector<ConstantBuffer>& constantBuffer)
{
	_sim[_simIndex]->iCreateObject(constantBuffer);
}

void LiquidManager::iUpdateConstantBuffer(vector<ConstantBuffer>& constantBuffer, int i)
{
	_sim[_simIndex]->iUpdateConstantBuffer(constantBuffer, i);
}

void LiquidManager::iDraw(ComPtr<ID3D12GraphicsCommandList>& mCommandList, int size, UINT indexCount, int i)
{
	_sim[_simIndex]->iDraw(mCommandList, size, indexCount, _drawFlag, i);
}

void LiquidManager::iSetDXApp(DX12App* dxApp)
{
	_dxapp = dxApp;
}

UINT LiquidManager::iGetConstantBufferSize()
{
	return _sim[_simIndex]->iGetConstantBufferSize();
}

XMINT3 LiquidManager::iGetObjectCount()
{
	return _sim[_simIndex]->iGetObjectCount();
}

XMFLOAT3 LiquidManager::iGetObjectSize()
{
	return _sim[_simIndex]->iGetObjectSize();
}

DirectX::XMFLOAT3 LiquidManager::iGetObjectPositionOffset()
{
	return _sim[_simIndex]->iGetObjectPositionOffset();
}

bool LiquidManager::iIsUpdated()
{
	return _updateFlag;
}


// WndProc methods
void LiquidManager::iWMCreate(HWND hwnd, HINSTANCE hInstance)
{
	CreateWindow(L"button", _getDrawFlag(FLAG::GRID) ? L"Grid : ON " : L"Grid : OFF ", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		90, 30, 100, 25, hwnd, reinterpret_cast<HMENU>(COM::GRID_BTN), hInstance, NULL);
	CreateWindow(L"button", _getDrawFlag(FLAG::PARTICLE) ? L"Particle : ON " : L"Particle : OFF ", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		30, 60, 100, 25, hwnd, reinterpret_cast<HMENU>(COM::PARTICLE_BTN), hInstance, NULL);
	CreateWindow(L"button", _getDrawFlag(FLAG::VELOCITY) ? L"Velcoity : ON " : L"Velcoity : OFF ", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		150, 60, 100, 25, hwnd, reinterpret_cast<HMENU>(COM::VELOCITY_BTN), hInstance, NULL);

	CreateWindow(L"button", L"Example", WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
		30, 100, 220, 50, hwnd, reinterpret_cast<HMENU>(COM::EX_GROUP), hInstance, NULL);
	CreateWindow(L"button", L"Dam Break", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
		45, 117, 100, 25, hwnd, reinterpret_cast<HMENU>(COM::DAM_RADIO), hInstance, NULL);
	CreateWindow(L"button", L"Fluid Drop", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		148, 117, 100, 25, hwnd, reinterpret_cast<HMENU>(COM::DROP_RADIO), hInstance, NULL);
	
	CreateWindow(L"button", L"Solver", WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
		30, 160, 220, 50, hwnd, reinterpret_cast<HMENU>(COM::SOLVER_GROUP), hInstance, NULL);
	CreateWindow(L"button", L"Eulerian", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
		52, 176, 75, 25, hwnd, reinterpret_cast<HMENU>(COM::EULERIAN_RADIO), hInstance, NULL);
	CreateWindow(L"button", L"PIC/FLIP", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		143, 176, 80, 25, hwnd, reinterpret_cast<HMENU>(COM::PICFLIP_RADIO), hInstance, NULL);

	CreateWindow(L"static", L"PIC  :", WS_CHILD | WS_VISIBLE,
		60, 220, 40, 20, hwnd, reinterpret_cast<HMENU>(COM::PIC_TEXT), hInstance, NULL);
	CreateWindow(L"static", to_wstring(100 - _scrollPos).c_str(), WS_CHILD | WS_VISIBLE,
		100, 220, 30, 20, hwnd, reinterpret_cast<HMENU>(COM::PIC_RATIO), hInstance, NULL);
	CreateWindow(L"static", L"FLIP :", WS_CHILD | WS_VISIBLE,
		155, 220, 40, 20, hwnd, reinterpret_cast<HMENU>(COM::FLIP_TEXT), hInstance, NULL);
	CreateWindow(L"static", to_wstring(_scrollPos).c_str(), WS_CHILD | WS_VISIBLE,
		195, 220, 30, 20, hwnd, reinterpret_cast<HMENU>(COM::FLIP_RATIO), hInstance, NULL);
	HWND scroll =
		CreateWindow(L"scrollbar", NULL, WS_CHILD | WS_VISIBLE | SBS_HORZ,
			40, 250, 200, 20, hwnd, reinterpret_cast<HMENU>(COM::RATIO_BAR), hInstance, NULL);

	CreateWindow(L"button", _updateFlag ? L"��" : L"��", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		65, 290, 50, 25, hwnd, reinterpret_cast<HMENU>(COM::PLAY), hInstance, NULL);
	CreateWindow(L"button", L"��", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		115, 290, 50, 25, hwnd, reinterpret_cast<HMENU>(COM::STOP), hInstance, NULL);
	CreateWindow(L"button", L"��l", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		165, 290, 50, 25, hwnd, reinterpret_cast<HMENU>(COM::NEXTSTEP), hInstance, NULL);

	CreateWindow(L"static", L"time :", WS_CHILD | WS_VISIBLE,
		95, 340, 40, 20, hwnd, reinterpret_cast<HMENU>(-1), hInstance, NULL);
	CreateWindow(L"static", to_wstring(_simTime).c_str(), WS_CHILD | WS_VISIBLE,
		140, 340, 40, 20, hwnd, reinterpret_cast<HMENU>(COM::TIME_TEXT), hInstance, NULL);
	CreateWindow(L"static", L"frame :", WS_CHILD | WS_VISIBLE,
		86, 360, 45, 20, hwnd, reinterpret_cast<HMENU>(-1), hInstance, NULL);
	CreateWindow(L"static", to_wstring(_simFrame).c_str(), WS_CHILD | WS_VISIBLE,
		140, 360, 40, 20, hwnd, reinterpret_cast<HMENU>(COM::FRAME_TEXT), hInstance, NULL);


	CheckRadioButton(hwnd, static_cast<int>(COM::DAM_RADIO), static_cast<int>(COM::DROP_RADIO), 
		(_ex == EX::DAM) ? static_cast<int>(COM::DAM_RADIO) : static_cast<int>(COM::DROP_RADIO));
	CheckRadioButton(hwnd, static_cast<int>(COM::EULERIAN_RADIO), static_cast<int>(COM::PICFLIP_RADIO), 
		(_simIndex == 0) ? static_cast<int>(COM::EULERIAN_RADIO) : static_cast<int>(COM::PICFLIP_RADIO));

	if (_updateFlag)
	{
		EnableWindow(GetDlgItem(hwnd, static_cast<int>(COM::NEXTSTEP)), false);
	}

	if (_simIndex == 0)
	{
		EnableWindow(GetDlgItem(hwnd, static_cast<int>(COM::RATIO_BAR)), false);
		EnableWindow(GetDlgItem(hwnd, static_cast<int>(COM::PIC_TEXT)), false);
		EnableWindow(GetDlgItem(hwnd, static_cast<int>(COM::PIC_RATIO)), false);
		EnableWindow(GetDlgItem(hwnd, static_cast<int>(COM::FLIP_TEXT)), false);
		EnableWindow(GetDlgItem(hwnd, static_cast<int>(COM::FLIP_RATIO)), false);
	}

	SetScrollRange(scroll, SB_CTL, 0, 100, TRUE);
	SetScrollPos(scroll, SB_CTL, _scrollPos, TRUE);
	dynamic_cast<PICFLIP*>(_sim[1])->setFlipRatio(_scrollPos);

	SetTimer(hwnd, 1, 10, NULL);
}

void LiquidManager::iWMCommand(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, HINSTANCE hInstance)
{
	switch (LOWORD(wParam))
	{
		// ### Top 3 buttons ###
		case static_cast<int>(COM::GRID_BTN) :
		{
			bool flag = !_getDrawFlag(FLAG::GRID);
			SetDlgItemText(hwnd, static_cast<int>(COM::GRID_BTN), flag ? L"Grid : ON " : L"Grid : OFF");
			_setDrawFlag(FLAG::GRID, flag);
			_dxapp->draw();
		}
		break;

		case static_cast<int>(COM::PARTICLE_BTN) :
		{
			bool flag = !_getDrawFlag(FLAG::PARTICLE);
			SetDlgItemText(hwnd, static_cast<int>(COM::PARTICLE_BTN), flag ? L"Particle : ON " : L"Particle : OFF");
			_setDrawFlag(FLAG::PARTICLE, flag);
			_dxapp->draw();

		}
		break;

		case static_cast<int>(COM::VELOCITY_BTN) :
		{
			bool flag = !_getDrawFlag(FLAG::VELOCITY);
			SetDlgItemText(hwnd, static_cast<int>(COM::VELOCITY_BTN), flag ? L"Velocity : ON " : L"Velocity : OFF");
			_setDrawFlag(FLAG::VELOCITY, flag);
			_dxapp->draw();
		}
		break;
		// #####################

		// ### Execution buttons ###
		case static_cast<int>(COM::PLAY) :
		{
			_updateFlag = !_updateFlag;
			SetDlgItemText(hwnd, static_cast<int>(COM::PLAY), _updateFlag ? L"��" : L"��");

			EnableWindow(GetDlgItem(hwnd, static_cast<int>(COM::STOP)), true);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(COM::NEXTSTEP)), !_updateFlag);
		}
		break;
		case static_cast<int>(COM::STOP) :
		{
			_dxapp->resetSimulationState();
		}
		break;
		case static_cast<int>(COM::NEXTSTEP) :
		{
			iUpdate();
			_dxapp->update();
			_dxapp->draw();
		}
		break;
		// #####################

		// ### State radio buttons ###
		case static_cast<int>(COM::DAM_RADIO) :
		{
			_ex = EX::DAM;
			_dxapp->resetSimulationState();
		}
		break;
		case static_cast<int>(COM::DROP_RADIO) :
		{
			_ex = EX::DROP;
			_dxapp->resetSimulationState();
		}
		break; 
		// #####################

		// ### Solver radio buttons ###
		case static_cast<int>(COM::EULERIAN_RADIO) :
		{
			_simIndex = 0;
			_dxapp->resetSimulationState();

			EnableWindow(GetDlgItem(hwnd, static_cast<int>(COM::RATIO_BAR)), false);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(COM::PIC_TEXT)), false);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(COM::PIC_RATIO)), false);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(COM::FLIP_TEXT)), false);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(COM::FLIP_RATIO)), false);
		}
		break;
		case static_cast<int>(COM::PICFLIP_RADIO) :
		{
			_simIndex = 1;
			_dxapp->resetSimulationState();

			EnableWindow(GetDlgItem(hwnd, static_cast<int>(COM::RATIO_BAR)), true);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(COM::PIC_TEXT)), true);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(COM::PIC_RATIO)), true);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(COM::FLIP_TEXT)), true);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(COM::FLIP_RATIO)), true);
		}
		break;
		// #####################
	}
}

void LiquidManager::iWMHScroll(HWND hwnd, WPARAM wParam, LPARAM lParam, HINSTANCE hInstance)
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
	SetDlgItemText(hwnd, static_cast<int>(COM::PIC_RATIO), to_wstring(100 - _scrollPos).c_str());
	SetDlgItemText(hwnd, static_cast<int>(COM::FLIP_RATIO), to_wstring(_scrollPos).c_str());

	dynamic_cast<PICFLIP*>(_sim[_simIndex])->setFlipRatio(_scrollPos);
	_dxapp->resetSimulationState();
}

void LiquidManager::iWMTimer(HWND hwnd)
{
	SetDlgItemText(hwnd, static_cast<int>(COM::TIME_TEXT), to_wstring(_simTime).c_str());
	SetDlgItemText(hwnd, static_cast<int>(COM::FRAME_TEXT), to_wstring(_simFrame).c_str());
}

void LiquidManager::iWMDestory(HWND hwnd)
{
	KillTimer(hwnd, 1);
}
// #######################################################################################
#pragma endregion