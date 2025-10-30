#pragma once
#include "core.hpp"
#include "Particle.h"
#include "ForceGenerator.h"

class Wind : public ForceGenerator {
  Vector3 windVelocity;  // Velocidad del viento
  float k1;              // Coeficiente de rozamiento lineal
  float k2;              // Coeficiente de rozamiento cuadrático (opcional)
public:
  Wind(const Vector3& wVel, float k1_ = 0.1f, float k2_ = 0.0f)
    : windVelocity(wVel)
    , k1(k1_)
    , k2(k2_)
  {
  }
  virtual void updateForce(Particle* particle, double t) override
  {
    if (!particle || particle->getInverseMass() == 0)
      return;

    // Diferencia de velocidad (v_v - v)
    Vector3 diff = windVelocity - particle->getVelocity();

    // Magnitud del flujo relativo
    float speed = diff.magnitude();

    // Fv = k1 * diff + k2 * |diff| * diff
    Vector3 force = k1 * diff + k2 * speed * diff;

    particle->addForce(force);
  }
};