#pragma once

// Console window is displayed in debug mode.
//#ifdef _DEBUG
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
//#endif

#include "FluidSimManager.h" // This includes Win32App.h


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
    // Simulation init
    int x = 38;
    int y = 60;
    float timeStep = 0.01f;

    FluidSimManager* fluidsim = new FluidSimManager(x, y, timeStep);

    // DirectX init
    DX12App* dxapp = new DX12App();
    dxapp->setProjectionType(PROJ::ORTHOGRAPHIC);

    // Window init
    Win32App winApp(800, 800);
    winApp.initialize(hInstance, dxapp, fluidsim);

    return winApp.run();
}