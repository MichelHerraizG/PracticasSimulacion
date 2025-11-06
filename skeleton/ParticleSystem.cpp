#include "ParticleSystem.h"
#include "RenderUtils.hpp"
#include <algorithm>
#include <cmath>
#include <random>

using namespace physx;
Emitter::Emitter(const EmitterData& data_, Particle* model_)
  : data(data_)
  , model(model_)
  , emitAccumulator(0.0)
{
  std::random_device rd;
  rng.seed(rd());
}

void Emitter::update(double dt, std::vector<Particle*>& particles)
{
  if (data.emitRate <= 0.0f || !model)
    return;

  emitAccumulator += data.emitRate * dt;
  int toEmit = static_cast<int>(std::floor(emitAccumulator));
  emitAccumulator -= toEmit;

  for (int i = 0; i < toEmit; ++i) {
    std::uniform_real_distribution<float> dx(-data.positionVar.x,
                                             data.positionVar.x);
    std::uniform_real_distribution<float> dy(-data.positionVar.y,
                                             data.positionVar.y);
    std::uniform_real_distribution<float> dz(-data.positionVar.z,
                                             data.positionVar.z);
    Vector3 offset(dx(rng), dy(rng), dz(rng));
    Vector3 pos = model->getPos() + offset;

    PxVec3 vel;
    if (data.velDist == VelocityDistribution::GAUSSIAN) {
      std::normal_distribution<float> nx(data.velMean.x, data.velStdDev.x);
      std::normal_distribution<float> ny(data.velMean.y, data.velStdDev.y);
      std::normal_distribution<float> nz(data.velMean.z, data.velStdDev.z);
      vel = PxVec3(nx(rng), ny(rng), nz(rng));
    }
    else {
      std::uniform_real_distribution<float> ux(data.velMin.x, data.velMax.x);
      std::uniform_real_distribution<float> uy(data.velMin.y, data.velMax.y);
      std::uniform_real_distribution<float> uz(data.velMin.z, data.velMax.z);
      vel = PxVec3(ux(rng), uy(rng), uz(rng));
    }

    Particle* p = new Particle(pos,
                               vel,
                               model->getMass(),
                               model->getDamping(),
                               model->getRadius(),
                               data.color);

    particles.push_back(p);
  }
}
ParticleSystem::ParticleSystem()
{
}
ParticleSystem::~ParticleSystem()
{
  for (auto& pi : particles) {
    delete pi.p;
  }
  particles.clear();
  emitters.clear();
}

void ParticleSystem::addEmitter(const EmitterData& cfg, Particle* model)
{
  emitters.emplace_back(cfg, model);
}

void ParticleSystem::spawnParticle(Particle* p, float lifetime)
{
  ParticleInstance pi;
  pi.p = p;
  pi.lifeRemaining = lifetime;
  regParticleSysForce(p);
  particles.push_back(pi);
}

void ParticleSystem::update(double dt)
{
  for (auto& e : emitters) {
    std::vector<Particle*> newParticles;
    e.update(dt, newParticles);
    for (auto* p : newParticles) {
      ParticleInstance pi;
      pi.p = p;
      pi.lifeRemaining = e.getData().particleLife;
      regParticleSysForce(p);
      particles.push_back(pi);
    }
  }
  std::vector<ParticleInstance> alive;
  alive.reserve(particles.size());
  for (auto& pi : particles) {
    if (!pi.p)
      continue;

    pi.p->integrateForces(dt);
    pi.lifeRemaining -= static_cast<float>(dt);

    Vector3 pos = pi.p->getPos();
    float dist2 = pos.x * pos.x + pos.y * pos.y + pos.z * pos.z;

    bool dead = (pi.lifeRemaining <= 0.0f || pos.y < removeBelowY ||
                 dist2 > removeFarDistance * removeFarDistance);

    if (dead) {
      delete pi.p;
      pi.p = nullptr;
    }
    else {
      alive.push_back(pi);
    }
  }
  particles.swap(alive);
}

void ParticleSystem::addSystemForce(ForceType* force, bool active)
{
  systemForces.push_back({ force, active });

  for (auto& pi : particles) {
    if (pi.p) {
      pi.p->addForceType(force, active);
    }
  }
}

void ParticleSystem::removeSystemForce(ForceType* force)
{
  systemForces.erase(std::remove_if(systemForces.begin(),
                                    systemForces.end(),
                                    [force](const SystemForce& sf) {
                                      return sf.force == force;
                                    }),
                     systemForces.end());

  for (auto& pi : particles) {
    if (pi.p) {
      pi.p->removeForceType(force);
    }
  }
}

void ParticleSystem::setSystemForceActive(ForceType* force, bool active)
{
  for (auto& sf : systemForces) {
    if (sf.force == force) {
      sf.active = active;
      break;
    }
  }

  for (auto& pi : particles) {
    if (pi.p && pi.p->getForceGenerator()) {
      pi.p->getForceGenerator()->setActive(force, active);
    }
  }
}

void ParticleSystem::regParticleSysForce(Particle* particle)
{
  for (const auto& sf : systemForces) {
    particle->addForceType(sf.force, sf.active);
  }
}