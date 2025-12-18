#pragma once
#include "AnchoredSpring.h"
#include "ParticleSystem.h"
#include "RenderUtils.hpp"
#include "SpringAnchor.h"
#include "core.hpp"
#include <PxPhysicsAPI.h>
#include <vector>

using namespace physx;

class ForceType;

enum BallType { STANDARD_BALL, LIGHT_BALL };
enum ShotType { POWER_SHOT, PRECISION_SHOT };

class Ball {
protected:
  PxRigidDynamic* rb;
  PxScene* myScene;
  RenderItem* renderItem;
  PxTransform* tr;
  BallType ballType;
  ShotType currentShotType;
  float radius;
  float baseMass;
  Vector4 color;
  bool inPlay;
  PxVec3 initialPos;
  PxVec3 initialVel;
  ParticleSystem* parSys;
  int auraIndex;
  std::vector<ForceType*> forces;
  std::vector<bool> activeForces;
  std::vector<bool>
    alwaysApply; 
  virtual void createRigidBody(PxPhysics* physics, PxMaterial* material) = 0;
  virtual void updateAura();
  void updateRenderTransform();

public:
  Ball(const PxVec3& pos,
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

  virtual ~Ball();
  virtual void launch(const PxVec3& direction, float power) = 0;
  virtual void reset();
  virtual void update(double dt);
  virtual void setBallType(BallType type);
  virtual void setShotType(ShotType type);
  void addForceType(ForceType* force, bool active, bool applyAlways = false);
  void removeForceType(ForceType* force);
  void setForceActive(ForceType* force, bool active);
  void clearForces();
  RenderItem* getRenderItem() { return renderItem; }

  void integrateForces(double dt);
  PxVec3 getPos() const;
  PxVec3 getVel() const;
  bool isInPlay() const { return inPlay; }
  PxRigidDynamic* getRigidBody() { return rb; }
  BallType getBallType() const { return ballType; }
  ShotType getShotType() const { return currentShotType; }
  float getRadius() const { return radius; }
  float getMass() const { return baseMass; }
  void setInPlay(bool play) { inPlay = play; }
  void setInitialPos(const PxVec3& pos);
};