# GridLiquid2D
## Overview
![demo](docs/images/demo.gif)

GridLiquid2D is a liquid simulation for Eulerian and PIC/FLIP. This simulation is rendered using <A href="https://github.com/frostsim/DXViewer">DXViewer</A>.

## Build
This repo was developed in the following environment:
* Windows 10 64-bit
* Microsoft Visual Studio 2019 on x64 platform (C++14)
* CMake 3.19.0

You should update submodules before creating a project with cmake.

```bash
git submodule update --progress --init -- "ext/DXViewer"
```

## Reference
* Kalle Bladin. _Fluid_Simulation_. https://github.com/kbladin/Fluid_Simulation
* Ryan Guy. _Blender-FLIP-Fluids_. https://github.com/rlguy/Blender-FLIP-Fluids
