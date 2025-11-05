#include <ctype.h>

#include <PxPhysicsAPI.h>

#include <vector>

#include "callbacks.hpp"
#include "core.hpp"
#include "Explosion.h"
#include "ForceGenerador.h"
#include "Gravity.h"
#include "Particle.h"
#include "ParticleSystem.h"
#include "RenderUtils.hpp"
#include "Vortex.h"
#include "Wind.h"
#include "SoccerBall.h"
#include "SoccerField.h"
#include "AimingReticle.h"
#include <iostream>

std::string display_text =
  "P: Shoot Projectile, K: Kick, R: Reset Ball, T: Toggle Shot Type";

using namespace physx;

PxDefaultAllocator gAllocator;
PxDefaultErrorCallback gErrorCallback;
PxFoundation* gFoundation = NULL;
PxPhysics* gPhysics = NULL;
Particle* gTestParticle = nullptr;
ParticleSystem* gParticleSystem = nullptr;
PxMaterial* gMaterial = NULL;
SoccerBall* gSoccerBall = nullptr;
SoccerField* gSoccerField = nullptr;
PxPvd* gPvd = NULL;
SoccerBall::ShotType gCurrentShotType = SoccerBall::POWER_SHOT;
PxDefaultCpuDispatcher* gDispatcher = NULL;
PxScene* gScene = NULL;
ContactReportCallback gContactReportCallback;
std::vector<Particle*> projectiles;
ForceGenerador gForceTypes;

// FUERZAS
Gravity* gEarthGravity = nullptr;
Gravity* gWeakGravity = nullptr;
Wind* gWind = nullptr;
Vortex* gVortex = nullptr;
Explosion* gExplosion = nullptr;
AimingReticle* gAimingReticle = nullptr;
bool gWindEnabled = true;
bool gGravityEnabled = true;
bool gExplosionEnabled = false;
const float RETICLEROT = 0.02f;
// Initialize physics engine
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

  // REBOTE
  gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.8f);
  // For Solid Rigids +++++++++++++++++++++++++++++++++++++
  PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
  sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);
  gDispatcher = PxDefaultCpuDispatcherCreate(2);
  sceneDesc.cpuDispatcher = gDispatcher;
  sceneDesc.filterShader = contactReportFilterShader;
  sceneDesc.simulationEventCallback = &gContactReportCallback;
  gScene = gPhysics->createScene(sceneDesc);


  // SISTEMA DE PARTÍCULAS
  gParticleSystem = new ParticleSystem();
  gSoccerField = new SoccerField(2.0f);
  gSoccerBall = new SoccerBall(PxVec3(0, 0.5f, 0.0f),  
    PxVec3(0, 0, 0),   
    0.43f,            
    0.99f,             
    0.60f,  
    Vector4(1, 1, 1, 1)  
  );
  // RETÍCULA
  gAimingReticle = new AimingReticle(3.0f);
  // FUERZAS
  gEarthGravity = new Gravity(PxVec3(0.0f, -9.8f, 0.0f));
  gWind = new Wind(Vector3(-20, 0, 0));
  //gWeakGravity = new Gravity(PxVec3(0.0f, -3.0f, 0.0f));
  //gVortex = new Vortex(Vector3(0.0f, 0.0f, 0.0f), 40.0f, 50.0f);
  gExplosion = new Explosion(Vector3(0.0f, 0.0f, 0.0f), 3000.0f, 30.0f, 1.0f);
  gForceTypes.add(gSoccerBall, gEarthGravity, gGravityEnabled);
  gForceTypes.add(gSoccerBall, gWind, gWindEnabled);
  gForceTypes.add(gSoccerBall, gExplosion,gExplosionEnabled);
}

void Kick()
{
    if (gSoccerBall && gAimingReticle) 
    {
        Vector3 kickdir =  Vector3(gAimingReticle->getAimDirection().x,1.0f, gAimingReticle->getAimDirection().z);

        gSoccerBall->setShotType(gCurrentShotType);
        gSoccerBall->SetInPlay(true);
        gSoccerBall->kick(kickdir, 2.0f);
    }
}
void ToggleShotType()
{
    gCurrentShotType = (gCurrentShotType == SoccerBall::POWER_SHOT) ?
        SoccerBall::PRECISION_SHOT :
        SoccerBall::POWER_SHOT;

    if (gAimingReticle) {
        if (gCurrentShotType == SoccerBall::POWER_SHOT) 
        {
            gAimingReticle->setColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
        }
        else {
            gAimingReticle->setColor(Vector4(0.0f, 0.0f, 1.0f, 1.0f));
        }
    }
}
void ResetSoccerBall()
{
  if (gSoccerBall) {
    gSoccerBall->reset();
  }
}
  // Function to configure what happens in each step of physics
// interactive: true if the game is rendering, false if it offline
// t: time passed since last call in milliseconds
void stepPhysics(bool interactive, double t)
{
  PX_UNUSED(interactive);
  gForceTypes.updateForces(t);

  if (gSoccerBall) {
    gSoccerBall->integrateForces(t);
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
  float desiredSpeed = 50.0f;
  Particle* proj = new Particle(
    shootPos,
    PxVec3(speed * shootDir.x, speed * shootDir.y, speed * shootDir.z),
    mass,
    0.99f,
    0.4f,
    Vector4(1.0f, 1.0f, 1.0f, 1.0f));
  static bool alternate = false;
  if (alternate)
    gForceTypes.add(proj, gEarthGravity,true);
  else
    gForceTypes.add(proj, gWeakGravity, true);

  gForceTypes.add(proj, gVortex, true);

  alternate = !alternate;
  projectiles.push_back(proj);
}
// Function to clean data
// Add custom code to the begining of the function
void cleanupPhysics(bool interactive)
{
  PX_UNUSED(interactive);

  // Rigid Body ++++++++++++++++++++++++++++++++++++++++++
  gScene->release();
  gDispatcher->release();
  // -----------------------------------------------------
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
  if (gWeakGravity)
    delete gWeakGravity;
  if (gVortex)
    delete gVortex;
  if (gExplosion) {
    delete gExplosion;
    gExplosion = nullptr;
  }
  gFoundation->release();
}

// Function called when a key is pressed
void keyPress(unsigned char key, const PxTransform& camera)
{
  PX_UNUSED(camera);

  switch (toupper(key)) 
  {
  case 'V':
      gWindEnabled = !gWindEnabled;
      gForceTypes.setActive(gWind,gWindEnabled);
  case 'G':
      gGravityEnabled = !gGravityEnabled;
      gForceTypes.setActive(gEarthGravity, gGravityEnabled);
      break;
  case 'X':
      gExplosionEnabled = !gExplosionEnabled;
      gForceTypes.setActive(gExplosion, gExplosionEnabled);
      break;
  case 'A':
      gAimingReticle->rotateLeft(RETICLEROT);
      break;
  case 'D':
      gAimingReticle->rotateRight(RETICLEROT);
      break;
 
    case 'P': {
      Shoot();
      break;
    }
    case 'K': {
      Kick();
      break;
    }
    case 'R': {
      ResetSoccerBall();
      break;
    }
    case 'T': {  
      ToggleShotType();
      break;
    }
    case 'E': {
      if (gExplosion) {
        gExplosion->trigger();
      }
      break;
    }
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