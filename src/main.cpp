#pragma once

// Console window is displayed in debug mode.
//#ifdef _DEBUG
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
//#endif

#include "LiquidManager.h" // This includes Win32App.h


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
    // Simulation init
    int x = 60;
    int y = 50;
    float timeStep = FPS_120;
    
    LiquidManager* liquidman = new LiquidManager(x, y, timeStep);

    // DirectX init
    DX12App* dxapp = new DX12App();
    dxapp->setProjectionType(PROJ::ORTHOGRAPHIC);
    dxapp->setBackgroundColor(DirectX::Colors::LightSlateGray);

    // Window init
    Win32App winApp(800, 800);
    winApp.initialize(hInstance, dxapp, liquidman);

    return winApp.run();
}