#pragma once
#include "Particle.h"
#include "ParticleSystem.h"

class SoccerBall : public Particle {
public:
  enum ShotType { POWER_SHOT, PRECISION_SHOT };

private:
  bool inPlay;
  PxVec3 initialPos;
  ShotType currentShotType;
  ParticleSystem* trailParticles;

public:
  SoccerBall(const PxVec3& pos,
             const PxVec3& vel,
             float mass,
             float damping,
             float radius,
             const Vector4& color);

  ~SoccerBall(); 

  void kick(const PxVec3& direction, float power);
  void reset();
  void integrateForces(double dt) override;
  void setShotType(ShotType type) { currentShotType = type; }
  void SetInPlay(bool play) { inPlay = play; }  
  bool isInPlay() const { return inPlay; }

  ParticleSystem* getTrailParticles() { return trailParticles; }
};