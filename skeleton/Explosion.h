#pragma once
#include "ForceType.h"
#include "Particle.h"
#include "Vector3D.h"
#include <cmath>
class Explosion : public ForceType {
public:
  Explosion(const Vector3& center_, float K_, float R_, float tau_)
    : center(center_)
    , K(K_)
    , R(R_)
    , tau(tau_)
    , elapsedTime(0.0f)
    , active(false)
  {
  }
  void trigger()
  {
    active = true;
    elapsedTime = 0.0f;
  }

  void triggerAtPosition(const Vector3& newCenter)
  {
    center = newCenter;
    trigger();
  }
  void deactivate() { active = false; }
  bool isActive() const { return active; }
  virtual void updateForce(Particle* particle, double t) override
  {
    if (!particle || particle->getInverseMass() == 0 || !active)
      return;

    elapsedTime += t;

    if (elapsedTime >= 4.0f * tau) {
      active = false;
      return;
    }


    Vector3 pos = particle->getPos();

    Vector3 diff = pos - center;
    float r = diff.magnitude();

   if (r >= R)
      return;

    if (r < 0.001f)
      r = 0.001f;

    float expFactor = std::exp(-elapsedTime / tau);

    float forceMagnitude = (K / (r * r)) * expFactor;

    Vector3 direction = diff / r;

    Vector3 force = direction * forceMagnitude;

    particle->addForce(force);
  }
  virtual void updateForceRigid(physx::PxRigidDynamic* rigid, double t) override {
      if (!rigid || !active || rigid->getMass() == 0) return;

      elapsedTime += t;

      if (elapsedTime >= 4.0f * tau) {
          active = false;
          return;
      }

 
      physx::PxTransform transform = rigid->getGlobalPose();
      physx::PxVec3 pos = transform.p;

      Vector3 particlePos(pos.x, pos.y, pos.z);
      Vector3 diff = particlePos - center;
      float r = diff.magnitude();

      if (r >= R) return;
      if (r < 0.001f) r = 0.001f;

      float expFactor = std::exp(-elapsedTime / tau);
      float forceMagnitude = (K / (r * r)) * expFactor;
      Vector3 direction = diff / r;
      Vector3 force = direction * forceMagnitude;

    
      rigid->addForce(physx::PxVec3(force.x, force.y, force.z));
  }
private:
  Vector3 center;
  float K;
  float R;
  float tau;
  float elapsedTime;
  bool active;
};