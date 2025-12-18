#include "SoccerBall.h"
#include "ForceType.h"
#include <iostream>

SoccerBall::SoccerBall(const PxVec3& pos,
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
  , volleyMode(false)
  , volleySpring(nullptr)
  , volleyAnchor(nullptr)
  , volleyAnchorPos(pos.x, pos.y + 4.0f, pos.z)
  , springConstant(20.0f)
  , restingLength(2.5f)
{
  createRigidBody(physics, material);
  setBallType(type);
  if (parSys) {
    EmitterData cfg;
    cfg.emitRate = 50.0f;
    cfg.particleLife = 0.5f;
    cfg.positionVar = Vector3(0.3f, 0.2f, 0.3f);
    cfg.velDist = VelocityDistribution::GAUSSIAN;
    cfg.velMean = Vector3(0.0f, 4.0f, 0.0f);
    cfg.velEstDev = Vector3(0.2f, 0.6f, 0.2f);
    cfg.particleRadius = 0.08f;
    cfg.damping = 0.8f;
    cfg.gravity = Vector3(0.0f, -2.0f, 0.0f);
    cfg.color = Vector4(1.0f, 0.2f, 0.1f, 0.8f);
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

void SoccerBall::createRigidBody(PxPhysics* physics, PxMaterial* material)
{
  PxTransform startTransform(initialPos);
  rb = physics->createRigidDynamic(startTransform);
  PxSphereGeometry sphereGeom(radius);
  PxShape* shape = physics->createShape(sphereGeom, *material);
  rb->attachShape(*shape);
  PxRigidBodyExt::updateMassAndInertia(*rb, baseMass);
  rb->setLinearVelocity(initialVel);
  rb->setLinearDamping(0.01f);
  rb->setAngularDamping(0.05f);
  myScene->addActor(*rb);
  tr = new PxTransform(startTransform);
  renderItem = new RenderItem(shape, color);
  renderItem->transform = tr;
  RegisterRenderItem(renderItem);
}

SoccerBall::~SoccerBall()
{
  if (volleySpring) {
    delete volleySpring;
    volleySpring = nullptr;
  }
  if (volleyAnchor) {
    delete volleyAnchor;
    volleyAnchor = nullptr;
  }
}

void SoccerBall::enableVolleyMode()
{
  if (volleyMode || inPlay)
    return;
  volleyMode = true;
  PxVec3 currentPos = rb->getGlobalPose().p;
  volleyAnchorPos = Vector3(currentPos.x, currentPos.y + 5.0f, currentPos.z);
  if (volleyAnchor) {
    delete volleyAnchor;
    volleyAnchor = nullptr;
  }
  if (volleySpring) {
    removeForceType(volleySpring);
    delete volleySpring;
    volleySpring = nullptr;
  }
  volleyAnchor =
    new SpringAnchor(volleyAnchorPos, 0.3f, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
  volleySpring =
    new AnchoredSpring(volleyAnchorPos, springConstant, restingLength);
  addForceType(volleySpring,
               true,
               true); 
  if (rb) {
    rb->setLinearVelocity(PxVec3(0, 0, 0));
    rb->setAngularVelocity(PxVec3(0, 0, 0));
  }

}

void SoccerBall::disableVolleyMode()
{
  if (!volleyMode)
    return;
  volleyMode = false;
  if (volleySpring) {
    removeForceType(volleySpring);
    delete volleySpring;
    volleySpring = nullptr;
  }
  if (volleyAnchor) {
    delete volleyAnchor;
    volleyAnchor = nullptr;
  }
}

void SoccerBall::launch(const PxVec3& direction, float power)
{
  if (!rb)
    return;

  if (volleyMode) {
    disableVolleyMode();
  }

  PxVec3 dir = direction.getNormalized();
  float kickForce = 0.0f;

  if (parSys && auraIndex >= 0) {
    EmitterData currentData = parSys->getEmitters()[auraIndex].getData();
    currentData.emitRate = 120.0f;
    parSys->updateEmitterData(auraIndex, currentData);
  }
  if (ballType == LIGHT_BALL) {
    if (currentShotType == POWER_SHOT) {
      kickForce = power * 25.0f;
      rb->setLinearDamping(0.03f);
    }
    else {
      kickForce = power * 30.0f;
      rb->setLinearDamping(0.01f);
    }
  }
  else {
    if (currentShotType == POWER_SHOT) {
      kickForce = power * 20.0f;
      rb->setLinearDamping(0.03f);
    }
    else {
      kickForce = power * 25.0f;
      rb->setLinearDamping(0.01f);
    }
  }
  if (rb) {
    PxVec3 force = dir * kickForce;
    rb->addForce(force, PxForceMode::eIMPULSE);
  }
  inPlay = true;
}

void SoccerBall::reset()
{
  Ball::reset();
  if (volleyMode) {
    disableVolleyMode();
  }
}

void SoccerBall::update(double dt)
{
  Ball::update(dt);
}