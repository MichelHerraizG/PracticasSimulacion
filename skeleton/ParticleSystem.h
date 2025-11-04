// ParticleSystem.h
#pragma once
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
  double emitAccumulator;
  std::mt19937 rng;

public:
  Emitter(const EmitterData& data_);
  void update(double dt, std::vector<Particle*>& particles);


  void setPosition(const Vector3& newPos) { data.position = newPos; }
  const EmitterData& getData() const { return data; }
  EmitterData& getData() { return data; }
};

struct ParticleInstance {
  Particle* p = nullptr;
  float lifeRemaining = 0.0f;
};

class ParticleSystem {
private:
  std::vector<ParticleInstance> particles;
  std::vector<Emitter> emitters;
  float removeBelowY = -100.0f;
  float removeFarDistance = 1000.0f;

public:
  ParticleSystem();
  ~ParticleSystem();

  void addEmitter(const EmitterData& cfg);
  void spawnParticle(Particle* p, float lifetime);
  void update(double dt);

  void clearEmitters() { emitters.clear(); }
  std::vector<Emitter>& getEmitters() { return emitters; }
  const std::vector<Emitter>& getEmitters() const { return emitters; }
};