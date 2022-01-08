#pragma once

// Console window is displayed in debug mode.
#ifdef _DEBUG
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif

#include "LiquidManager.h" // This includes Win32App.h

using namespace DXViewer::xmint3;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
    // Simulation init
    int x = 60;
    int y = 60;
    float timeStep = FPS120_F;
    
    LiquidManager* liquidman = new LiquidManager(x, y, timeStep);

    // DirectX init
    DX12App* dxapp = new DX12App();
    dxapp->setCameraProperties(
        PROJ::ORTHOGRAPHIC, 
        static_cast<float>(max_element(liquidman->iGetObjectCount())) * 0.0023f, // orthogonal distance
        2.0f, 0.0f, 0.0f                                                       // radius, theta, phi
    );
    //dxapp->setCameraProperties(PROJ::PERSPECTIVE, 0.0f, 1.5f, 0.0f, 0.0f);
    dxapp->setBackgroundColor(DirectX::Colors::LightSlateGray);

    // Window init
    Win32App winApp(500, 500);
    winApp.setWinName(L"2D Fluid Simulation");
    winApp.initialize(hInstance, dxapp, liquidman);

    return winApp.run();
}