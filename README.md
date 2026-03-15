# Sound Cube

## System Requirements
- **Language:** C (C99 or later for designated initializers)
- **Libraries:** raylib, raymath, math.h
- **Assets:** `metrome.mp3` (must exist in working directory)

## Configuration Constants
- `SCREENWIDTH`: 720
- `SCREENHEIGHT`: 400
- `TARGETFPS`: 60
- `LANE_WIDTH`: 2.0f
- `BPM`: 120.0f
- `beatDuration`: 60.0 / BPM (0.5s)

## Data Structures

### GameState
- `int lane`: Current logical lane index (-1, 0, 1)
- `float visualX`: Interpolated X position for rendering
- `float speed`: Unused
- `float distance`: Unused
- `float beatTimer`: Time elapsed within current beat cycle
- `float cubeScale`: Current scale multiplier based on beat

### KeyboardState
- Boolean flags for keys: W, S, A, D, LEFT_CONTROL, X, C, Z, Q, LEFT, RIGHT

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
6. Load Music Stream ("metrome.mp3")
7. Play Music Stream
8. Calculate initial beat state (unused variable `currentBeat`)

### Main Loop (Label: BEGIN)
1. **Check Exit Condition**
   - If `WindowShouldClose()` -> Goto END
2. **Input Handling**
   - Poll keyboard state into `KeyboardState` struct 
   - If (LEFT or A) AND `lane` > -1 -> Decrement `lane`
   - If (RIGHT or D) AND `lane` < 1 -> Increment `lane`
3. **Audio & Beat Logic**
   - Update Music Stream
   - Get `timePlayed`
   - Calculate `beatTimer` = `timePlayed` % `beatDuration`
   - Calculate `sting` = 1.0 - (`beatTimer` / `beatDuration`)
   - Update `cubeScale` = 1.0 + (`sting` * 0.5)
4. **Physics**
   - Update `visualX` = Lerp(current, `lane` * `LANE_WIDTH`, 0.2)
5. **Rendering**
   - Begin Drawing
   - Clear Background (BLACK)
   - Begin 3D Mode
   - Draw Plane (10x100, DARKGRAY)
   - Draw Lane Lines (GRAY) at x = +/- `LANE_WIDTH`/2
   - Calculate `cubePos` (x: `visualX`, y: `cubeScale` * 0.5, z: 0)
   - Draw Cube (BLUE) scaled by `cubeScale`
   - Draw Cube Wires (WHITE) scaled by `cubeScale`
   - End 3D Mode
   - Draw FPS Counter
   - End Drawing
6. **Loop Control**
   - Goto BEGIN (Intentional infinite loop structure)

### Termination (Label: END)
1. Close Window
2. Return 0

## Input Map
- **A / LEFT Arrow**: Move Lane Left (-1)
- **D / RIGHT Arrow**: Move Lane Right (+1)
- **Other Keys**: Polling not active, no logic assigned (yet) (W, S, Q, Z, X, C, LCTRL)

## Known Issues
- Music file path is hardcoded ("metrome.mp3"). Missing file causes runtime failure.
- `UnloadMusicStream` is not called before exit; relies on OS cleanup.
- Beat synchronization relies on `GetMusicTimePlayed` starting at 0; does not account for audio latency or buffer delay.
- [FIXED] `KeyboardState` struct allocates memory on the stack every frame despite being temporary.
- `GameState` fields `speed` and `distance` are defined but unused.
- Variable `currentBeat` calculated during initialization is never used.

## Current Goals
- Implement note spawning and collision detection.
- Add audio analysis for dynamic beat detection instead of fixed BPM.
- Expand lane system beyond 3 lanes.
- Implement score tracking.
- Add resource cleanup for audio streams explicitly.
