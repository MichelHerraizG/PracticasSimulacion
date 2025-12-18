#pragma once
#include "AnchoredSpring.h"
#include "Ball.h"
#include "SpringAnchor.h"

class SoccerBall : public Ball {
private:
  bool volleyMode;
  AnchoredSpring* volleySpring;
  SpringAnchor* volleyAnchor;
  Vector3 volleyAnchorPos;
  float springConstant;
  float restingLength;
  void createRigidBody(PxPhysics* physics, PxMaterial* material) override;

public:
  SoccerBall(const PxVec3& pos,
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

  ~SoccerBall();
  void launch(const PxVec3& direction, float power) override;
  void reset() override;
  void update(double dt) override;
  void enableVolleyMode();
  void disableVolleyMode();
  bool isVolleyMode() const { return volleyMode; }
};