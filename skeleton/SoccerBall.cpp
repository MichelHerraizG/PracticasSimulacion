#include "SoccerBall.h"
#include <iostream>

SoccerBall::SoccerBall(const PxVec3& pos,
                       const PxVec3& vel,
                       float mass,
                       float damping,
                       float radius,
                       const Vector4& color)
  : Particle(pos, vel, mass, damping, radius, color)
  , inPlay(false)
  , initialPos(pos)
  , currentShotType(POWER_SHOT)
  , trailParticles(new ParticleSystem())
{
 
}

SoccerBall::~SoccerBall()
{
  if (trailParticles) {
    delete trailParticles;
    trailParticles = nullptr;
  }
}

void SoccerBall::kick(const PxVec3& direction, float power)
{
  PxVec3 kickDirection = direction.getNormalized();
  float kickForce;


  trailParticles->clearEmitters();

  if (currentShotType == POWER_SHOT) {
    kickForce = power * 25.0f;
    damping = 0.95f;


    EmitterData powerData;
    powerData.position = position;
    powerData.emitRate = 120.0f;
    powerData.particleLife = 0.4f;
    powerData.color = Vector4(1.0f, 0.1f, 0.1f, 1.0f);
    powerData.particleRadius = 0.06f;  
    powerData.damping = 0.7f;
    powerData.gravity = PxVec3(0.0f, -3.0f, 0.0f);
    powerData.positionVar = PxVec3(0.4f, 0.4f, 0.4f);
    powerData.velDist = VelocityDistribution::UNIFORM;
    powerData.velMin = PxVec3(-1.0f, -0.5f, -1.0f);
    powerData.velMax = PxVec3(1.0f, 2.0f, 1.0f);

    trailParticles->addEmitter(powerData);
  }
  else {
    kickForce = power * 15.0f;
    damping = 0.99f;


    EmitterData precisionData;
    precisionData.position = position;
    precisionData.emitRate = 80.0f;
    precisionData.particleLife = 0.6f;
    precisionData.color = Vector4(0.1f, 0.1f, 1.0f, 0.8f);  
    precisionData.particleRadius = 0.04f;
    precisionData.damping = 0.85f;
    precisionData.gravity = PxVec3(0.0f, -1.5f, 0.0f);
    precisionData.positionVar = PxVec3(0.25f, 0.25f, 0.25f);
    precisionData.velDist = VelocityDistribution::GAUSSIAN;
    precisionData.velMean = PxVec3(0.0f, 0.5f, 0.0f);
    precisionData.velStdDev = PxVec3(0.5f, 0.3f, 0.5f);

    trailParticles->addEmitter(precisionData);
  }

  clearForceAccumulator();
  velocity = kickDirection * kickForce;
  inPlay = true;
}

void SoccerBall::reset()
{
  position = initialPos;
  velocity = PxVec3(0, 0, 0);
  forceAccumulator = PxVec3(0, 0, 0);
  inPlay = false;
  damping = 0.99f;
  transform = PxTransform(position);

  if (trailParticles) {
    trailParticles->clearEmitters();
  }
}

void SoccerBall::integrateForces(double dt)
{
  if (inPlay) {

    if (trailParticles && !trailParticles->getEmitters().empty()) {
      auto& emitters = trailParticles->getEmitters();
      for (auto& emitter : emitters) {
        emitter.setPosition(position);
      }
    }


    Particle::integrateForces(dt);

  
    if (trailParticles) {
      trailParticles->update(dt);
    }

    
    if (velocity.magnitude() < 0.5f) {
      inPlay = false;
      if (trailParticles) {
        trailParticles->clearEmitters();
      }
    }
  }
}