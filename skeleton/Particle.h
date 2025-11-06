#pragma once
#include "RenderUtils.hpp"
#include "core.hpp"
using namespace physx;

class ForceGenerador;
class ForceType;

class Particle {
protected:
  Vector3 velocity;
  Vector3 position;
  Vector3 gravity;
  float damping;
  float inverseMass = 0.0f;
  float mass = 1.0f;
  float rad;
  physx::PxTransform transform;
  RenderItem* renderItem;
  Vector3 forceAccumulator;
  Vector4 col;
  Vector3 prePosition;
  int stepNumber = 0;
  ForceGenerador* forceGenerator;

public:
  Particle(const physx::PxVec3& pos,
           const physx::PxVec3& vel,
           const physx::PxVec3& acc,
           float damping_,
           float radius,
           const Vector4& color);

  Particle(const physx::PxVec3& pos,
           const physx::PxVec3& velR,
           const physx::PxVec3& velSim,
           const physx::PxVec3& accR,
           float massR,
           float damping_,
           float radius,
           const Vector4& color);

  Particle(const physx::PxVec3& pos,
           const physx::PxVec3& vel,
           float mass,
           float damping_,
           float radius,
           const Vector4& color);

  virtual ~Particle();

  virtual void integrateForces(double dt);
  void intergrateEulerExplicit(double dt);
  void intergrateEulerSemiExplicit(double dt);
  void intergrateVerlet(double dt);
  void addForce(const physx::PxVec3& force);
  void clearForceAccumulator();
  void changeAcceleration(Vector3 newAcceleration);
  void setScale(float scale);
  ForceGenerador* getForceGenerator() { return forceGenerator; }
  void addForceType(ForceType* fg, bool active = true);
  void removeForceType(ForceType* fg);

  Vector3 getPos() const { return position; }
  Vector3 getVel() const { return velocity; }
  float getMass() const { return mass; }
  float getInverseMass() const { return inverseMass; }
  float getDamping() const { return damping; }
  float getRadius() const { return rad; }
  Vector4 getColor() const { return col; }
  void setForceActive(ForceType* fg, bool active);
  void setPos(const physx::PxVec3& newPos)
  {
    position = newPos;
    transform = physx::PxTransform(position);
  }
};
