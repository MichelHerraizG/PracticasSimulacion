#include "Ball.h"
#include "ForceType.h"
#include <iostream>

Ball::Ball(const PxVec3& pos,
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
  : rb(nullptr)
  , myScene(scene)
  , renderItem(nullptr)
  , tr(nullptr)
  , ballType(type)
  , currentShotType(POWER_SHOT)
  , radius(rad)
  , baseMass(mass)
  , color(col)
  , inPlay(false)
  , initialPos(pos)
  , initialVel(vel)
  , parSys(system)
  , auraIndex(-1)
{
}

Ball::~Ball()
{
  if (renderItem) {
    DeregisterRenderItem(renderItem);
    delete renderItem;
    renderItem = nullptr;
  }

  if (tr) {
    delete tr;
    tr = nullptr;
  }








}

void Ball::reset()
{
  if (!rb)
    return;
  PxTransform resetTransform(initialPos);
  rb->setGlobalPose(resetTransform);
  rb->setLinearVelocity(initialVel);
  rb->setAngularVelocity(PxVec3(0, 0, 0));
  rb->setLinearDamping(0.005f);
  rb->setAngularDamping(0.02f);
  inPlay = false;
  if (parSys && auraIndex >= 0) {
    EmitterData currentData = parSys->getEmitters()[auraIndex].getData();
    currentData.emitRate = 50.0f;
    parSys->updateEmitterData(auraIndex, currentData);
  }
}

void Ball::update(double dt)
{
  updateRenderTransform();
  updateAura();
}

void Ball::updateAura()
{
  if (parSys && auraIndex >= 0 && rb) {
    PxVec3 currentPos = rb->getGlobalPose().p;
    parSys->updateEmitterPosition(
      auraIndex, Vector3(currentPos.x, currentPos.y + 0.1f, currentPos.z));
  }
}

void Ball::updateRenderTransform()
{
  if (rb && tr) {
    *tr = rb->getGlobalPose();
  }
}

PxVec3 Ball::getPos() const
{
  if (rb && rb->getScene()) {
    return rb->getGlobalPose().p;
  }
  return PxVec3(0, 0, 0);
}

PxVec3 Ball::getVel() const
{
  if (rb && rb->getScene()) {
    return rb->getLinearVelocity();
  }
  return PxVec3(0, 0, 0);
}

void Ball::setBallType(BallType type)
{
  ballType = type;
  if (!rb)
    return;
  if (type == LIGHT_BALL) {
    baseMass = 0.15f;
    PxRigidBodyExt::updateMassAndInertia(*rb, baseMass);
    rb->setLinearDamping(0.15f);
  }
  else {
    baseMass = 0.70f;
    PxRigidBodyExt::updateMassAndInertia(*rb, baseMass);
    rb->setLinearDamping(0.01f);
  }
}

void Ball::setShotType(ShotType type)
{
  currentShotType = type;
  if (parSys && auraIndex >= 0) {
    Vector4 newColor;
    if (type == POWER_SHOT) {
      newColor = Vector4(1.0f, 0.0f, 0.0f, 0.7f);
    }
    else {
      newColor = Vector4(0.0f, 0.5f, 1.0f, 0.7f);
    }
    EmitterData newData = parSys->getEmitters()[auraIndex].getData();
    newData.color = newColor;
    parSys->updateEmitterData(auraIndex, newData);
  }
}

void Ball::setInitialPos(const PxVec3& pos)
{
  initialPos = pos;
}

void Ball::addForceType(ForceType* force, bool active, bool applyAlways)
{
  if (!force)
    return;
  forces.push_back(force);
  activeForces.push_back(active);
  alwaysApply.push_back(applyAlways);
}

void Ball::removeForceType(ForceType* force)
{
  for (size_t i = 0; i < forces.size(); ++i) {
    if (forces[i] == force) {
      forces.erase(forces.begin() + i);
      activeForces.erase(activeForces.begin() + i);
      alwaysApply.erase(alwaysApply.begin() + i);
      return;
    }
  }
}

void Ball::setForceActive(ForceType* force, bool active)
{
  for (size_t i = 0; i < forces.size(); ++i) {
    if (forces[i] == force) {
      activeForces[i] = active;
      return;
    }
  }
}

void Ball::clearForces()
{
  forces.clear();
  activeForces.clear();
  alwaysApply.clear();
}

void Ball::integrateForces(double dt)
{
  if (!rb || !rb->getScene())
    return;
  
  for (size_t i = 0; i < forces.size(); ++i) {
    if (activeForces[i] && forces[i]) {
      if (alwaysApply[i] || inPlay) {
        forces[i]->updateForceRigid(rb, dt);
      }
    }
  }
}