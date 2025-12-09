
#include "ForceType.h"
#include "SoccerBall.h"
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

  std::cout << "SoccerBall creado como sólido rígido dinámico" << std::endl;
}
void SoccerBall::createRigidBody(PxPhysics* physics, PxMaterial* material)
{
  PxTransform startTransform(initialPos);
  rb = physics->createRigidDynamic(startTransform);
  PxSphereGeometry sphereGeom(radius);
  PxShape* shape = physics->createShape(sphereGeom, *material);
  rb->attachShape(*shape);
  shape->release();
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
}

void SoccerBall::setBallType(BallType type)
{
  ballType = type;

  if (!rb)
    return;

  if (type == LIGHT_BALL) {
    baseMass = 0.05f;
    PxRigidBodyExt::updateMassAndInertia(*rb, baseMass);
    rb->setLinearDamping(0.05f);
  }
  else {
    baseMass = 0.43f;
    PxRigidBodyExt::updateMassAndInertia(*rb, baseMass);
    rb->setLinearDamping(0.01f);
  }
}

void SoccerBall::launch(const PxVec3& direction, float power)
{
  if (!rb) return;
  
  PxVec3 dir = direction.getNormalized();
  float kickForce = 0.0f;
  
  if (parSys && auraIndex >= 0) {
    EmitterData currentData = parSys->getEmitters()[auraIndex].getData();
    currentData.emitRate = 120.0f;
    parSys->updateEmitterData(auraIndex, currentData);
  }
  
  if (ballType == LIGHT_BALL) {
    if (currentShotType == POWER_SHOT) {
      kickForce = power * 15.0f;
      rb->setLinearDamping(0.08f);
    }
    else {
      kickForce = power * 10.0f;
      rb->setLinearDamping(0.04f);
    }
  }
  else {
    if (currentShotType == POWER_SHOT) {
      kickForce = power * 15.0f;
      rb->setLinearDamping(0.08f);
    }
    else {
      kickForce = power * 10.0f;
      rb->setLinearDamping(0.03f);
    }
  }
  
  PxVec3 kickVelocity = dir * kickForce;
  if (rb) {
    PxVec3 force = dir * kickForce;
    rb->addForce(force, PxForceMode::eIMPULSE);
  }

  inPlay = true;
  
}
void SoccerBall::reset()
{
  if (!rb)
    return;


  PxTransform resetTransform(initialPos);
  rb->setGlobalPose(resetTransform);
  rb->setLinearVelocity(initialVel);
  rb->setAngularVelocity(PxVec3(0, 0, 0));

  rb->setLinearDamping(0.01f);
  rb->setAngularDamping(0.05f);

  inPlay = false;
  if (parSys && auraIndex >= 0) {
    EmitterData currentData = parSys->getEmitters()[auraIndex].getData();
    currentData.emitRate = 50.0f;
    parSys->updateEmitterData(auraIndex, currentData);
  }
}

void SoccerBall::updateAura()
{
  if (parSys && auraIndex >= 0 && rb) {
    PxVec3 currentPos = rb->getGlobalPose().p;
    parSys->updateEmitterPosition(
      auraIndex, Vector3(currentPos.x, currentPos.y + 0.1f, currentPos.z));
  }
}
void SoccerBall::updateRenderTransform()
{
  if (rb && tr) {
    *tr = rb->getGlobalPose();
  }
}
PxVec3 SoccerBall::getPos() const
{
  if (rb) {
    return rb->getGlobalPose().p;
  }
  return PxVec3(0, 0, 0);
}
PxVec3 SoccerBall::getVel() const
{
  if (rb) {
    return rb->getLinearVelocity();
  }
  return PxVec3(0, 0, 0);
}
void SoccerBall::setForceActive(ForceType* force, bool active)
{
  for (size_t i = 0; i < forces.size(); ++i) {
    if (forces[i] == force) {
      activeForces[i] = active;
      return;
    }
  }
}
void SoccerBall::integrateForces(double dt)
{
  if (!rb || !inPlay)
    return;
  for (int i = 0; i < forces.size(); ++i) {
    if (activeForces[i] && forces[i]) {
      PxVec3 pos = rb->getGlobalPose().p;
      PxVec3 vel = rb->getLinearVelocity();
      Particle tempParticle(Vector3(pos.x, pos.y, pos.z),
                            Vector3(vel.x, vel.y, vel.z),
                            1.0f / rb->getMass(),  
                            0.99f,
                            radius,
                            color);

      forces[i]->updateForce(&tempParticle, dt);
      Vector3 force = tempParticle.getForce();
      rb->addForce(PxVec3(force.x, force.y, force.z));
    }
  }
}
void SoccerBall::update(double dt)
{
  updateRenderTransform();
  updateAura();
}

void SoccerBall::setShotType(ShotType type)
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
void SoccerBall::addForceType(ForceType* force, bool active)
{
  if (!force)
    return;

  forces.push_back(force);
  activeForces.push_back(active);

}

void SoccerBall::clearForces()
{
  forces.clear();
  activeForces.clear();
}
