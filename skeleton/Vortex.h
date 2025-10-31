#pragma once
#include "Wind.h"
#include "Particle.h"
#include "core.hpp"

class Vortex : public Wind {
public:
  Vortex(const Vector3& center_,
         float K_,
         float radius_,
         float k1_ = 0.2f,
         float k2_ = 0.0f)
    : Wind(Vector3(0, 0, 0), k1_, k2_),
 
    center(center_)
    , K(K_)
    , radius(radius_)
  {
  }
  virtual void updateForce(Particle* particle, double t) override
  {
    if (!particle || particle->getInverseMass() == 0)
      return;
    Vector3 pos = particle->getPos();
    float vx = -(pos.z - center.z);
    float vy = 50.0f - (pos.y - center.y);
    float vz = (pos.x - center.x);
    Vector3 localWind = Vector3(vx, vy, vz) * K;
    float dist = (pos - center).magnitude();
    if (dist > radius)
      return;
    setWindVelocity(localWind);
    Wind::updateForce(particle, t);
  }

  private:
  Vector3 center;
  float K;  
  float radius;
};