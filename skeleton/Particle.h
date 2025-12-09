#pragma once
#include "RenderUtils.hpp"
#include "core.hpp"
using namespace physx;

#include "ForceGenerador.h"
class ForceType;

class Particle {
protected:
  Vector3 position;
  Vector3 velocity;
  Vector3 gravity;
  Vector3 forceAccumulator;
  Vector3 prePosition;

  float mass = 1.0f;
  float inverseMass = 0.0f;
  float damping;
  float rad;

  int stepNumber = 0;

  Vector4 col;
  physx::PxTransform transform;
  RenderItem* renderItem;
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
  Vector3 getForce() const { return forceAccumulator; }
  void addForce(const physx::PxVec3& force);
  void clearForceAccumulator();
  void changeAcceleration(Vector3 newAcceleration);
  void setScale(float scale);

  void addForceType(ForceType* fg, bool active = true);
  void removeForceType(ForceType* fg);
  void setForceActive(ForceType* fg, bool active);

  void setPos(const physx::PxVec3& newPos)
  {
    position = newPos;
    transform = physx::PxTransform(position);
  }

  Vector3 getPos() const { return position; }
  Vector3 getVel() const { return velocity; }
  float getMass() const { return mass; }
  float getInverseMass() const { return inverseMass; }
  float getDamping() const { return damping; }
  float getRadius() const { return rad; }
  Vector4 getColor() const { return col; }
  ForceGenerador* getForceGenerator() { return forceGenerator; }
  RenderItem* getRenderItem() { return renderItem; }
  void setVisible(bool visible);
  void clearForces() {
      forceGenerator->removeAll(this);
  }
};
