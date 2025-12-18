#pragma once
#include "ForceType.h"
#include "Particle.h"
#include "core.hpp"
#include <PxPhysicsAPI.h>

class Gravity : public ForceType {
private:
  Vector3 gravityAcceleration;

public:
  Gravity(const Vector3& gravity)
    : gravityAcceleration(gravity)
  {
  }

  virtual void updateForce(Particle* particle, double dt) override
  {
    if (!particle || particle->getInverseMass() == 0)
      return;

    // F = m * g
    Vector3 force = gravityAcceleration * particle->getMass();
    particle->addForce(force);
  }

  virtual void updateForceRigid(physx::PxRigidDynamic* rigid,
                                double dt) override
  {
    // PhysX ya maneja la gravedad por defecto en la escena,
    // pero si quisiéramos aplicar una gravedad adicional:
    // (Por ahora dejamos esto vacío ya que PhysX maneja la gravedad)
  }

  void setGravity(const Vector3& gravity) { gravityAcceleration = gravity; }

  const Vector3& getGravity() const { return gravityAcceleration; }
};