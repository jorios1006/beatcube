# Sound Cube

## System Requirements
- **Language:** C (C99 or later for designated initializers)
- **Libraries:** raylib, raymath, math.h, rlights.h
- **Assets:** 
  - `resources/audio/metronome.mp3` (must exist in directory)
  - `resources/shaders/lighting.vs`
  - `resources/shaders/lighting.fs`
  - `include/rlights.h`

## Configuration Constants
| Constant | Value | Description |
|----------|-------|-------------|
| `SCREENWIDTH` | 720 | Window width in pixels |
| `SCREENHEIGHT` | 400 | Window height in pixels |
| `TARGETFPS` | 60 | Target frames per second |
| `LANE_WIDTH` | 2.0f | Distance between lanes in world units |
| `LANE_AMOUNT` | 3 | Total number of lanes |
| `PLANE_WIDTH` | `LANE_WIDTH * LANE_AMOUNT` | Floor plane width |
| `PLANE_LENGHT` | `SCREENHEIGHT * (2.0f / 3.0f)` | Floor plane length |
| `HALF_LANE_WIDTH` | `LANE_WIDTH / 2.0f` | Half lane width for boundary lines |
| `SMOOTHING_SPEED` | 5.0f | Lane interpolation speed |
| `BPM` | 120.0f | Beats per minute for music synchronization |
| `BEAT_DURATION` | `60.0f / BPM` (0.5s) | Duration of one beat in seconds |

## Color Palette (OEL Theme)
| Constant | RGB Values | Usage |
|----------|------------|-------|
| `OEL_BG` | (4, 12, 24) | Background color |
| `OEL_DIM` | (0, 80, 120) | UI borders |
| `OEL_MID` | (0, 180, 255) | Player cube |
| `OEL_BRIGHT` | (180, 240, 255) | Cube wires, floor |

## Data Structures

### GameState
| Field | Type | Description |
|-------|------|-------------|
| `lane` | int | Current logical lane index (-1, 0, 1) |
| `visualX` | float | Interpolated X position for rendering |
| `speed` | float | **Unused** (reserved for future gameplay) |
| `distance` | float | **Unused** (reserved for future gameplay) |
| `beatTimer` | float | Time elapsed within current beat cycle |
| `cubeScale` | float | Current scale multiplier based on beat |

### KeyboardState
| Field | Type | Description |
|-------|------|-------------|
| `W, S, A, D` | bool | WASD key states |
| `LEFT_CONTROL` | bool | Left Control key state |
| `X, C, Z, Q` | bool | Additional action keys |
| `LEFT, RIGHT` | bool | Arrow key states |

## Execution Flow

### Initialization
1. Initialize Window (720x400)
2. Set Config Flags: `FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT`
3. Set Target FPS to 60
4. Configure Camera3D
   - Position: (0, 2, 5)
   - Target: (0, 1, 0)
   - Up: (0, 1, 0)
   - FOV: 45
   - Projection: Perspective
5. Initialize GameState (`cubeScale`: 1.0)
6. Initialize Audio Device
7. Load Music Stream (`resources/audio/metronome.mp3`)
   - **On Failure:** Goto `CLEAN`
8. Play Music Stream
9. Load Shader (`resources/shaders/lighting.vs`, `lighting.fs`)
   - **On Failure:** Goto `CLEAN`
10. Configure Shader Lighting
    - Set ambient light values (0.2, 0.2, 0.2, 1.0)
    - Create point light source at (0, 4, 0)

### Main Loop (Label: BEGIN)
1. **Check Exit Condition**
   - If `WindowShouldClose()` → Goto `CLEAN`
2. **Input Handling**
   - Poll keyboard state into `KeyboardState` struct
   - If (LEFT or A) AND `lane` > -1 → Decrement `lane`
   - If (RIGHT or D) AND `lane` < 1 → Increment `lane`
3. **Camera Follow**
   - Lerp `camera.position.x` toward `gs.visualX` (factor: 0.1)
   - Lerp `camera.target.x` toward `gs.visualX` (factor: 0.1)
