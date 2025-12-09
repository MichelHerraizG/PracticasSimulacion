#pragma once
#include "ParticleSystem.h"
#include "RenderUtils.hpp"
#include <PxPhysicsAPI.h>
#include <vector>

enum BallType { LIGHT_BALL, STANDARD_BALL };
enum ShotType { POWER_SHOT, PRECISION_SHOT };

class ForceType;

using namespace physx;

class SoccerBall {
private:
  PxRigidDynamic* rb;
  PxScene* myScene;
  RenderItem* renderItem;
  PxTransform* tr;

  float radius;
  float baseMass;
  Vector4 color;

  BallType ballType;
  ShotType currentShotType;

  bool inPlay;
  PxVec3 initialPos;
  PxVec3 initialVel;

  ParticleSystem* parSys;
  int auraIndex;

  std::vector<ForceType*> forces;
  std::vector<bool> activeForces;

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

  void launch(const PxVec3& direction, float power);
  void reset();
  void setBallType(BallType type);
  void setShotType(ShotType type);
  void addForceType(ForceType* force, bool active = true);
  void setForceActive(ForceType* force, bool active);
  void clearForces();
  void integrateForces(double dt);
  void update(double dt);

  RenderItem* getRenderItem() { return renderItem; }
  PxRigidDynamic* getRigidBody() { return rb; }

  PxVec3 getPos() const;
  PxVec3 getVel() const;

  void setInPlay(bool playing) { inPlay = playing; }
  bool isInPlay() const { return inPlay; }
  BallType getBallType() const { return ballType; }



private:
  void updateAura();
  void updateRenderTransform();
  void createRigidBody(PxPhysics* physics, PxMaterial* material);
};
