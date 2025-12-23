# 3D Maze Game

A single-player 3D maze game built with OpenGL and C++17. Navigate through a cube filled with rotating octahedron obstacles to reach the goal!

## Features

- First-person camera with mouse look controls
- Procedurally generated maze with configurable grid size
- Collision detection with obstacles and walls
- Color-coded obstacles for spatial orientation
- Centered square viewport that maintains aspect ratio

## Build Instructions

### Prerequisites
- CMake 3.10 or higher
- Visual Studio 2019 or later (or MinGW on Windows)

### Building the Project

1. **Using the provided batch script (recommended):**
   ```bash
   buildRun.bat [seed] [N]