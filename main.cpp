#include "raylib.h"
#include "raymath.h"
#include "rcamera.h"

// ima edin takuv na client
class gigger
{
public:
    Vector3 velocity;
    Vector3 position;
    Vector3 rotation;
    Model model;

    // camerata si stoi v instancea na igracha
    Camera camera;
    const float falloff = 0.98f;
    gigger(Model &model)
    {
        // modela se pasva ot main v sluchai ako polzvame muhata za drugi neshta

        camera.position = (Vector3){50.0f, 50.0f, 50.0f};
        camera.target = (Vector3){0.0f, 12.0f, 0.0f};
        camera.up = (Vector3){0.0f, 1.0f, 0.0f};
        camera.fovy = 80.0f;
        camera.projection = CAMERA_THIRD_PERSON;

        // todo: zameni vgradenata funkciq sus custom UPDATECAMERAPRO69 kato izpolzva pochti sushtata implementaciq
        // kato CAMERA_THIRDPERSON
        // https://www.reddit.com/r/raylib/comments/v3q41l/full_camera_rotation/

        UpdateCamera(&camera, CAMERA_PERSPECTIVE);
    }
    void update()
    {
        // NAPRAVI: napravi da moje da se obrushta kamerata kato na third person i da se interpolira rotaciqta na modela
        // s tazi na kamerata
        // pomoshti promenlivi za tuj
        Vector3 up = {0, 1, 0};
        Vector3 forward = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
        Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, up));
        // camerata vinagi e oburnata kum camera.target
        // moje bi go napravi da se obrushta modela bazirano na razlikata mejdu vectora ot predishniq frame i ot tozi?
        // mahni updatecamera i polzvaj chast ot logikata deto se polzva v rcamera.h

        // updateva se poziciqta wseki frame kakto i targeta

        camera.target = position;

        // wzima posokata na kamerata i go dobavq kum skorosta
        forward.y = 0.0f;
        forward = Vector3Normalize(forward);

        velocity = Vector3Add(velocity, forward);
        velocity = Vector3Scale(velocity, falloff);
        position = Vector3Add(position, velocity);
    }
    void draw()
    {
        // wseki frame applyva rotaciqta kum transforma
        model.transform = MatrixRotateXYZ(rotation);

        DrawModel(model, position, 1.0f, WHITE);
    }
    void drawui()
    {
    }
};
int main()
{
    // turi boilerplate coda ot modelload.cpp tuk i ne zabravqj da callnesh update() i draw()
}
