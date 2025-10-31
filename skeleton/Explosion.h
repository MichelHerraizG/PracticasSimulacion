#pragma once
#include "ForceGenerator.h"
#include "Particle.h"
#include "Vector3D.h"
#include <cmath>
class Explosion : public ForceGenerator {
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

  
    if (r >= R || r < 0.001f)  
      return;


    float expFactor = std::exp(-elapsedTime / tau);


    float forceMagnitude = (K / (r * r)) * expFactor;

 
    Vector3 direction = diff / r;


    Vector3 force = direction * forceMagnitude;

    particle->addForce(force);
  }

private:
  Vector3 center;    
  float K;         
  float R;           
  float tau;         
  float elapsedTime;  
  bool active;
};