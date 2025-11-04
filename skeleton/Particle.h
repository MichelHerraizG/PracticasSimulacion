#pragma once
#include "PxPhysicsAPI.h"
#include "RenderUtils.hpp"
#include "Vector3D.h"

using namespace physx;


class Particle {
public:
  // Constructor básico 
  Particle(const Vector3& pos,
           const Vector3& vel = Vector3(0.0f),
           const Vector3& acc = Vector3(0.0f, -9.8f, 0.0f),
           float damping_ = 0.99f,
           float radius = 1.0f,
           const Vector4& color = Vector4(1.0f, 0.0f, 0.0f, 1.0f));

  // Constructor proyectiles
  Particle(const Vector3& pos,
           const Vector3& velR = Vector3(0.0f),
           const Vector3& velSim = Vector3(0.0f),
           const Vector3& accR = Vector3(0.0f, -9.8f, 0.0f),
           float massR = 0.0f,
           float damping_ = 0.99f,
           float radius = 1.0f,
           const Vector4& color = Vector4(1.0f, 0.0f, 0.0f, 1.0f));

  // Constructor fuerzas
  Particle(const physx::PxVec3& pos,
           const physx::PxVec3& vel,
           float mass_,
           float damping_,
           float radius,
           const Vector4& color);

  ~Particle();


  // Setters

  void changeAcceleration(Vector3 newAcceleration);


  // Integración 
  void intergrateEulerExplicit(double dt);
  void intergrateEulerSemiExplicit(double dt);
  void intergrateVerlet(double dt);
  virtual void integrateForces(double dt);



  // Métodos

  Vector3 getPos() { return position; }
  void addForce(const physx::PxVec3& force);
  void clearForceAccumulator();

  float getMass() const { return mass; }
  float getInverseMass() const { return inverseMass; }
  Vector3 getVelocity() const { return velocity; }

protected:

  // Atributos físicos

  Vector3 position;
  Vector3 velocity;
  Vector3 prePosition;
  Vector3 gravity;
  PxVec3 forceAccumulator;
  float stepNumber;
  float damping;
  float mass;
  float inverseMass;


  // Estado

  PxTransform transform;
  RenderItem* renderItem;
  float timeRemaining;
};