4. **Audio & Beat Logic**
   - Update Music Stream
   - Get `timePlayed`
   - Calculate `beatTimer` = `timePlayed` % `BEAT_DURATION`
   - Calculate `sting` = 1.0 - (`beatTimer` / `BEAT_DURATION`)
   - Update `cubeScale` = `(1.0 + (sting * 0.5)) * 0.5`
5. **Physics**
   - Calculate `lerpAmount` = `SMOOTHING_SPEED * GetFrameTime()` (capped at 1.0)
   - Update `visualX` = Lerp(current, `lane * LANE_WIDTH`, `lerpAmount`)
   - *Frame-independent interpolation*
6. **Shader Updates**
   - Update camera position in shader
   - Update light values via `UpdateLightValues()`
7. **Rendering**
   - Begin Drawing
   - Clear Background (`OEL_BG`)
   - Begin 3D Mode
   - Begin Shader Mode
   - Draw Plane (`PLANE_WIDTH` x `PLANE_LENGHT`, `OEL_BRIGHT`)
   - Draw Lane Lines (`GRAY`) at x = +/- `HALF_LANE_WIDTH`
   - Calculate `cubePos` (x: `visualX`, y: `cubeScale * 0.5`, z: 0)
   - Draw Cube (`OEL_MID`) scaled by `cubeScale`
   - Draw Cube Wires (`OEL_BRIGHT`) scaled by `cubeScale`
   - End Shader Mode
   - End 3D Mode
   - Draw UI Border (`OEL_DIM`)
   - Draw Title: "PONYONEER GRAPHIC ENGINE v1.0" (`OEL_MID`)
   - Draw FPS Counter
   - End Drawing
8. **Loop Control**
   - Goto `BEGIN` (Intentional infinite loop structure)

### Termination (Label: CLEAN → EXIT)
1. **CLEAN Section** (Resource Deallocation)
   - Unload Music Stream (if valid via `IsMusicValid()`)
   - Unload Shader (if valid via `IsShaderValid()`)
   - Close Audio Device
2. **EXIT Section**
   - Close Window
   - Return 0

## Input Map
| Key | Action |
|-----|--------|
| A / LEFT Arrow | Move Lane Left (-1) |
| D / RIGHT Arrow | Move Lane Right (+1) |
| W, S, Q, Z, X, C, LCTRL | Polling active, no logic assigned (reserved) |

## Known Issues
| Issue | Status | Notes |
|-------|--------|-------|
| Beat synchronization relies on `GetMusicTimePlayed` starting at 0 | ️ Active | Does not account for audio latency or buffer delay |
| `GameState` fields `speed` and `distance` are defined but unused | ️ Active | Reserved for future gameplay mechanics |
| Camera follow may cause disorientation at lane extremes | ️ Active | Camera lerps but may not match player speed perfectly |

## Current Goals
| Goal | Priority | Status |
|------|----------|--------|
| Implement note spawning and collision detection | High |  Planned |
| Add audio analysis for dynamic beat detection instead of fixed BPM | High |  Planned |
| Expand lane system beyond 3 lanes | Medium |  Planned |
| Implement score tracking | Medium |  Planned |
| Add resource cleanup for audio streams explicitly | Low |  Complete |
| Fix frame-independent physics | Low |  Complete |
| Add camera follow system | Low |  Complete |

## Code Structure Notes
- **Goto Usage:** Intentional for cleanup flow (`BEGIN` → `CLEAN` → `EXIT`)
- **Resource Management:** All exit paths go through `CLEAN` to prevent leaks
- **Validation:** Resources checked with `IsMusicValid()`/`IsShaderValid()` before unload
- **Delta Time:** Physics calculations use `GetFrameTime()` for frame independence
- **Global State:** `camera`, `gs`, `kb` declared at global scope for accessibility
- **Lighting:** Uses `rlights.h` library for shader-based lighting system

## Build Instructions
```bash
gcc main.c -o soundcube -lraylib -lm -lopengl32 -lgdi32 -lwinmm
```

## Directory Structure
```
project/
├── main.c
├── README.md
├── resources/
│   ├── audio/
│   │   └── metronome.mp3
│   └── shaders/
│       ├── lighting.vs
│       └── lighting.fs
└── include/
    └── rlights.h
```
