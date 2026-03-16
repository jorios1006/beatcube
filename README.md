# Sound Cube

## System Requirements
- **Language:** C (C99 or later for designated initializers)
- **Libraries:** raylib, raymath, math.h
- **Assets:** `metronome.mp3` (must exist in `resources/audio/` directory)

## Configuration Constants
| Constant | Value | Description |
|----------|-------|-------------|
| `SCREENWIDTH` | 720 | Window width in pixels |
| `SCREENHEIGHT` | 400 | Window height in pixels |
| `TARGETFPS` | 60 | Target frames per second |
| `LANE_WIDTH` | 2.0f | Distance between lanes in world units |
| `BPM` | 120.0f | Beats per minute for music synchronization |
| `beatDuration` | 60.0 / BPM (0.5s) | Duration of one beat in seconds |
| `SMOOTHING_SPEED` | 5.0f | Lane interpolation speed (frame-independent) |

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
2. Set Target FPS to 60
3. Configure Camera3D
   - Position: (0, 2, 5)
   - Target: (0, 1, 0)
   - Up: (0, 1, 0)
   - FOV: 45
   - Projection: Perspective
4. Initialize GameState
   - `cubeScale`: 1.0
5. Initialize Audio Device
6. Load Music Stream (`resources/audio/metronome.mp3`)
   - **On Failure:** Goto `CLEAN`
7. Play Music Stream
8. Load Shader (`resources/shaders/lighting.vs`, `lighting.fs`)
   - **On Failure:** Goto `CLEAN`
9. Configure Shader Lighting
   - Set ambient light values
   - Create point light source

### Main Loop (Label: BEGIN)
1. **Check Exit Condition**
   - If `WindowShouldClose()` → Goto `CLEAN`
2. **Input Handling**
   - Poll keyboard state into `KeyboardState` struct
   - If (LEFT or A) AND `lane` > -1 → Decrement `lane`
   - If (RIGHT or D) AND `lane` < 1 → Increment `lane`
3. **Audio & Beat Logic**
   - Update Music Stream
   - Get `timePlayed`
   - Calculate `beatTimer` = `timePlayed` % `beatDuration`
   - Calculate `sting` = 1.0 - (`beatTimer` / `beatDuration`)
   - Update `cubeScale` = 1.0 + (`sting` * 0.5)
4. **Physics**
   - Update `visualX` = Lerp(current, `lane` * `LANE_WIDTH`, `SMOOTHING_SPEED * GetFrameTime()`)
   - *Frame-independent interpolation*
5. **Shader Updates**
   - Update camera position in shader
   - Update light values
6. **Rendering**
   - Begin Drawing
   - Clear Background (BLACK)
   - Begin 3D Mode
   - Begin Shader Mode
   - Draw Plane (10x100, DARKGRAY)
   - Draw Lane Lines (GRAY) at x = +/- `LANE_WIDTH`/2
   - Calculate `cubePos` (x: `visualX`, y: `cubeScale` * 0.5, z: 0)
   - Draw Cube (BLUE) scaled by `cubeScale`
   - Draw Cube Wires (WHITE) scaled by `cubeScale`
   - End Shader Mode
   - End 3D Mode
   - Draw FPS Counter
   - End Drawing
7. **Loop Control**
   - Goto `BEGIN` (Intentional infinite loop structure)

### Termination (Label: CLEAN → EXIT)
1. **CLEAN Section** (Resource Deallocation)
   - Unload Music Stream (if valid)
   - Unload Shader (if valid)
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
| Filename typo: `metrome.mp3` vs `metronome.mp3` | ️ Fixed | Verify correct filename in resources directory |
| Camera is static at x=0 while player moves along X axis | ️ Fixed? | Player may move off-screen at lane extremes |
| No resource validation before unloading in cleanup |  Fixed | Now checks `IsMusicValid`/`IsShaderValid` before unload |
| Audio device not closed on exit |  Fixed | `CloseAudioDevice()` added to CLEAN section |
| Frame-rate dependent smoothing |  Fixed | Now uses `GetFrameTime()` for delta-time interpolation |

## Current Goals
| Goal | Priority | Status |
|------|----------|--------|
| Implement note spawning and collision detection | High |  Planned |
| Add audio analysis for dynamic beat detection instead of fixed BPM | High |  Planned |
| Expand lane system beyond 3 lanes | Medium | Planned |
| Implement score tracking | Medium |  Planned |
| Add camera follow system for player X position | Medium |  Planned |
| Add resource cleanup for audio streams explicitly | Low |  Complete |
| Fix frame-independent physics | Low |  Complete |

## Code Structure Notes
- **Goto Usage:** Intentional for cleanup flow (`BEGIN` → `CLEAN` → `EXIT`)
- **Resource Management:** All exit paths go through `CLEAN` to prevent leaks
- **Validation:** Resources checked with `IsMusicValid()`/`IsShaderValid()` before unload
- **Delta Time:** Physics calculations use `GetFrameTime()` for frame independence

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
