#include "AimingReticle.h"
#include "AnchoredSpring.h"
#include "Barrier.h"
#include "Explosion.h"
#include "ForceGenerador.h"
#include "Goalkeeper.h"
#include "Gravity.h"
#include "Particle.h"
#include "ParticleSystem.h"
#include "PowerMeter.h"
#include "RenderUtils.hpp"
#include "SoccerBall.h"
#include "SoccerField.h"
#include "TennisBall.h"
#include "Wind.h"

#include "callbacks.hpp"
#include "core.hpp"
#include <PxPhysicsAPI.h>
#include <cstdlib>
#include <ctime>
#include <ctype.h>
#include <iostream>
#include <vector>

std::string display_text =
  "K: Empezar/Confirmar tiro (detiene medidor), R: Reset Ball, M: Toggle Volley Mode\n"
  "T: Toggle Shot Type, V: Toggle Wind, A/D: Aim Left/Right, B: Toggle Ball Type\n"
  "El cuadrado rojo sube y baja - presiona K para fijar la potencia";

using namespace physx;

PxDefaultAllocator gAllocator;
PxDefaultErrorCallback gErrorCallback;
PxFoundation* gFoundation = NULL;
PxPhysics* gPhysics = NULL;
ParticleSystem* gBallParticleSystem = nullptr;
PxMaterial* gMaterial = NULL;
Ball* gBall = nullptr;
SoccerBall* gSoccerBall = nullptr;
TennisBall* gTennisBall = nullptr;

Goalkeeper* gGoalkeeper = nullptr;
SoccerField* gSoccerField = nullptr;
PowerMeter* gPowerMeter = nullptr;
Barrier* gBarrier = nullptr;
bool gAimingPhase = true;
PxPvd* gPvd = NULL;
ShotType gCurrentShotType = POWER_SHOT;
PxDefaultCpuDispatcher* gDispatcher = NULL;
PxScene* gScene = NULL;
int gGoalsScored = 0;
ContactReportCallback gContactReportCallback;
std::vector<Particle*> projectiles;
ForceGenerador gForceTypes;
Gravity* gEarthGravity = nullptr;

bool gUsingSoccerBall = true;
bool gExplosionActive = false;
double gExplosionTimer = 0.0;

bool gVictoryActive = false;
ParticleSystem* gVictoryParticleSystem = nullptr;

AimingReticle* gAimingReticle = nullptr;
bool gWindEnabled = true;
bool gGravityEnabled = true;
const float RETICLEROT = 0.02f;
PxVec3 gCameraBasePos(0, 10.0f, 20.0f);
ParticleSystem* gExplosionParticleSystem = nullptr;
Explosion* gExplosion;

ParticleSystem* gWindParticleSystem = nullptr;
Wind* gWind = nullptr;

