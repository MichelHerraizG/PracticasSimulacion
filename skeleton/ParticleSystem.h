
#pragma once

#include <vector>
#include <random>
#include <functional>
#include "Particle.h"

using namespace physx;

enum class VelocityDistribution{UNIFORM,GAUSSIAN};

struct EmitterData {
	Vector3 position;
	Vector3 positionVar;
	float emitRate;
	float particleLife;
	float particleRadius;
	Vector4 color;
	VelocityDistribution velDist;
	Vector3 velMean;
	Vector3 velStdDev;

	Vector3 velMin;
	Vector3 velMax;
	PxVec3 gravity = PxVec3(0.0f, -9.8f, 0.0f);
	float damping = 0.99f;
	bool looping = true; // si true el emisor sigue emitiendo
};

class Emitter {
public:
	Emitter(const EmitterData& data);
	void update(double dt, std::vector<Particle*>& particles);
	const EmitterData& getData() const { return data; }
private:
	EmitterData data;
	double emitAccumulator;
	std::mt19937 rng;
};

class ParticleSystem {
public:
	ParticleSystem();
	~ParticleSystem();
	void addEmitter(const EmitterData& cfg);
	void update(double dt);
	void spawnParticle(Particle* p, float lifetime);
private:
	struct ParticleInstance {
		Particle* p;
		float lifeRemaining;
	};

	std::vector<Emitter> emitters;
	std::vector<ParticleInstance> particles;

	float removeBelowY = -50.0f;
	float removeFarDistance = 500.0f;
};