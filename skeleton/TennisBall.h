#pragma once
#include "Projectile.h"

class TennisBall : public Projectile {
public:
  TennisBall(const PxVec3& pos,
             const PxVec3& vel,
             float mass = 0.057f,
             float damping = 0.96f,
             float radius = 0.2f,
             const Vector4& color = Vector4(0.9f, 1.0f, 0.4f, 1.0f))
    : Projectile(pos, vel, mass, damping, radius, color)
  {
    scalePhysics(0.6f);
  }

  void launch(const PxVec3& direction, float power) override
  {
    velocity = direction.getNormalized() * (power * 20.0f);
    clearForceAccumulator();
    inPlay = true;
  }
};