void CreateWindParticles()
{
  EmitterData windData;
  windData.position = Vector3(0.0f, 0.0f, 0.0f);
  windData.positionVar = Vector3(2.0f, 2.0f, 2.0f);
  windData.emitRate = 25.0f;
  windData.particleLife = 8.0f;
  windData.particleRadius = 0.1f;
  windData.color = Vector4(0.8f, 0.8f, 0.8f, 1.0f);
  windData.damping = 0.8f;
  windData.velDist = VelocityDistribution::UNIFORM;
  windData.velMin = Vector3(-8.0f, 2.0f, -8.0f);
  windData.velMax = Vector3(8.0f, 12.0f, 8.0f);

  Particle* windModel = new Particle(Vector3(20.0f, 2.5f, 0.0f),
                                     Vector3(0, 0, 0),
                                     0.1f,
                                     windData.damping,
                                     windData.particleRadius,
                                     windData.color);

  gWindParticleSystem->addEmitter(windData, windModel);
}
void UpdateCameraForGoal()
{
  Camera* camera = GetCamera();
  if (camera) {
    float newZ = 15.0f + (gGoalsScored * 12.0f);
    float newX = 0.0f;

    if (gGoalsScored == 1) {
      newX = -15.0f;
    }
    else if (gGoalsScored == 2) {
      newX = 15.0f;
    }

    camera->setEye(Vector3(newX, 5.0f, newZ));
    camera->setDir(PxVec3(0.0f, 0.0f, -1.0f));
  }
}
void initPhysics(bool interactive)
{
  srand(static_cast<unsigned int>(time(nullptr)));

  gFoundation =
    PxCreateFoundation(PX_FOUNDATION_VERSION, gAllocator, gErrorCallback);
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

  Camera* camera = GetCamera();
  if (camera) {
    camera->setEye(Vector3(0.0f, 5.0f, 15.0f));
    camera->setDir(PxVec3(0.0f, 0.0f, -1.0f));
  }

  gBallParticleSystem = new ParticleSystem();
  gSoccerField = new SoccerField(2.0f, gScene);

  gVictoryParticleSystem = new ParticleSystem();
  gWindParticleSystem = new ParticleSystem();
  gWind = new Wind(Vector3(-30, 0, 0));
  gWindParticleSystem->addSystemForce(gWind, true);
  CreateWindParticles();

  gEarthGravity = new Gravity(Vector3(0.0f, -9.8f, 0.0f));

  gPowerMeter = new PowerMeter(0.5f, 3.0f, 2.5f);
  gPowerMeter->setPosition(Vector3(-8.0f, 0.5f, 0.0f));

  bool useSoccerBall = (rand() % 2) == 0;

  if (useSoccerBall) {
    gSoccerBall = new SoccerBall(PxVec3(0, 1.0f, 0.0f),
                                 PxVec3(0, 0, 0),
                                 0.43f,
                                 0.99f,
                                 0.6f,
                                 Vector4(1, 1, 1, 1),
                                 STANDARD_BALL,
                                 gBallParticleSystem,
                                 gScene,
                                 gPhysics,
                                 gMaterial);
    gBall = gSoccerBall;
  }
  else {
    gTennisBall = new TennisBall(PxVec3(0, 1.0f, 0.0f),
                                 PxVec3(0, 0, 0),
                                 0.058f,
                                 0.99f,
                                 0.33f,
                                 Vector4(0.9f, 0.9f, 0.2f, 1.0f),
                                 LIGHT_BALL,
                                 gBallParticleSystem,
                                 gScene,
                                 gPhysics,
                                 gMaterial);
    gBall = gTennisBall;
  }

  gAimingReticle = new AimingReticle(3.0f);

  gGoalkeeper = new Goalkeeper(PxVec3(0.0f, 1.0f, -34.0f),
                               2.5f,
                               8.0f,
                               0.5f,
                               6.0f,
                               12.0f,
                               Vector4(0.0f, 0.8f, 0.2f, 1.0f),
                               gScene,
                               gPhysics,
                               gMaterial);
  gBarrier = new Barrier(PxVec3(0, 1.0f, -15.0f),
                         3.0f,
                         5.0f,
                         0.5f,
                         1200.0f,
                         8.0f,
                         Vector4(0.8f, 0.2f, 0.2f, 1.0f),
                         gScene,
                         gPhysics,
                         gMaterial);
  gExplosionParticleSystem = new ParticleSystem();
  gExplosion = new Explosion(Vector3(0, 0, 0), 1500.0f, 10.0f, 0.5f);
  gExplosionParticleSystem->addSystemForce(gExplosion, false);

  if (gWindEnabled)
    gBall->addForceType(gWind, true);
}

void ToggleVolleyMode()
{
  if (!gBall)
    return;

  if (gSoccerBall) {
    if (gSoccerBall->isVolleyMode()) {
      gSoccerBall->disableVolleyMode();
    }
    else {
      gSoccerBall->enableVolleyMode();
    }
  }
}

