#include "raylib.h"

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [models] example - loading");

    // Define the camera to look into our 3d world
    Camera camera = {0};
    camera.position = (Vector3){50.0f, 50.0f, 50.0f}; // Camera position
    camera.target = (Vector3){0.0f, 12.0f, 0.0f};     // Camera looking at point
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                              // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;           // Camera mode type

    Model model = LoadModel("Fly/Fly.obj");                          // Load model
    Texture2D texture = LoadTexture("Fly/images/Fly.png");           // Load model texture
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture; // Set map diffuse texture

    Vector3 position = {0.0f, 0.0f, 0.0f}; // Set model position

    BoundingBox bounds = GetMeshBoundingBox(model.meshes[0]); // Set model bounds

    // NOTE: bounds are calculated from the original size of the model,
    // if model is scaled on drawing, bounds must be also scaled

    bool selected = false; // Selected object flag

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        UpdateCamera(&camera, CAMERA_ORBITAL);

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        DrawModel(model, position, 1.0f, WHITE); // Draw 3d model with texture

        if (selected)
            DrawBoundingBox(bounds, GREEN); // Draw selection box

        EndMode3D();

        DrawFPS(10, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(texture); // Unload texture
    UnloadModel(model);     // Unload model

    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}