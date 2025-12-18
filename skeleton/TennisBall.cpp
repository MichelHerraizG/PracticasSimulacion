#include "TennisBall.h"
#include "ForceType.h"
#include <iostream>

TennisBall::TennisBall(const PxVec3& pos,
                       const PxVec3& vel,
                       float mass,
                       float damping,
                       float rad,
                       const Vector4& col,
                       BallType type,
                       ParticleSystem* system,
                       PxScene* scene,
                       PxPhysics* physics,
                       PxMaterial* material)
  : Ball(
      pos, vel, mass, damping, rad, col, type, system, scene, physics, material)
  , isSpinning(false)
{
  createRigidBody(physics, material);
  setBallType(type);
  if (parSys) {
    EmitterData cfg;
    cfg.emitRate = 30.0f;
    cfg.particleLife = 0.4f;
    cfg.positionVar = Vector3(0.2f, 0.15f, 0.2f);
    cfg.velDist = VelocityDistribution::GAUSSIAN;
    cfg.velMean = Vector3(0.0f, 3.0f, 0.0f);
    cfg.velEstDev = Vector3(0.15f, 0.4f, 0.15f);
    cfg.particleRadius = 0.05f;
    cfg.damping = 0.85f;
    cfg.gravity = Vector3(0.0f, -1.5f, 0.0f);
    cfg.color = Vector4(0.9f, 0.9f, 0.2f, 0.6f);
    Particle* auraModel = new Particle(Vector3(pos.x, pos.y, pos.z),
                                       Vector3(0.0f),
                                       cfg.gravity,
                                       cfg.damping,
                                       cfg.particleRadius,
                                       cfg.color);
    auraIndex = parSys->addEmitter(cfg, auraModel);
    parSys->setEmitterActive(auraIndex, true);
  }
}

void TennisBall::createRigidBody(PxPhysics* physics, PxMaterial* material)
{
  PxTransform startTransform(initialPos);
  rb = physics->createRigidDynamic(startTransform);
  PxSphereGeometry sphereGeom(radius);
  PxShape* shape = physics->createShape(sphereGeom, *material);
  rb->attachShape(*shape);
  PxRigidBodyExt::updateMassAndInertia(*rb, baseMass);
  rb->setLinearVelocity(initialVel);
  rb->setLinearDamping(0.005f);
  rb->setAngularDamping(0.03f);
  myScene->addActor(*rb);
  tr = new PxTransform(startTransform);
  renderItem = new RenderItem(shape, color);
  renderItem->transform = tr;
  RegisterRenderItem(renderItem);
}

TennisBall::~TennisBall()
{
}

void TennisBall::launch(const PxVec3& direction, float power)
{
  if (!rb)
    return;
  PxVec3 dir = direction.getNormalized();
  float hitForce = 0.0f;
  if (parSys && auraIndex >= 0) {
    EmitterData currentData = parSys->getEmitters()[auraIndex].getData();
    currentData.emitRate = 80.0f;
    parSys->updateEmitterData(auraIndex, currentData);
  }
  if (ballType == LIGHT_BALL) {
    hitForce = power * 8.0f;
  }
  else {
    hitForce = power * 10.0f;
  }
  if (rb) {
    PxVec3 force = dir * hitForce;
    if (isSpinning) {
      PxVec3 spinTorque = PxVec3(0, 0, -hitForce * 0.1f);
      rb->addTorque(spinTorque, PxForceMode::eIMPULSE);
    }
    rb->addForce(force, PxForceMode::eIMPULSE);
  }
  inPlay = true;
}

void TennisBall::reset()
{
  Ball::reset();
  isSpinning = false;
  if (parSys && auraIndex >= 0) {
    EmitterData currentData = parSys->getEmitters()[auraIndex].getData();
    currentData.emitRate = 30.0f;
    parSys->updateEmitterData(auraIndex, currentData);
  }
}

void TennisBall::update(double dt)
{
  Ball::update(dt);
}

void TennisBall::setShotType(ShotType type)
{
  currentShotType = type;
  if (parSys && auraIndex >= 0) {
    Vector4 newColor;
    if (type == POWER_SHOT) {
      newColor = Vector4(1.0f, 0.3f, 0.0f, 0.6f);
    }
    else {
      newColor = Vector4(0.0f, 0.8f, 0.2f, 0.6f);
    }
    EmitterData newData = parSys->getEmitters()[auraIndex].getData();
    newData.color = newColor;
    parSys->updateEmitterData(auraIndex, newData);
  }
}