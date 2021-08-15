#pragma once

// Console window is displayed in debug mode.
#ifdef _DEBUG
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif

#include "EulerianSimulation.h" // This includes Win32App.h


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
    int width = 800;
    int height = 800;

    Win32App winApp(width, height);
    winApp.initialize(hInstance);

    EulerianSimulation* eulersim = new EulerianSimulation();
    eulersim->setGridCountXY(10, 10);
    eulersim->setGridScale(0.2f);
    eulersim->initialize();

    DX12App* dxapp = new DX12App();
    dxapp->setSimulation(eulersim, 0.05);

    winApp.initDirectX(dxapp);

    return winApp.run();
}