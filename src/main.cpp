#pragma once

// Console window is displayed in debug mode.
//#ifdef _DEBUG
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
//#endif

#include "LiquidManager.h" // This includes Win32App.h

using namespace DXViewer::xmint3;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
    // Simulation init
    int x = 3;
    int y = 2;
    float timeStep = FPS_120;
    
    LiquidManager* liquidman = new LiquidManager(x, y, timeStep);

    // DirectX init
    DX12App* dxapp = new DX12App();
    dxapp->setCameraProperties(
        PROJ::ORTHOGRAPHIC, 
        static_cast<float>(max_element(liquidman->iGetObjectCount())) * 0.0015f,
        2.0f
    );
    //dxapp->setCameraProperties(PROJ::PERSPECTIVE, 0.0f, 1.5f);
    dxapp->setBackgroundColor(DirectX::Colors::LightSlateGray);

    // Window init
    Win32App winApp(800, 800);
    winApp.initialize(hInstance, dxapp, liquidman);

    return winApp.run();
}