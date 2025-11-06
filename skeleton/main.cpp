#include "AimingReticle.h"
#include "Explosion.h"
#include "ForceGenerador.h"
#include "Gravity.h"
#include "Particle.h"
#include "ParticleSystem.h"
#include "RenderUtils.hpp"
#include "SoccerBall.h"
#include "SoccerField.h"
#include "Wind.h"
#include "callbacks.hpp"
#include "core.hpp"
#include <PxPhysicsAPI.h>
#include <ctype.h>
#include <iostream>
#include <vector>

std::string display_text =
  "P: Shoot Projectile, K: Kick, R: Reset Ball, T: Toggle Shot Type\n"
  "G: Toggle Gravity, W: Toggle Wind";
using namespace physx;

PxDefaultAllocator gAllocator;
PxDefaultErrorCallback gErrorCallback;
PxFoundation* gFoundation = NULL;
PxPhysics* gPhysics = NULL;
ParticleSystem* gParticleSystem = nullptr;
PxMaterial* gMaterial = NULL;
SoccerBall* gSoccerBall = nullptr;
SoccerField* gSoccerField = nullptr;
PxPvd* gPvd = NULL;
ShotType gCurrentShotType = POWER_SHOT;
PxDefaultCpuDispatcher* gDispatcher = NULL;
PxScene* gScene = NULL;
ContactReportCallback gContactReportCallback;
std::vector<Particle*> projectiles;
ForceGenerador gForceTypes;

Gravity* gEarthGravity = nullptr;
Wind* gWind = nullptr;
AimingReticle* gAimingReticle = nullptr;
bool gWindEnabled = true;
bool gGravityEnabled = true;
const float RETICLEROT = 0.02f;
float gExplosionTimer = 0.0f;
bool gExplosionScheduled = false;
Explosion* gExplosion = nullptr;

void initPhysics(bool interactive)
{
  PX_UNUSED(interactive);

  gFoundation =
    PxCreateFoundation(PX_FOUNDATION_VERSION, gAllocator, gErrorCallback);
  gPvd = PxCreatePvd(*gFoundation);
  PxPvdTransport* transport =
    PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
  gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
  gPhysics = PxCreatePhysics(
    PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

  gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.8f);

  PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
  sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);
  gDispatcher = PxDefaultCpuDispatcherCreate(2);
  sceneDesc.cpuDispatcher = gDispatcher;
  sceneDesc.filterShader = contactReportFilterShader;
  sceneDesc.simulationEventCallback = &gContactReportCallback;
  gScene = gPhysics->createScene(sceneDesc);

  gParticleSystem = new ParticleSystem();
  gSoccerField = new SoccerField(2.0f);

  gSoccerBall = new SoccerBall(PxVec3(0, 1.0f, 0.0f),
                               PxVec3(30.0f, 0, 0),
                               0.43f,
                               0.99f,
                               0.60f,
                               Vector4(1, 1, 1, 1),
                               STANDARD_BALL,
                               gParticleSystem);

  gAimingReticle = new AimingReticle(3.0f);

  gEarthGravity = new Gravity(PxVec3(0.0f, -9.8f, 0.0f));
  gWind = new Wind(Vector3(-20, 0, 0));

  if (gGravityEnabled)
    gSoccerBall->addForceType(gEarthGravity, true);
  if (gWindEnabled)
    gSoccerBall->addForceType(gWind, true);
}

void Kick()
{
  if (gSoccerBall && gAimingReticle) {
    Vector3 kickDir = Vector3(gAimingReticle->getAimDirection().x,
                              0.3f,
                              gAimingReticle->getAimDirection().z);

    gSoccerBall->setInPlay(true);
    gSoccerBall->setShotType(gCurrentShotType);
    gSoccerBall->launch(PxVec3(kickDir.x, kickDir.y, kickDir.z), 2.0f);
    gExplosionTimer = 0.0f;
    gExplosionScheduled = false;
  }
}

