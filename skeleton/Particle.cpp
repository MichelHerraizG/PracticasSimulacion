#include "Particle.h"
#include "RenderUtils.hpp"
#include "core.hpp"
#include <cmath>
extern physx::PxPhysics* gPhysics;
extern physx::PxMaterial* gMaterial;
using namespace physx;


Particle::Particle(const PxVec3& pos, const PxVec3& vel, const PxVec3& acc, float damping_, float radius, const Vector4& color)
    : position(pos), velocity(vel), acceleration(acc), damping(damping_), transform(PxTransform(pos)), renderItem(nullptr)
{
    PxSphereGeometry sphere(radius);
    renderItem = new RenderItem(CreateShape(sphere, gMaterial), color);
    renderItem->transform = &transform;
    prePosition = pos;
    RegisterRenderItem(renderItem);
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
    acceleration = newAcceleration;
}
void Particle::intergrateEulerExplicit(double dt) 
{
    velocity = velocity + acceleration * dt;
    position = position + velocity * dt;
    velocity = velocity * (pow(damping, dt));
    transform = PxTransform(position); 
}

void Particle::intergrateEulerSemiExplicit(double dt)
{
    velocity = velocity + dt * acceleration;
    position = position + dt * velocity;
    velocity = velocity * (pow(damping, dt));
    transform = PxTransform(position);
}

void Particle::intergrateVerlet(double dt)
{
    if (stepNumber > 0) 
    {
        prePosition = position;
        position = 2 * position - prePosition + ((dt * dt) * acceleration);
        velocity = velocity * (pow(damping, dt));
        transform = PxTransform(position);
    }
    else 
    {
        prePosition = position;
        velocity = velocity + dt * acceleration;
        position = position + dt * velocity;
        velocity = velocity * (pow(damping, dt));
        transform = PxTransform(position);
    }
    stepNumber++;
}