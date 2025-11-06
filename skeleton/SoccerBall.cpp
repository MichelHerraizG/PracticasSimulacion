#include "SoccerBall.h"
#include <iostream>

SoccerBall::SoccerBall(const PxVec3& pos,
    const PxVec3& vel,
    float mass,
    float damping,
    float radius,
    const Vector4& color,
    BallType type,
    ParticleSystem* system)
    : Projectile(pos, vel, mass, damping, radius, color)
    , ballType(type)
    , currentShotType(POWER_SHOT)
    , parSys(system)
    , auraIndex(-1)
{
    setBallType(type);
    scalePhysics(0.95f);
    if (parSys) {
        EmitterData cfg;
        cfg.emitRate = 100.0f;
        cfg.particleLife = 1.0f;
        cfg.positionVar = Vector3(0.3f, 0.2f, 0.3f);
        cfg.velDist = VelocityDistribution::GAUSSIAN;
        cfg.velMean = Vector3(0.0f, 4.0f, 0.0f);
        cfg.velStdDev = Vector3(0.2f, 0.6f, 0.2f);

        cfg.particleRadius = 0.08f;
        cfg.damping = 0.8f;
        cfg.gravity = Vector3(0.0f, -4.0f, 0.0f);

        // Color inicial rojo para POWER_SHOT
        cfg.color = Vector4(1.0f, 0.2f, 0.1f, 0.8f);

        Particle* auraModel = new Particle(Vector3(pos.x, pos.y, pos.z),
            Vector3(0.0f),
            cfg.gravity,
            cfg.damping,
            cfg.particleRadius,
            cfg.color);

        // El ParticleSystem devuelve el ID y gestiona el emitter
        auraIndex = parSys->addEmitter(cfg, auraModel);
        parSys->setEmitterActive(auraIndex, true);
    }
}

SoccerBall::~SoccerBall()
{
}

void SoccerBall::setBallType(BallType type)
{
    ballType = type;

    if (type == LIGHT_BALL) {

        setMass(0.05f);
        damping = 0.95f;
        setScale(0.5f);
    }
    else {
        setMass(0.90f);
        damping = 0.99f;
        setScale(1.0f);
    }
}

void SoccerBall::launch(const PxVec3& direction, float power)
{
    PxVec3 dir = direction.getNormalized();
    float kickForce = 0.0f;

    if (parSys && auraIndex >= 0) {
        EmitterData currentData = parSys->getEmitters()[auraIndex].getData();
        currentData.emitRate = 120.0f;
        parSys->updateEmitterData(auraIndex, currentData);
    }

    if (ballType == LIGHT_BALL) {
        if (currentShotType == POWER_SHOT) {
            kickForce = power * 35.0f;
            damping = 0.92f;
        }
        else {
            kickForce = power * 25.0f;
            damping = 0.96f;
        }
    }
    else {
        if (currentShotType == POWER_SHOT) {
            kickForce = power * 45.0f;
            damping = 0.92f;
        }
        else {
            kickForce = power * 30.0f;
            damping = 0.97f;
        }
    }

    velocity = dir * kickForce;
 
    clearForceAccumulator();
    inPlay = true;
}

void SoccerBall::reset()
{
    Projectile::reset();

  
    if (parSys && auraIndex >= 0) {
        EmitterData currentData = parSys->getEmitters()[auraIndex].getData();
        currentData.emitRate = 80.0f;
        parSys->updateEmitterData(auraIndex, currentData);
    }
}

void SoccerBall::updateAura()
{
    if (parSys && auraIndex >= 0) {
        PxVec3 currentPos = getPos();
        parSys->updateEmitterPosition(
            auraIndex,
            Vector3(currentPos.x, currentPos.y + 0.1f, currentPos.z));
    }
}

void SoccerBall::integrateForces(double dt)
{
    updateAura();

    if (!inPlay) {
        return;
    }

    Projectile::integrateForces(dt);
}

void SoccerBall::setShotType(ShotType type)
{
    currentShotType = type;

    if (parSys && auraIndex >= 0) {
        Vector4 newColor;
        if (type == POWER_SHOT) {
            newColor = Vector4(1.0f, 0.0f, 0.0f, 0.7f);
        }
        else if (type == PRECISION_SHOT) {
            newColor = Vector4(0.0f, 0.5f, 1.0f, 0.7f);
        }

        EmitterData newData = parSys->getEmitters()[auraIndex].getData();
        newData.color = newColor;
        parSys->updateEmitterData(auraIndex, newData);
    }
}