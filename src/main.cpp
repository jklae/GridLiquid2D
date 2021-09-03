#pragma once

// Console window is displayed in debug mode.
//#ifdef _DEBUG
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
//#endif

#include "EulerGasSim.h" // This includes Win32App.h
#include "EulerLiquidSim.h"
#include "PICLiquidSim.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
    Win32App winApp(800, 800);
    winApp.initialize(hInstance);

    std::vector<ISimulation*> fluidsim;
    fluidsim.push_back(new EulerLiquidSim(0.01f, 0));
    fluidsim.push_back(new EulerGasSim(0.1f, 10));
    fluidsim.push_back(new PICLiquidSim(0.01f, 0));

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