void Kick()
{
  if (!gBall || !gAimingReticle || !gPowerMeter)
    return;

  if (gBall->isInPlay())
    return;

  if (!gPowerMeter->getIsStopped()) {
    gPowerMeter->stop();
    return;
  }

  if (gSoccerBall && gSoccerBall->isVolleyMode()) {
    gSoccerBall->disableVolleyMode();
  }

  float normalizedPower = gPowerMeter->getNormalizedValue();
  float minY = 0.05f;
  float maxY = 0.3f;
  float kickY = minY + normalizedPower * (maxY - minY);

  Vector3 kickDir = Vector3(gAimingReticle->getAimDirection().x,
                            kickY,
                            gAimingReticle->getAimDirection().z);

  gBall->setInPlay(true);
  gBall->setShotType(gCurrentShotType);

  float powerMultiplier;
  if (gTennisBall) {
    powerMultiplier = (gCurrentShotType == POWER_SHOT) ? 1.2f : 1.0f;
  }
  else {
    powerMultiplier = (gCurrentShotType == POWER_SHOT) ? 2.0f : 1.5f;
  }

  gBall->launch(PxVec3(kickDir.x, kickDir.y, kickDir.z), powerMultiplier);
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

void ToggleBallType()
{
  if (!gBall)
    return;

  BallType currentType = gBall->getBallType();
  BallType newType =
    (currentType == STANDARD_BALL) ? LIGHT_BALL : STANDARD_BALL;

  gBall->setBallType(newType);

  //if (newType == LIGHT_BALL) {
  //  std::cout << "Tipo de pelota cambiado a: LIGERA" << std::endl;
  //}
  //else {
  //  std::cout << "Tipo de pelota cambiado a: ESTÁNDAR" << std::endl;
  //}
}

void ResetProjectile()
{
  if (gBall) {
    gBall->reset();
    gBall->setInPlay(false);
  }

  if (gPowerMeter) {
    gPowerMeter->reset();
  }

  if (gVictoryActive) {
    gVictoryActive = false;
    if (gVictoryParticleSystem) {
      gVictoryParticleSystem->clearEmitters();
    }
  }
}

void CreateExplosionParticles()
{
  if (!gExplosionParticleSystem)
    return;

  gExplosionParticleSystem->clearEmitters();

  EmitterData explosionData;
  explosionData.position = Vector3(0, 2.5f, -35.0f);
  explosionData.positionVar = Vector3(2.0f, 2.0f, 2.0f);
  explosionData.emitRate = 300.0f;
  explosionData.particleLife = 2.0f;
  explosionData.particleRadius = 0.3f;
  explosionData.color = Vector4(1.0f, 0.5f, 0.0f, 1.0f);
  explosionData.damping = 0.9f;
  explosionData.velDist = VelocityDistribution::UNIFORM;
  explosionData.velMin = Vector3(-12.0f, 5.0f, -12.0f);
  explosionData.velMax = Vector3(12.0f, 20.0f, 12.0f);

  Particle* explosionModel = new Particle(Vector3(0, 2.5f, -35.0f),
                                          Vector3(0, 10.0f, 0),
                                          0.2f,
                                          explosionData.damping,
                                          explosionData.particleRadius,
                                          explosionData.color);

  gExplosionParticleSystem->addEmitter(explosionData, explosionModel);
}

void CreateVictoryParticles()
{
  if (!gVictoryParticleSystem)
    return;

  gVictoryParticleSystem->clearEmitters();

  for (int i = 0; i < 5; i++) {
    EmitterData victoryData;
    float xPos = -10.0f + (i * 5.0f);
    victoryData.position = Vector3(xPos, 3.0f, -35.0f);
    victoryData.positionVar = Vector3(1.0f, 1.0f, 1.0f);
    victoryData.emitRate = 200.0f;
    victoryData.particleLife = 2.5f;
    victoryData.particleRadius = 0.25f;
    victoryData.color = Vector4(0.0f, 1.0f, 0.2f, 1.0f);
    victoryData.damping = 0.85f;
    victoryData.velDist = VelocityDistribution::UNIFORM;
    victoryData.velMin = Vector3(-8.0f, 8.0f, -8.0f);
    victoryData.velMax = Vector3(8.0f, 25.0f, 8.0f);

    Particle* victoryModel = new Particle(Vector3(xPos, 3.0f, -35.0f),
                                          Vector3(0, 15.0f, 0),
                                          0.15f,
                                          victoryData.damping,
                                          victoryData.particleRadius,
                                          victoryData.color);

    gVictoryParticleSystem->addEmitter(victoryData, victoryModel);
  }
}

void OnGoalScored()
{
  gGoalsScored++;

  if (gGoalsScored >= 3) {
    gVictoryActive = true;
    CreateVictoryParticles();

    gGoalsScored = 0;
    if (gBall) {
      gBall->setInitialPos(PxVec3(0.0f, 1.0f, 0.0f));
    }
    if (gPowerMeter) {
      gPowerMeter->setPosition(Vector3(-8.0f, 0.5f, 0.0f));
    }
    if (gBarrier) {
      gBarrier->repositionForGoal(0.0f);
    }
  }
  else {
    float newX = (gGoalsScored == 1) ? -15.0f : 15.0f;
    float newZ = gGoalsScored * 12.0f;
    if (gBall) {
      gBall->setInitialPos(PxVec3(newX, 1.0f, newZ));
    }

    if (gPowerMeter) {
      gPowerMeter->setPosition(Vector3(newX - 8.0f, 0.5f, newZ));
    }

    if (gBarrier) {
      gBarrier->repositionForGoal(newX);
    }
  }
  if (gExplosion) {
    PxVec3 p = gSoccerField->getGoalPosition();
    gExplosion->triggerAtPosition(Vector3(p.x, p.y, p.z));
    gExplosionParticleSystem->setSystemForceActive(gExplosion, true);
    gBall->setForceActive(gExplosion, true);
  }
  UpdateCameraForGoal();

  if (gBall) {
    gBall->reset();
    gBall->setInPlay(false);
  }

  if (gPowerMeter) {
    gPowerMeter->reset();
  }

  gAimingPhase = true;
}

void stepPhysics(bool interactive, double t)
{
  PX_UNUSED(interactive);

  if (gVictoryActive) {
    display_text = "WIN!!!!!!!!!!!!!!!!!!\n"  "Press *R* to RESTART";
  }
  else {
    display_text =
      "K: Empezar/Confirmar tiro (detiene medidor), R: Reset Ball, M: Toggle Volley Mode\n"
      "T: Toggle Shot Type, V: Toggle Wind, A/D: Aim Left/Right, B: Toggle Ball Type\n"
      "El cuadrado rojo sube y baja - presiona K para fijar la potencia";
  }

  if (gExplosion->isActive()) {
    gExplosionTimer += t;
    if (gExplosionTimer >= 1.5) {
      gExplosionActive = false;
      gExplosionTimer = 0.0;
      gExplosion->deactivate();
      if (gExplosionParticleSystem) {
        gExplosionParticleSystem->clearEmitters();
      }
    }
  }

  if (gBall) {
    gBall->integrateForces(t);
  }

  if (gGoalkeeper) {
    bool ballInPlay = gBall && gBall->isInPlay();
    PxVec3 ballPos = ballInPlay ? gBall->getPos() : PxVec3(0.0f, 0.0f, 0.0f);

    if (ballInPlay) {
      gGoalkeeper->setTargetX(gBall->getPos().x);
    }
    else {
      gGoalkeeper->setTargetX(gGoalkeeper->getCenterPos().x);
    }

    gGoalkeeper->update(t, ballPos, ballInPlay);
  }

  if (gPowerMeter && gBall && !gBall->isInPlay() &&
      !gPowerMeter->getIsStopped()) {
    gPowerMeter->update(t);
  }

  if (gAimingReticle && gBall && !gBall->isInPlay()) {
    gAimingReticle->update(gBall->getPos());
  }
  if (gBarrier) {
    bool ballInPlay = gBall && gBall->isInPlay();
    PxVec3 ballPos = ballInPlay ? gBall->getPos() : PxVec3(0.0f);
    gBarrier->update(t, ballPos, ballInPlay);
  }
  if (gBallParticleSystem)
    gBallParticleSystem->update(t);

  if (gExplosionParticleSystem) {
    gExplosionParticleSystem->update(t);
  }
  if (gExplosion) {
    gExplosion->update(t);

  }
  if (gVictoryParticleSystem) {
    gVictoryParticleSystem->update(t);
  }

  if (gWindParticleSystem) {
    gWindParticleSystem->update(t);
  }

  gScene->simulate(t);
  gScene->fetchResults(true);

  if (gBall) {
    gBall->update(t);
  }
}

void cleanupPhysics(bool interactive)
{
  PX_UNUSED(interactive);

  gScene->release();
  gDispatcher->release();
  gPhysics->release();


  if (gBallParticleSystem) {
    delete gBallParticleSystem;
    gBallParticleSystem = nullptr;
  }
  if (gSoccerField) {
    delete gSoccerField;
    gSoccerField = nullptr;
  }
  if (gSoccerBall) {
    delete gSoccerBall;
    gSoccerBall = nullptr;
  }
  if (gTennisBall) {
    delete gTennisBall;
    gTennisBall = nullptr;
  }
  gBall = nullptr;

  if (gGoalkeeper) {
    delete gGoalkeeper;
    gGoalkeeper = nullptr;
  }
  if (gAimingReticle) {
    delete gAimingReticle;
    gAimingReticle = nullptr;
  }
  if (gPowerMeter) {
    delete gPowerMeter;
    gPowerMeter = nullptr;
  }
  if (gEarthGravity)
    delete gEarthGravity;
  if (gWind)
    delete gWind;
  if (gExplosion)
    delete gExplosion;

  if (gVictoryParticleSystem) {
    delete gVictoryParticleSystem;
    gVictoryParticleSystem = nullptr;
  }

  gFoundation->release();
}

void keyPress(unsigned char key, const PxTransform& camera)
{
  PX_UNUSED(camera);

  switch (toupper(key)) {
    case 'V':
      gWindEnabled = !gWindEnabled;
      if (gBall)
        gBall->setForceActive(gWind, gWindEnabled);
      if (gWindParticleSystem) {
        auto& emitters = gWindParticleSystem->getEmitters();
        for (size_t i = 0; i < emitters.size(); ++i) {
          gWindParticleSystem->setEmitterActive(i, gWindEnabled);
        }
      }
      break;

    case 'M':
      ToggleVolleyMode();
      break;


    case 'A':
      if (gAimingReticle)
        gAimingReticle->rotateLeft(RETICLEROT);
      break;

    case 'D':
      if (gAimingReticle)
        gAimingReticle->rotateRight(RETICLEROT);
      break;

    case 'K':
      Kick();
      break;

    case 'R':
      ResetProjectile();
      break;

    case 'T':
      ToggleShotType();
      break;

    case 'B':
      ToggleBallType();
      break;

    default:
      break;
  }
}

void onCollision(physx::PxActor* actor1, physx::PxActor* actor2)
{
  if (gSoccerField && gBall) {
    PxRigidStatic* goalTrigger = gSoccerField->getGoalTrigger();
    PxRigidDynamic* ball = gBall->getRigidBody();

    if ((actor1 == goalTrigger && actor2 == ball) ||
        (actor2 == goalTrigger && actor1 == ball)) {
      CreateExplosionParticles();
      gExplosionActive = true;
      gExplosionTimer = 0.0;

      OnGoalScored();
    }
  }
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
    stepPhysics(false, 0.016);
  cleanupPhysics(false);
#endif

  return 0;
}