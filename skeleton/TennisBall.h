#pragma once
#include "Ball.h"

class TennisBall : public Ball {
private:
  bool isSpinning;
  void createRigidBody(PxPhysics* physics, PxMaterial* material) override;

public:
  TennisBall(const PxVec3& pos,
             const PxVec3& vel,
             float mass,
             float damping,
             float rad,
             const Vector4& col,
             BallType type,
             ParticleSystem* system,
             PxScene* scene,
             PxPhysics* physics,
             PxMaterial* material);

  ~TennisBall();
  void launch(const PxVec3& direction, float power) override;
  void reset() override;
  void update(double dt) override;
  void setShotType(ShotType type) override;
};