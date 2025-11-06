#pragma once
#include "Projectile.h"
#include "ParticleSystem.h"
enum BallType { LIGHT_BALL, STANDARD_BALL };
enum ShotType { POWER_SHOT, PRECISION_SHOT };
using namespace physx;
class SoccerBall : public Projectile {
private:
  BallType ballType;
  ShotType currentShotType;
  ParticleSystem* parSys;
  Emitter* auraEmitter;
  bool auraActive;
  int auraIndex;

public:
  SoccerBall(const PxVec3& pos,
             const PxVec3& vel,
             float mass = 0.43f,
             float damping = 0.99f,
             float radius = 0.11f,
             const Vector4& color = Vector4(1.0f, 1.0f, 1.0f, 1.0f),
             BallType type = STANDARD_BALL,
             ParticleSystem* system = nullptr);

 ~SoccerBall();
  void launch(const PxVec3& direction, float power) override;
  void setBallType(BallType type);
  void reset() override;
  void integrateForces(double dt) override;
  void setShotType(ShotType type);
  void updateAura();

};