#pragma once
#include "Particle.h" 
#include "ForceType.h" 
class Projectile : public Particle {
public:
  Projectile(const PxVec3& pos = PxVec3(0.0f),
             const PxVec3& vel = PxVec3(0.0f),
             float mass = 1.0f,
             float damping = 0.99f,
             float radius = 0.1f,
             const Vector4& color = Vector4(1.0f, 1.0f, 1.0f, 1.0f));
  virtual ~Projectile();
  virtual void launch(const PxVec3& direction, float power) = 0;
  virtual void reset();
  virtual void integrateForces(double dt) override;
  virtual void scalePhysics(float velocityScale);
  void setMass(float newMass);
  float getRealMass() const { return realMass; }
  float getSimulatedMass() const { return mass; }
  bool isInPlay() const { return inPlay; }
  void setInPlay(bool v) { inPlay = v; }

protected:
  PxVec3 initialPos; 
  bool inPlay;      
  float gravityScale;
  float realMass;
};
