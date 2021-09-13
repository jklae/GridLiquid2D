#pragma once

// Console window is displayed in debug mode.
//#ifdef _DEBUG
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
//#endif

#include "FluidSimManager.h" // This includes Win32App.h
#include "EulerGasSim.h" 
#include "EulerLiquidSim.h"
#include "PICLiquidSim.h"
#include "FLIPLiquidSim.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
    // Simulation init
    std::vector<GridFluidSim*> sims;
    sims.push_back(new EulerLiquidSim(0.01f, 0));
    sims.push_back(new EulerGasSim(0.1f, 0));
    sims.push_back(new PICLiquidSim(0.01f, 0));
    sims.push_back(new FLIPLiquidSim(0.01f, 0));

    for (auto& sim : sims)
    {
        sim->setGridDomain(60, 20);
        sim->initialize();
    }

    FluidSimManager* fluidsim = new FluidSimManager(sims);

    // DirectX init
    DX12App* dxapp = new DX12App();
    dxapp->setProjectionType(PROJ::ORTHOGRAPHIC);

    // Window init
    Win32App winApp(800, 800);
    winApp.initialize(hInstance, dxapp, fluidsim);
    //winApp.initDirectX(dxapp, fluidsim);

    return winApp.run();
}