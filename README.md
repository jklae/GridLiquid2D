# GridLiquids
## Overview
<img src="docs/images/demo.gif" width="80%" height="80%">
GridLiquids is a liquid simulation for Eulerian and PIC/FLIP. This simulation is rendered using DXViewer. For more details about DXViewer, see <A href="https://github.com/frostsim/DXViewer">here</A>.

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
