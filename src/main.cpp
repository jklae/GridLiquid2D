#pragma once

// Console window is displayed in debug mode.
//#ifdef _DEBUG
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
//#endif

#include "EulerianGasSimulation.h" // This includes Win32App.h
#include "EulerianLiquidSimulation.h"
#include "PICLiquidSimulation.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
    Win32App winApp(800, 800);
    winApp.initialize(hInstance);

    std::vector<ISimulation*> fluidsim;
    fluidsim.push_back(new EulerianLiquidSimulation(0.01f));
    fluidsim.push_back(new EulerianGasSimulation(0.1f));
    fluidsim.push_back(new PICLiquidSimulation(0.01f));

    for (auto& sim : fluidsim)
    {
        dynamic_cast<GridFluidSim*>(sim)->setGridDomain(30, 20);
        dynamic_cast<GridFluidSim*>(sim)->initialize();
    }

    DX12App* dxapp = new DX12App();
    dxapp->setProjectionType(PROJ::ORTHOGRAPHIC);

    winApp.initDirectX(dxapp, fluidsim);

    return winApp.run();
}