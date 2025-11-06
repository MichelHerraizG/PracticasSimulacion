// ParticleSystem.h
#pragma once
#include "ForceGenerador.h"
#include "Particle.h"
#include <random>
#include <vector>

enum class VelocityDistribution { UNIFORM, GAUSSIAN };

struct EmitterData {
  Vector3 position = Vector3(0, 0, 0);
  Vector3 positionVar = Vector3(0.1f, 0.1f, 0.1f);
  float emitRate = 10.0f;
  float particleLife = 2.0f;
  float particleRadius = 0.1f;
  Vector4 color = Vector4(1, 1, 1, 1);
  float damping = 0.99f;
  Vector3 gravity = Vector3(0, -9.8f, 0);

  VelocityDistribution velDist = VelocityDistribution::UNIFORM;
  Vector3 velMin = Vector3(-1, -1, -1);
  Vector3 velMax = Vector3(1, 1, 1);
  Vector3 velMean = Vector3(0, 0, 0);
  Vector3 velStdDev = Vector3(1, 1, 1);
};

class Emitter {
private:
    EmitterData data;
    Particle* model;
    double emitAccumulator;
    std::mt19937 rng;
    bool active;
    int id;

public:
    Emitter(const EmitterData& data_, Particle* model_, int id_);
    void update(double dt, std::vector<Particle*>& particles);
    const EmitterData& getData() const { return data; }
    int getId() const { return id; }

    void updateData(const EmitterData& newData) { data = newData; }
    void setPosition(const Vector3& pos)
    {
        if (model) {
            model->setPos(pos);
        }
    }
    void setActive(bool act) { active = act; }
    bool isActive() const { return active; }
};


struct SystemForce
{
  ForceType* force;
  bool active;
};


struct ParticleInstance {
  Particle* p = nullptr;
  float lifeRemaining = 0.0f;
};

class ParticleSystem {
private:
  std::vector<SystemForce> systemForces;
  std::vector<ParticleInstance> particles;
  std::vector<Emitter> emitters;
  float removeBelowY = -100.0f;
  float removeFarDistance = 1000.0f;
  ForceGenerador forceGenerador;
  void regParticleSysForce(Particle* particle);
  int nextEmitterId = 0;
public:
  ParticleSystem();
  ~ParticleSystem();

  int addEmitter(const EmitterData& cfg, Particle* model);
  void spawnParticle(Particle* p, float lifetime);
  void update(double dt);
  void addSystemForce(ForceType* force, bool active = true);
  void removeSystemForce(ForceType* force);
  void setSystemForceActive(ForceType* force, bool active);
  void setEmitterActive(int id, bool active);
  void updateEmitterPosition(int id, const Vector3& pos);
  void updateEmitterData(int id, const EmitterData& data);
  bool isEmitterActive(int id) const;
  void clearEmitters() { emitters.clear(); nextEmitterId = 0; }
  std::vector<Emitter>& getEmitters() { return emitters; }
};