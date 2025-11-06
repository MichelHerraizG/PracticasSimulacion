#include "ForceGenerador.h"
#include "Particle.h"
#include "core.hpp"
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
  , rad(radius)
  , transform(PxTransform(pos))
  , renderItem(nullptr)
  , col(color)
{
  forceGenerator = new ForceGenerador();
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
  , velocity(velSim)
  , gravity(accR)    
  , damping(damping_)
  , rad(radius)
  , mass(massR)  
  , transform(PxTransform(pos))
  , renderItem(nullptr)
  , forceAccumulator(PxVec3(0.0f))
  , col(color)
{

  if (massR > 0.0f)
    inverseMass = 1.0f / massR;
  else
    inverseMass = 0.0f;
  forceGenerator = new ForceGenerador();
  PxSphereGeometry sphere(radius);
  renderItem = new RenderItem(CreateShape(sphere, gMaterial), color);
  renderItem->transform = &transform;
  prePosition = pos;

}
Particle::Particle(const PxVec3& pos,
                   const PxVec3& vel,
                   float mass_,
                   float damping_,
                   float radius,
                   const Vector4& color)
  : position(pos)
  , velocity(vel)
  , rad(radius)
  , mass(mass_)
  , damping(damping_)
  , transform(PxTransform(pos))
  , renderItem(nullptr)
  , forceAccumulator(PxVec3(0.0f))
  , gravity(PxVec3(0.0f))
  , col(color)
{

  if (mass_ > 0.0f)
    inverseMass = 1.0f / mass_;
  else
    inverseMass = 0.0f; 

  PxSphereGeometry sphere(radius);
  renderItem = new RenderItem(CreateShape(sphere, gMaterial), color);
  renderItem->transform = &transform;
  prePosition = pos;
  forceGenerator = new ForceGenerador();
}
Particle::~Particle()
{
  if (renderItem != nullptr) {
    DeregisterRenderItem(renderItem);
    delete renderItem;
  }
  delete forceGenerator;
}
void Particle::addForce(const PxVec3& force)
{
  forceAccumulator += force;
}
void Particle::setScale(float scale)
{
  rad *= scale;
  if (renderItem != nullptr) {
    DeregisterRenderItem(renderItem);
    delete renderItem;
    renderItem = nullptr;
  }
  PxSphereGeometry sphere(rad);
  renderItem = new RenderItem(CreateShape(sphere, gMaterial), col);
  renderItem->transform = &transform;
}
void Particle::integrateForces(double dt)
{
  if (inverseMass <= 0.0f)
    return;


  forceGenerator->updateForces(dt);

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

void Particle::addForceType(ForceType* fg, bool active)
{
  if (forceGenerator)
    forceGenerator->add(this, fg, active);
}

void Particle::removeForceType(ForceType* fg)
{
  if (forceGenerator)
    forceGenerator->remove(this, fg);
}