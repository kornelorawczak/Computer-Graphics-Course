# OpenGL Helicopter Game

A 3D helicopter game where you fly to a volcano crater while avoiding deadly soap bubbles.

## Build Instructions

### Prerequisites
- CMake 3.10 or higher
- Visual Studio 2019+ or MinGW-w64 on Windows
- OpenGL 3.3 compatible graphics card

### Building
1. Extract the project to a directory
2. Run `buildRun.bat` from the project root
3. The script will:
   - Create build directory
   - Run CMake
   - Build in Release mode
   - Copy shaders to output directory
   - Run the executable

## Controls
- **W/S/A/D** or **Arrow Keys**: Move helicopter forward/back/left/right
- **Shift**: Increase speed (sprint)
- **Ctrl**: Reduce speed
- **Mouse Movement**: Look around (when in mouse-look mode)
- **Right Mouse Button**: Toggle mouse-look mode
- **Mouse Wheel**: Zoom in/out (adjust FOV)
- **TAB**: Cycle view modes (first-person, free camera, overview)
- **T**: Toggle bubble transparency
- **ESC**: Exit game

## Gameplay
- Fly your helicopter to the volcano crater
- Avoid rising soap bubbles that burst and release deadly gas
- Each level increases bubble spawn rate and speed
- Game ends when you reach the crater, die, or press ESC

## Technical Implementation

### Volcano Mesh Generation
The volcano is generated as a hyperboloid of revolution using the parametric equations:
x = (baseRadius + (craterRadius - baseRadius) * (y/height)) * cos(θ)
z = (baseRadius + (craterRadius - baseRadius) * (y/height)) * sin(θ)
y = [0, height]

Where:
- Base radius: 2000m at y=0
- Crater radius: 500m at y=3000m
- Height: 3000m

### Bubble Lifecycle
- Spawn: Random positions on terrain surface
- Growth: Radius increases by growthRate * dt each frame
- Rise: Position.y increases by riseSpeed * dt each frame
- Burst: When radius >= 2× initial radius
- Gas Effect: 10× burst radius lethal zone

### Collision Detection
- **Player vs Bubble**: Sphere-sphere intersection using helicopter collision spheres
- **Player vs Terrain**: ClosestPointTriangle algorithm for precise triangle collision
- **Resolution**: Move player to surface normal offset when underground

### Transparency & Instancing
**Transparency Algorithm:**
1. Render all opaque objects first
2. Sort transparent bubbles by distance to camera (far to near)
3. Enable blending: `glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)`
4. Disable depth writes: `glDepthMask(GL_FALSE)`
5. Render sorted transparent objects
6. Re-enable depth writes: `glDepthMask(GL_TRUE)`

**Instancing Implementation:**
- Single sphere mesh shared across all bubbles
- Dynamic VBO containing per-instance attributes:
  - vec3 position (12 bytes)
  - float scale (4 bytes)  
  - vec3 color (12 bytes)
  - float alpha (4 bytes)
  - **Total per instance: 32 bytes**

### Data Transfer Analysis

**Per-Frame GPU Transfers:**
- Camera matrices (view + projection): 2 × mat4 = 128 bytes
- For 500 bubbles: 500 × 32 bytes = 16,000 bytes
- **Total per frame: ~16.1 KB**

**Optimizations:**
- Static geometry uploaded once at initialization
- Instance VBO updated only when bubbles spawn, die, or move significantly
- Use `glBufferSubData` for partial updates
- Camera UBO shared across shaders

### Additional Effects

**Specular Reflections:**
- Phong specular term computed per-fragment
- Enhanced bubble reflections using Fresnel approximation
- Environment mapping using sky color

**Fog Implementation:**
- Beer's Law: `fogFactor = 1.0 - exp(-density * distance)`
- Exponential falloff for realistic atmospheric scattering

**Minimap Rendering:**
- Orthographic projection of world from top-down view
- Color coding for terrain elevation
- Player and bubble positions shown as icons

## Known Limitations
- Collision detection uses simplified sphere approximation for helicopter
- Bubble sorting is O(n log n) per frame - could use spatial partitioning
- No level progression persistence
- Basic particle effects for bubble bursts

## Future Improvements
- Implement spatial partitioning for faster bubble sorting
- Add texture mapping for terrain and objects
- Implement sound effects and background music
- Add more sophisticated particle systems
- Implement level progression saving
- Add more complex helicopter physics