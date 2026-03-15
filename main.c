#include "raylib.h"
#include "raymath.h"
#include <math.h>
#define SCREENHEIGHT 400
#define SCREENWIDTH 720
#define TARGETFPS 60
#define LANE_WIDTH 2.0f
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

int main(void) {
  const float BPM = 120.0f;
  const float beatDuration = 60.0f / BPM; // 0.5s for 120 BPM
  InitWindow(SCREENWIDTH, SCREENHEIGHT, "window");
  SetTargetFPS(TARGETFPS);
  Camera3D camera = {0};
  camera.position = (Vector3){0.0f, 2.0f, 5.0f};
  camera.target = (Vector3){0.0f, 1.0f, 0.0f};
  camera.up = (Vector3){0.0f, 1.0f, 0.0f};
  camera.fovy = 45.0f;
  camera.projection = CAMERA_PERSPECTIVE;
  // -- GAME STATE --
  GameState gs = {0};
  gs.cubeScale = 1.0f;
  // -- AUDIO ---
  InitAudioDevice(); // Initialize audio hardware
  Music music = LoadMusicStream("metrome.mp3");
  PlayMusicStream(music);
  float timePlayed = GetMusicTimePlayed(music);
  // If your song is 120 BPM, one beat happens every 0.5 seconds
  float currentBeat = (timePlayed * 120.0f) / 60.0f;
BEGIN:
  if (WindowShouldClose())
    goto END;
  //------- KEYBOARD --------//
  KeyboardState kb = {
      .W = IsKeyDown(KEY_W),
      .S = IsKeyDown(KEY_S),
      .A = IsKeyDown(KEY_A),
      .D = IsKeyDown(KEY_D),
      .LEFT_CONTROL = IsKeyDown(KEY_LEFT_CONTROL),
      .X = IsKeyDown(KEY_X),
      .C = IsKeyDown(KEY_C),
      .Z = IsKeyDown(KEY_Z),
      .Q = IsKeyDown(KEY_Q),
      .LEFT = IsKeyDown(KEY_LEFT),
      .RIGHT = IsKeyDown(KEY_RIGHT),
  };
  if ((kb.LEFT || kb.A) && gs.lane > -1)
    gs.lane--;
  if ((kb.RIGHT || kb.D) && gs.lane < 1)
    gs.lane++;
  //------- AUDIO & BEAT LOGIC -----//
  UpdateMusicStream(music);
  timePlayed = GetMusicTimePlayed(music);
  gs.beatTimer = fmodf(timePlayed, beatDuration);
  float sting = 1.0f - (gs.beatTimer / beatDuration);
  gs.cubeScale = 1.0f + (sting * 0.5f); // Grows by 50% on the beat
  //------- PHYSICS --------/
  gs.visualX = Lerp(gs.visualX, (float)gs.lane * LANE_WIDTH, 0.2f);

  //------- DRAWING --------//
  BeginDrawing();
  ClearBackground(BLACK);
  //------- 3D --------//
  BeginMode3D(camera);
  // FLOOR
  DrawPlane((Vector3){0, 0, 0}, (Vector2){10, 100}, DARKGRAY);
  // LANES
  DrawLine3D((Vector3){-LANE_WIDTH / 2, 0.01f, 50},
             (Vector3){-LANE_WIDTH / 2, 0.01f, -50}, GRAY);
  DrawLine3D((Vector3){LANE_WIDTH / 2, 0.01f, 50},
             (Vector3){LANE_WIDTH / 2, 0.01f, -50}, GRAY);
  // PLAYER
  Vector3 cubePos = {gs.visualX, (gs.cubeScale * 0.5f), 0.0f};
  DrawCube(cubePos, gs.cubeScale, gs.cubeScale, gs.cubeScale, BLUE);
  DrawCubeWires(cubePos, gs.cubeScale, gs.cubeScale, gs.cubeScale, WHITE);
  EndMode3D();
  DrawFPS(10, 10);
  EndDrawing();
  goto BEGIN;
END:
  CloseWindow();
  return 0;
}
