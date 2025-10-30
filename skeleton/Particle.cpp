#include "core.hpp"
#include "Particle.h"
#include "RenderUtils.hpp"
#include <cmath>
extern physx::PxPhysics* gPhysics;
extern physx::PxMaterial* gMaterial;
using namespace physx;

Particle::Particle(const PxVec3& pos,
                   const PxVec3& vel,
                   const PxVec3& acc,
                   float damping_,
                   float radius,
                   const Vector4& color)
  : position(pos)
  , velocity(vel)
  , gravity(acc)
  , damping(damping_)
  , transform(PxTransform(pos))
  , renderItem(nullptr)
{
  PxSphereGeometry sphere(radius);
  renderItem = new RenderItem(CreateShape(sphere, gMaterial), color);
  renderItem->transform = &transform;
  prePosition = pos;
}

Particle::Particle(const PxVec3& pos,
                   const PxVec3& velR,
                   const PxVec3& velSim,
                   const PxVec3& accR,
                   float massR,
                   float damping_,
                   float radius,
                   const Vector4& color)
  : position(pos)
  , damping(damping_)
  , velocity(velSim)
  , transform(PxTransform(pos))
  , renderItem(nullptr)
{
  PxSphereGeometry sphere(radius);
  renderItem = new RenderItem(CreateShape(sphere, gMaterial), color);
  renderItem->transform = &transform;
  prePosition = pos;
  mass = (massR * velR.magnitude() * velR.magnitude()) /
         (velocity.magnitude() * velocity.magnitude());
  gravity = (accR * velR.magnitude() * velR.magnitude()) /
            (velocity.magnitude() * velocity.magnitude());
}
Particle::Particle(const PxVec3& pos,
                   const PxVec3& vel,
                   float mass_,
                   float damping_,
                   float radius,
                   const Vector4& color)
  : position(pos)
  , velocity(vel)
  , mass(mass_)
  , damping(damping_)
  , transform(PxTransform(pos))
  , renderItem(nullptr)
  , forceAccumulator(PxVec3(0.0f))
  , gravity(PxVec3(0.0f))
{

  if (mass_ > 0.0f)
    inverseMass = 1.0f / mass_;
  else
    inverseMass = 0.0f; 

  PxSphereGeometry sphere(radius);
  renderItem = new RenderItem(CreateShape(sphere, gMaterial), color);
  renderItem->transform = &transform;
  prePosition = pos;
}
Particle::~Particle()
{
  if (renderItem != nullptr) {
    DeregisterRenderItem(renderItem);
    delete renderItem;
    renderItem = nullptr;
  }
}
void Particle::addForce(const PxVec3& force)
{
  forceAccumulator += force;
}
void Particle::integrateForces(double dt)
{
  if (inverseMass <= 0.0f)
    return;  

  
  PxVec3 acceleration = forceAccumulator * inverseMass;


  velocity += acceleration * dt;
  position += velocity * dt;

 
  velocity *= pow(damping, dt);


  transform = PxTransform(position);

  
  clearForceAccumulator();
}

void Particle::clearForceAccumulator()
{
  forceAccumulator = PxVec3(0.0f, 0.0f, 0.0f);
}
void Particle::changeAcceleration(Vector3 newAcceleration)
{
  gravity = newAcceleration;
}
void Particle::intergrateEulerExplicit(double dt)
{
  position = position + velocity * dt;
  velocity = velocity + gravity * dt;
  velocity = velocity * (pow(damping, dt));
  transform = PxTransform(position);
}

void Particle::intergrateEulerSemiExplicit(double dt)
{
  velocity = velocity + dt * gravity;
  position = position + dt * velocity;
  velocity = velocity * (pow(damping, dt));
  transform = PxTransform(position);
}

void Particle::intergrateVerlet(double dt)
{
  if (stepNumber > 0) {
    prePosition = position;
    position = 2 * position - prePosition + ((dt * dt) * gravity);
    velocity = velocity * (pow(damping, dt));
    transform = PxTransform(position);
  }
  else {
    prePosition = position;
    velocity = velocity + dt * gravity;
    position = position + dt * velocity;
    velocity = velocity * (pow(damping, dt));
    transform = PxTransform(position);
  }
  stepNumber++;
}