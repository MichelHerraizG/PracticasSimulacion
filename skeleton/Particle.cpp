#include "Particle.h"
#include "RenderUtils.hpp"
#include "core.hpp"
#include <cmath>
extern physx::PxPhysics* gPhysics;
extern physx::PxMaterial* gMaterial;
using namespace physx;


Particle::Particle(const PxVec3& pos, const PxVec3& vel, const PxVec3& acc, float damping_, float radius, const Vector4& color)
    : position(pos), velocity(vel), gravity(acc), damping(damping_), transform(PxTransform(pos)), renderItem(nullptr)
{
    PxSphereGeometry sphere(radius);
    renderItem = new RenderItem(CreateShape(sphere, gMaterial), color);
    renderItem->transform = &transform;
    prePosition = pos; 
}

Particle::Particle(const PxVec3& pos, const PxVec3& velR, const PxVec3& velSim, const PxVec3& accR, float massR, float damping_, float radius, const Vector4& color)
    : position(pos), damping(damping_), velocity(velSim),transform(PxTransform(pos)), renderItem(nullptr)
{
    PxSphereGeometry sphere(radius);
    renderItem = new RenderItem(CreateShape(sphere, gMaterial), color);
    renderItem->transform = &transform;
    prePosition = pos;
    mass = (massR * velR.magnitude()*velR.magnitude()) / (velocity.magnitude()* velocity.magnitude());
    gravity= (accR * velR.magnitude() * velR.magnitude()) / (velocity.magnitude() * velocity.magnitude());
}
Particle::~Particle()
{
    if (renderItem != nullptr) {
        DeregisterRenderItem(renderItem);
        delete renderItem;
        renderItem = nullptr;
    }
}
void
Particle::changeAcceleration(Vector3 newAcceleration)
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
    if (stepNumber > 0) 
    {
        prePosition = position;
        position = 2 * position - prePosition + ((dt * dt) * gravity);
        velocity = velocity * (pow(damping, dt));
        transform = PxTransform(position);
    }
    else 
    {
        prePosition = position;
        velocity = velocity + dt * gravity;
        position = position + dt * velocity;
        velocity = velocity * (pow(damping, dt));
        transform = PxTransform(position);
    }
    stepNumber++;
}