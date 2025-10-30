#pragma once
#include "core.hpp"
#include "ForceGenerator.h"
#include "Particle.h"
class Gravity : public ForceGenerator {
private:
  physx::PxVec3 gravity;

public:

  Gravity(const physx::PxVec3& g)
    : gravity(g)
  {
  }


  virtual void updateForce(Particle* particle, double dt) override
  {

    if (particle->getInverseMass() <= 0.0f)
      return;


    physx::PxVec3 force = gravity * (1.0f / particle->getInverseMass());

    particle->addForce(force);
  }


  physx::PxVec3 getGravity() const { return gravity; }

  void setGravity(const physx::PxVec3& g) { gravity = g; }
};