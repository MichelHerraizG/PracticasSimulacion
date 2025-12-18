#pragma once

#include "DefensiveEntity.h"

class Goalkeeper : public DefensiveEntity {
private:

  float moveRange;       
  float moveSpeed;         
  float targetX;        
  float currentDirection;  

  void createRigidBody(PxPhysics* physics, PxMaterial* material) override;

public:
  Goalkeeper(const PxVec3& pos,
             float width_,
             float height_,
             float depth_,
             float moveRange_,
             float moveSpeed_,
             const Vector4& col,
             PxScene* scene,
             PxPhysics* physics,
             PxMaterial* material);

  ~Goalkeeper() override = default;

  void update(double dt, const PxVec3& ballPos, bool ballInPlay) override;

  void setTargetX(float x);
  PxVec3 getCenterPos() const { return basePos; }
  float getMoveRange() const { return moveRange; }
  float getMoveSpeed() const { return moveSpeed; }
};

