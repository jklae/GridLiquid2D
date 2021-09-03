#pragma once

// Console window is displayed in debug mode.
//#ifdef _DEBUG
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
//#endif

#include "FluidSimManager.h" // This includes Win32App.h


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
    Win32App winApp(800, 800);
    winApp.initialize(hInstance);

    FluidSimManager* fluidsim = new FluidSimManager();
    //fluidsim->setGridDomain(30, 20);
    //fluidsim->initialize();

    DX12App* dxapp = new DX12App();
    dxapp->setProjectionType(PROJ::ORTHOGRAPHIC);

    winApp.initDirectX(dxapp, fluidsim);

    return winApp.run();
}