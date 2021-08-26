#pragma once

// Console window is displayed in debug mode.
#ifdef _DEBUG
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif

#include "EulerianGasSimulation.h" // This includes Win32App.h


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
    Win32App winApp(800, 800);
    winApp.initialize(hInstance);

    EulerianSimulation* eulersim = new EulerianGasSimulation();
    eulersim->setGridDomain(30, 20);
    eulersim->initialize();

    DX12App* dxapp = new DX12App();
    dxapp->setProjectionType(PROJ::ORTHOGRAPHIC);
    dxapp->setSimulation(eulersim, 0.1);

    winApp.initDirectX(dxapp);

    return winApp.run();
}