void ToggleShotType()
{
  gCurrentShotType =
    (gCurrentShotType == POWER_SHOT) ? PRECISION_SHOT : POWER_SHOT;

  if (gAimingReticle) {
    if (gCurrentShotType == POWER_SHOT)
      gAimingReticle->setColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    else
      gAimingReticle->setColor(Vector4(0.0f, 0.0f, 1.0f, 1.0f));
  }
}

void ResetSoccerBall()
{
  if (gSoccerBall) {
    gSoccerBall->reset();
    gExplosionTimer = 0.0f;
    gExplosionScheduled = false;
  }
}

void TriggerBallExplosion()
{

}
void stepPhysics(bool interactive, double t)
{
  PX_UNUSED(interactive);
  gForceTypes.updateForces(t);

  if (gSoccerBall) {
    gSoccerBall->integrateForces(t);
    if (gSoccerBall->isInPlay() && !gExplosionScheduled) {
      gExplosionTimer += t;
      if (gExplosionTimer >= 3.0f) {
        TriggerBallExplosion();
        gExplosionScheduled = true;
      }
    }
  }

  for (auto proj : projectiles) {
    proj->integrateForces(t);
  }

  if (gAimingReticle && gSoccerBall && !gSoccerBall->isInPlay()) {
    gAimingReticle->update(gSoccerBall->getPos());
  }

  if (gParticleSystem)
    gParticleSystem->update(t);

  gScene->simulate(t);
  gScene->fetchResults(true);
}

void Shoot()
{
  Vector3 shootPos = GetCamera()->getEye();
  Vector3 shootDir = GetCamera()->getDir();
  shootDir.normalize();
  float speed = 60.0f;
  float mass = 5.0f;

  Particle* proj = new Particle(
    shootPos,
    PxVec3(speed * shootDir.x, speed * shootDir.y, speed * shootDir.z),
    mass,
    0.99f,
    0.4f,
    Vector4(1.0f, 1.0f, 1.0f, 1.0f));

  gForceTypes.add(proj, gEarthGravity, true);
  projectiles.push_back(proj);
}

void cleanupPhysics(bool interactive)
{
  PX_UNUSED(interactive);

  gScene->release();
  gDispatcher->release();
  gPhysics->release();
  PxPvdTransport* transport = gPvd->getTransport();
  gPvd->release();
  transport->release();

  if (gParticleSystem) {
    delete gParticleSystem;
    gParticleSystem = nullptr;
  }
  if (gSoccerField) {
    delete gSoccerField;
    gSoccerField = nullptr;
  }
  if (gSoccerBall) {
    delete gSoccerBall;
    gSoccerBall = nullptr;
  }
  if (gAimingReticle) {
    delete gAimingReticle;
    gAimingReticle = nullptr;
  }
  if (gEarthGravity)
    delete gEarthGravity;
  if (gWind)
    delete gWind;
  if (gExplosion)
    delete gExplosion;

  gFoundation->release();
}

void keyPress(unsigned char key, const PxTransform& camera)
{
  PX_UNUSED(camera);

  switch (toupper(key)) {
    case 'V':
      gWindEnabled = !gWindEnabled;
      gForceTypes.setActive(gWind, gWindEnabled);
      break;
    case 'G':
      gGravityEnabled = !gGravityEnabled;
      gForceTypes.setActive(gEarthGravity, gGravityEnabled);
      break;
    case 'A':
      gAimingReticle->rotateLeft(RETICLEROT);
      break;
    case 'D':
      gAimingReticle->rotateRight(RETICLEROT);
      break;
    case 'P':
      Shoot();
      break;
    case 'K':
      Kick();
      break;
    case 'R':
      ResetSoccerBall();
      break;
    case 'T':
      ToggleShotType();
      break;
    default:
      break;
  }
}

void onCollision(physx::PxActor* actor1, physx::PxActor* actor2)
{
  PX_UNUSED(actor1);
  PX_UNUSED(actor2);
}

int main(int, const char* const*)
{
#ifndef OFFLINE_EXECUTION
  extern void renderLoop();
  renderLoop();
#else
  static const PxU32 frameCount = 100;
  initPhysics(false);
  for (PxU32 i = 0; i < frameCount; i++)
    stepPhysics(false);
  cleanupPhysics(false);
#endif

  return 0;
}