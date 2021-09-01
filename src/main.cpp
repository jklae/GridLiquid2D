#pragma once

// Console window is displayed in debug mode.
#ifdef _DEBUG
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif

#include "EulerianGasSimulation.h" // This includes Win32App.h
#include "EulerianLiquidSimulation.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
    Win32App winApp(800, 800);
    winApp.initialize(hInstance);

    std::vector<ISimulation*> fluidsim;
    fluidsim.push_back(new EulerianLiquidSimulation());
    fluidsim.push_back(new EulerianGasSimulation());

    for (auto& sim : fluidsim)
    {
        dynamic_cast<FluidSimulation*>(sim)->setGridDomain(30, 20);
        dynamic_cast<FluidSimulation*>(sim)->initialize();
    }

    /*FluidSimulation* fluidsim = new EulerianLiquidSimulation();
    fluidsim->setGridDomain(30, 20);
    fluidsim->initialize();*/


    DX12App* dxapp = new DX12App();
    dxapp->setProjectionType(PROJ::ORTHOGRAPHIC);

    winApp.initDirectX(dxapp, fluidsim, 0.1f);

    return winApp.run();
}