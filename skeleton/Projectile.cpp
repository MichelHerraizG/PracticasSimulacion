#include "Projectile.h"
#include "ForceGenerador.h" 
#include <cmath>
Projectile::Projectile(const PxVec3& pos,
                       const PxVec3& vel,
                       float mass,
                       float damping,
                       float radius,
                       const Vector4& color)
  : Particle(pos, vel, mass, damping, radius, color)
  , initialPos(pos)
  , inPlay(false)
  , gravityScale(1.0f)
  , masaReal(mass)
{
}
Projectile::~Projectile()
{
}
void Projectile::reset()
{
  position = initialPos;
  velocity = PxVec3(0, 0, 0);
  forceAccumulator = PxVec3(0, 0, 0);
  inPlay = false;
  transform = PxTransform(position);
  mass = masaReal;
  if (mass > 0.0f)
    inverseMass = 1.0f / mass;
  gravityScale = 1.0f;
}
void Projectile::integrateForces(double dt)
{
  if (!inPlay)
    return;

  if (inverseMass <= 0.0f)
    return;

  forceGenerator->updateForces(dt);

  PxVec3 scaledGravity = gravity * gravityScale;
  PxVec3 acceleration = (forceAccumulator + scaledGravity * mass) * inverseMass;

  velocity += acceleration * dt;
  position += velocity * dt;
  velocity *= pow(damping, dt);

  transform = PxTransform(position);
  clearForceAccumulator();
}
void Projectile::scalePhysics(float velocityScale)
{
  if (velocityScale <= 0.0f)
    return;
  if (masaReal <= 0.0f)
    masaReal = mass;

  velocity *= velocityScale;

  float k2 = velocityScale * velocityScale;
  mass = masaReal / k2; 
  if (mass > 0.0f)
    inverseMass = 1.0f / mass;

  gravityScale = k2;
}

void Projectile::setMass(float newMass)
{
  mass = newMass;
  if (mass > 0.0f)
    inverseMass = 1.0f / mass;
  else
    inverseMass = 0.0f;
}