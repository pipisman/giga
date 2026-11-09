#pragma once
#include <raylib.h>
#include <raymath.h>
#include <vector>

struct particle {
  float scale = 1;
  float scalingFactor = 0;
  // ot 0 kum 1
  float velocityFactor = 0;
  float life;
  Vector3 position;
  Vector3 velocity;
  Model *model;

  void update() {
    position += Vector3Scale(velocity, GetFrameTime());
    life -= GetFrameTime();
    if (scale > 0) {
      scale += scalingFactor * GetFrameTime();
    } else {
      scale = 0;
    }
    velocity = Vector3Scale(velocity, velocityFactor);
  }

  void draw() { DrawModel(*model, position, scale, WHITE); }
};
class particleManager {
private:
  float timer = 0;
  float counter;

public:
  // tuk se spawnvat
  Vector3 origin;
  bool enabled = false;
  // posoka na koito se nasochvat particulite
  Vector3 direction;
  int emissionPerSecond;
  float accuracy = 0;
  float plife;
  float speed;
  Model *pModel;
  std::vector<particle> particles = std::vector<particle>();
  float scalingFactor = 0;
  float velocityFactor = 0;
  // accuracy e obratno aka 0 e naj tochno 10 e ujasno netochno
  // position -- da se updateva wseki frame
  // direction -- sushto
  particleManager(Vector3 position, int emissionPerSecond, Vector3 direction,
                  float accuracy, float plife, float speed, Model *pModel) {
    this->origin = position;
    this->emissionPerSecond = emissionPerSecond;
    this->direction = direction;
    this->accuracy = accuracy;
    this->plife = plife;
    this->speed = speed;
    this->pModel = pModel;
  }

  virtual void spawnParticle() {
    particle p;
    // tuj kvoto prai e wzima ili 0 ili 1 izvajda 0.5 i go umnojava po 2 da
    // stane ili -1 ili 1 da opredeli posokata na particela float sign = 2 *
    // ((float)(rand() % 2) - 0.5f);

    // fuck this nqma da si pravq truda da napisha kakvo pravi
    // float vX = direction.x * speed + direction.x * 0.01 * accuracy * (rand()
    // % 20) * sign; sign = 2 * ((float)(rand() % 2) - 0.5f); float vY =
    // direction.y * speed + direction.y * 0.01 * accuracy * (rand() % 20) *
    // sign; sign = 2 * ((float)(rand() % 2) - 0.5f); float vZ = direction.z *
    // speed + direction.z * 0.01 * accuracy * (rand() % 20) * sign;
    Vector3 baseVelocity = Vector3Scale(direction, speed);

    float jitterScale = accuracy * 0.5f;

    float rx = ((float)rand() / RAND_MAX - 0.5f) * jitterScale;
    float ry = ((float)rand() / RAND_MAX - 0.5f) * jitterScale;
    float rz = ((float)rand() / RAND_MAX - 0.5f) * jitterScale;

    Vector3 pDirection = Vector3Add(baseVelocity, (Vector3){rx, ry, rz});
    p.life = plife;
    p.velocity = pDirection;
    p.model = pModel;
    p.position = origin;
    p.scalingFactor = scalingFactor;
    particles.push_back(p);
  }

  virtual void update() {
    // ot pred nazad za da ne dade segmentation fault kato loopva element kojto
    // ne sushtestvuva
    for (int i = particles.size() - 1; i >= 0; i--) {
      particles[i].update();
      if (particles[i].life <= 0) {
        particles.erase(particles.begin() + i);
      }
    }

    if (timer > 0) {
      timer--;
      if (timer == 0)
        enabled = false;
    }

    if (enabled) {
      counter += GetFrameTime();
      if (counter >= 1.0f / (float)emissionPerSecond) {
        spawnParticle();
        counter = 0;
      }
    }
  }

  virtual void draw() {
    for (int i = 0; i < particles.size(); i++) {
      particles[i].draw();
    }
  }

  void start() { enabled = true; }

  void stop() { enabled = false; }

  void startSeconds(float t) {
    enabled = true;
    this->timer = t;
  }
};
