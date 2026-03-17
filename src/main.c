#include "raylib.h"
#include "raymath.h"
#include <math.h>
#define RLIGHTS_IMPLEMENTATION
#include "../include/rlights.h"

/*----  The intention of this constants are to be use as fallback  ----*/
/*----            Right now they are just placeholders             ----*/
#define SCREENHEIGHT 400
#define SCREENWIDTH 720
#define TARGETFPS 60
#define LANE_WIDTH 2.0f
#define LANE_AMOUNT 3
#define PLANE_WIDTH LANE_WIDTH * LANE_AMOUNT
#define HALF_LANE_WIDTH LANE_WIDTH / 2.0f
#define SMOOTHING_SPEED 5.0f
#define BPM 120.0f
#define BEAT_DURATION 60.0f / BPM // 0.5s for 120 BPM
#define PLANE_LENGHT SCREENHEIGHT * (2.0f / 3.0f)
const Color OEL_BG = (Color){4, 12, 24, 255};
const Color OEL_DIM = (Color){0, 80, 120, 255};
const Color OEL_MID = (Color){0, 180, 255, 255};
const Color OEL_BRIGHT = (Color){180, 240, 255, 255};
typedef struct {
  int lane;      // -1, 0, 1
  float visualX; // For smooth sliding between lanes
  float speed;
  float distance;
  float beatTimer;
  float cubeScale;
} GameState;
typedef struct {
  bool W, S, A, D;
  bool LEFT_CONTROL;
  bool X, C, Z, Q;
  bool LEFT, RIGHT;
} KeyboardState;
Camera3D camera = {0};
GameState gs = {0};
KeyboardState kb = {0};
int main(void) {
  InitWindow(SCREENWIDTH, SCREENHEIGHT, "window");
  SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
  SetTargetFPS(TARGETFPS);
  camera.position = (Vector3){0.0f, 2.0f, 5.0f};
  camera.target = (Vector3){0.0f, 1.0f, 0.0f};
  camera.up = (Vector3){0.0f, 1.0f, 0.0f};
  camera.fovy = 45.0f;
  camera.projection = CAMERA_PERSPECTIVE;
  // -- GAME STATE --
  gs.cubeScale = 1.0f;
  // -- AUDIO ---
  InitAudioDevice(); // Initialize audio hardware
  Music music = LoadMusicStream("resources/audio/metronome.mp3");
  if (!IsMusicValid(music)) {
    TraceLog(LOG_ERROR, "Failed to load music");
    goto CLEAN; // Jump to CLEAN instead of EXIT
  }

  PlayMusicStream(music);
  float timePlayed = GetMusicTimePlayed(music);
  //--- SHADERS ---//
  Shader shader = LoadShader("resources/shaders/lighting.vs",
                             "resources/shaders/lighting.fs");
  if (!IsShaderValid(shader)) {
    TraceLog(LOG_ERROR, "Failed to load shader");
    goto CLEAN; // Jump to CLEAN instead of EXIT
  }
  shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
  int ambientLoc = GetShaderLocation(shader, "ambient");
  float ambient[4] = {0.2f, 0.2f, 0.2f, 1.0f};
  SetShaderValue(shader, ambientLoc, ambient, SHADER_UNIFORM_VEC4);
  Light sun = CreateLight(LIGHT_POINT, (Vector3){0, 4, 0}, Vector3Zero(), WHITE,
                          shader);
BEGIN:
  //------- KEYBOARD --------//
  kb.A = IsKeyDown(KEY_A);
  kb.D = IsKeyDown(KEY_D);
  kb.LEFT = IsKeyDown(KEY_LEFT);
  kb.RIGHT = IsKeyDown(KEY_RIGHT);
  if ((kb.LEFT || kb.A) && gs.lane > -1)
    gs.lane--;
  if ((kb.RIGHT || kb.D) && gs.lane < 1)
    gs.lane++;
  //------- CAMERA FOLLOW --------//
  // Smoothly move camera to follow player X
  camera.position.x = Lerp(camera.position.x, gs.visualX, 0.1f);
  camera.target.x = Lerp(camera.target.x, gs.visualX, 0.1f);
  //------- AUDIO & BEAT LOGIC -----//
  UpdateMusicStream(music);
  timePlayed = GetMusicTimePlayed(music);
  gs.beatTimer = fmodf(timePlayed, BEAT_DURATION);
  float sting = 1.0f - (gs.beatTimer / BEAT_DURATION);
  gs.cubeScale = (1.0f + (sting * 0.5f)) * 0.5f; // Grows by 50% on the beat
  //------- PHYSICS --------/
  float lerpAmount = SMOOTHING_SPEED * GetFrameTime();
  if (lerpAmount > 1.0f)
    lerpAmount = 1.0f; // Cap to prevent overshooting
  gs.visualX = Lerp(gs.visualX, (float)gs.lane * LANE_WIDTH, lerpAmount);

  // --- SHADERS ---
  // Update camera position in shader
  float camPos[3] = {camera.position.x, camera.position.y, camera.position.z};
  SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], camPos,
                 SHADER_UNIFORM_VEC3);

  // If you change light colors/positions based on music, call this:
  UpdateLightValues(shader, sun);

  //------- DRAWING --------//
  BeginDrawing();
  ClearBackground(OEL_BG);
  //------- 3D --------//
  BeginMode3D(camera);
  BeginShaderMode(shader); // <--- EVERYTHING between these two uses lighting
  // FLOOR
  DrawPlane((Vector3){0, 0, 0}, (Vector2){PLANE_WIDTH, PLANE_LENGHT}, OEL_BRIGHT);
  // LANES
  DrawLine3D((Vector3){-HALF_LANE_WIDTH, 0.01f, PLANE_LENGHT  / 2.0f},
             (Vector3){-HALF_LANE_WIDTH, 0.01f, -PLANE_LENGHT / 2.0f}, GRAY);
  DrawLine3D((Vector3){HALF_LANE_WIDTH, 0.01f, PLANE_LENGHT / 2.0f},
             (Vector3){HALF_LANE_WIDTH, 0.01f, -PLANE_LENGHT / 2.0f}, GRAY);
  // PLAYER
  Vector3 cubePos = {gs.visualX, (gs.cubeScale * 0.5f), 0.0f};
  DrawCube(cubePos, gs.cubeScale, gs.cubeScale, gs.cubeScale, OEL_MID);
  DrawCubeWires(cubePos, gs.cubeScale, gs.cubeScale, gs.cubeScale, OEL_BRIGHT);
  EndShaderMode();
  EndMode3D();
  DrawRectangleLines(5, 5, SCREENWIDTH - 10, SCREENHEIGHT - 10, OEL_DIM);
  DrawText("PONYONEER GRAPHIC ENGINE v1.0", 20, 20, 10, OEL_MID);
  DrawFPS(SCREENWIDTH - 80, 20);
  EndDrawing();

  if (WindowShouldClose())
    goto CLEAN;
  goto BEGIN;
CLEAN:
  if (IsMusicValid(music))
    UnloadMusicStream(music);
  if (IsShaderValid(shader))
    UnloadShader(shader);
  CloseAudioDevice();
EXIT:
  CloseWindow();
  return 0;
}
