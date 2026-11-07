#include "raylib.h"
#include "raymath.h"
#include "rcamera.h"
#include <algorithm>
#include <iostream>
#include <stdlib.h>

// modificiran kod ot camera.h
// hodi nalqvo i nadqsno
void CameraYaw2(Camera *camera, float angle) {
  // nagore
  Vector3 up = GetCameraUp(camera);

  // target poziciqta relativna kum camerata
  Vector3 targetPosition = Vector3Subtract(camera->target, camera->position);

  // zavurta se okolo y
  targetPosition = Vector3RotateByAxisAngle(targetPosition, up, angle);

  // wzima targeta i otiva nazade
  camera->position = Vector3Subtract(camera->target, targetPosition);
  // interpolaciq na rotaciqta (deprekirano, runva se v update)
  // rotation->y = Lerp(rotation->y, angle, 0.1f);
}

// modificiran kod ot rcamera.h
// pitch shte reche nagore i nadolu
void CameraPitch2(Camera *camera, float angle) {
  // up relativno kum kamerata
  // Vector3 up = GetCameraUp(camera);

  // globalen target vector
  Vector3 targetPosition = Vector3Subtract(camera->target, camera->position);

  // nadqsno relativno ot camerata
  Vector3 right = GetCameraRight(camera);

  // Rotate view vector around right axis
  targetPosition = Vector3RotateByAxisAngle(targetPosition, right, angle);

  camera->position = Vector3Subtract(camera->target, targetPosition);

  // interpolaciq na rotaciqta (deprekirano, runva se v update)
  // rotation->y = Lerp(rotation->y, angle, 0.1f);
}
// lerp funkciq koqto nqma da kara muhata da flipva kogato yaw stigne
// otricatelni stojnosti
float LerpAngle(float current, float target, float amount) {
  float diff = target - current;
  while (diff > PI)
    diff -= 2.0f * PI;
  while (diff < -PI)
    diff += 2.0f * PI;
  return current + diff * amount;
}

// edin takuv na client
class gigger {
public:
  Vector3 velocity;
  Vector3 position;
  Vector3 rotation;
  Model model;
  float cpitch = 0;
  // camerata si stoi v instancea na igracha
  Camera camera;
  const float falloff = 0.98f;
  gigger(Model &model) {
    this->model = model;
    DisableCursor();
    position = (Vector3){0.0f, 0.0f, 0.0f};
    rotation = (Vector3){0.0f, 0.0f, 0.0f};
    velocity = (Vector3){0.0f, 0.0f, 0.0f};

    camera.target = position;
    camera.position = (Vector3){0.0f, 2.5f, -6.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 80.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    // todo: zameni vgradenata funkciq sus custom UPDATECAMERAPRO69 kato
    // izpolzva pochti sushtata implementaciq kato CAMERA_THIRDPERSON
    // https://www.reddit.com/r/raylib/comments/v3q41l/full_camera_rotation/
    // note: napravih go veche

    // deprekirano
    // UpdateCamera(&camera, CAMERA_THIRD_PERSON);
  }

  void update() {

    Vector2 mouseDelta = GetMouseDelta();
    float sensitivity = 0.003f;

    // wzima pitch, suzdava promenliva i gi subira, clampva promenlivata,
    // izvajda gi i ostava samo clampnatata promqna
    float targetPitch = cpitch - (mouseDelta.y * sensitivity);
    targetPitch = Clamp(targetPitch, DEG2RAD * -80, DEG2RAD * 80);
    float pitchDiff = targetPitch - cpitch;
    cpitch = targetPitch;

    CameraYaw2(&camera, -mouseDelta.x * sensitivity);
    CameraPitch2(&camera, pitchDiff);
    // NAPRAVI: napravi da moje da se obrushta kamerata kato na third person i
    // da se interpolira rotaciqta na modela s tazi na kamerata pomoshti
    // promenlivi za tuj Vector3 up = {0, 1, 0};
    Vector3 forward =
        Vector3Normalize(Vector3Subtract(camera.target, camera.position));
    // Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, up));
    // camerata vinagi e oburnata kum camera.target
    // moje bi go napravi da se obrushta modela bazirano na razlikata mejdu
    // vectora ot predishniq frame i ot tozi? mahni updatecamera i polzvaj chast
    // ot logikata deto se polzva v rcamera.h

    // updateva se poziciqta wseki frame kakto i targeta

    // vurti camerata

    // wzima posokata na kamerata i go dobavq kum skorosta
    // forward.y = 0.0f;
    forward = Vector3Normalize(forward);

    // velocity handling
    if (IsKeyDown(KEY_W))
      velocity = Vector3Add(velocity, forward);
    velocity = Vector3Scale(velocity, falloff);
    float dt = GetFrameTime();
    position =
        Vector3Add(position, Vector3Multiply(velocity, (Vector3){dt, dt, dt}));

    // premestva kamerata nazad i slaga positiona na fly-a kato target
    Vector3 offset = Vector3Subtract(camera.position, camera.target);
    camera.target = position;
    camera.position = Vector3Add(camera.target, offset);

    // atan2f e kato vector kum gradusi direktno samo trqbwa da se convertne kum
    // radiani
    float targetAngleYaw = atan2f(forward.x, forward.z);

    // interpolira se v update za razlika ot v yaw() i pitch()
    // std::cout << "predi: " << rotation.y << "\n";
    // rotation.y = Lerp(rotation.y, targetAngleYaw, 0.2f);
    // std::cout << "sled: " << rotation.y << "\n";

    // Use LerpAngle to handle wrap-around smoothly without flipping
    rotation.y = LerpAngle(rotation.y, targetAngleYaw, 0.2f);
  }
  void draw() {
    // rotation.y -= 3.14 / 2;
    //
    // // wseki frame applyva rotaciqta kum transforma
    // model.transform = MatrixRotateXYZ(rotation);

    // Apply the offset directly to the matrix transformation without mutating
    // rotation.y state
    Vector3 adjustedRotation = {rotation.x, rotation.y - (PI / 2.0f),
                                rotation.z};
    model.transform = MatrixRotateXYZ(adjustedRotation);

    DrawModel(model, position, 1.0f, WHITE);

    DrawCube(position, 1.0f, 1.0f, 1.0f, RED);
    DrawCubeWires(position, 1.0f, 1.0f, 1.0f, BLACK);
  }
  void drawui() {}
};

int main(void) {
  const int screenWidth = 800;
  const int screenHeight = 450;

  InitWindow(screenWidth, screenHeight, "gigger");

  Model model = LoadModel("Fly/Fly_centered.obj");
  Texture2D texture = LoadTexture("Fly/images/Fly.png");
  model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;

  BoundingBox bounds = GetMeshBoundingBox(model.meshes[0]);

  SetTargetFPS(60);

  gigger giga = gigger(model);
  while (!WindowShouldClose()) {
    // callvat se gigger funkciite
    giga.update();

    BeginDrawing();

    ClearBackground(RAYWHITE);

    BeginMode3D(giga.camera);

    giga.draw();
    DrawGrid(40, 1.0f); // grid
    EndMode3D();
    giga.drawui();
    DrawFPS(10, 10);

    EndDrawing();
  }

  UnloadTexture(texture);
  UnloadModel(model);

  CloseWindow();

  return 0;
}
