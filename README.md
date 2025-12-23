# Computer-Graphics-Advanced-Course
This repository contains a collection of projects developed for the **Advanced Computer Graphics** course. The projects demonstrate various graphics rendering techniques, physics simulations, and game logic implementations using **OpenGL** and **C++**.

The codebase includes three distinct applications ranging from 2D geometry manipulation to complex 3D environments with advanced lighting and particle systems.

## 🛠 Technologies

* **Language:** C++
* **Graphics API:** OpenGL
* **Build System:** CMake
* **Dependencies:** All necessary libraries (GLFW, GLAD, GLM, etc.) are bundled within the repository.

---

## 🚀 Getting Started

### Prerequisites
* **CMake:** Ensure CMake is installed and added to your system PATH.
* **C++ Compiler:** A compatible C++ compiler (e.g., MSVC, MinGW).

### Installation & Execution
This repository is designed to be self-contained. You do not need to manually install external libraries.

1.  Clone the repository:
    ```bash
    git clone "https://github.com/kornelorawczak/Computer-Graphics-Course.git"
    cd Computer-Graphics-Course
    ```

2.  Run the automated build script in the selected project directory:
    ```cmd
    ./buildRun.bat
    ```
    *This script configures the project using CMake, builds the executables, and launches the application.*

---

## 📂 Projects Overview

### 1. Labirynt2D
**Description:**
A fundamental 2D maze game focused on 2D transformations and collision logic. The player must navigate through a generated maze to reach a specific objective.

**Key Features:**
* **Dynamic Obstacles:** Logic for randomly placing and rotating obstacles within the game world.
* **Player Control:** Full 2D movement system.
* **Objective System:** Defined win-conditions upon reaching the target.

### 2. Labirynt3D
**Description:**
A fully immersive 3D maze exploration game. This project expands on the 2D concepts by introducing perspective projection, texture mapping, and first-person camera control.

**Key Features:**
* **FPS Controller:** Immersive First-Person Shooter style camera movement.
* **Minimap System:** Features a real-time minimap with two distinct viewpoints for navigation assistance.
* **Rendering:** Implementation of texture mapping on walls and various obstacles.
* **Gameplay Loop:** Collectible coins and obstacle avoidance.

### 3. Volcano (Survival Simulation)
**Description:**
The most advanced project in the collection. A 3D survival game spanning 5 levels where the player pilots a helicopter. The goal is to reach the volcano crater while avoiding environmental hazards. This project demonstrates advanced lighting and particle behavior.

**Key Features:**
* **Helicopter Physics:** Controls designed to simulate helicopter movement.
* **Particle Systems:** "Toxic bubbles" that are randomly generated and exhibit realistic physical behavior/movement.
* **Advanced Lighting:**
    * Global ambient and diffuse lighting.
    * Spotlight implementation (Flashlight) attached to the player.
* **Atmospherics:** Implementation of distance fog for depth perception and atmosphere.
* **Camera Modes:** Toggleable First-Person (FPS) and Third-Person (TPS) perspectives.
* **UI/HUD:** Integrated minimap for navigation.

---
