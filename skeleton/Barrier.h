#pragma once

#include "DefensiveEntity.h"

class Barrier : public DefensiveEntity {
private:
  float jumpForce;
  float detectionRadius;
  bool isJumping;         
  float groundCheckTimer;  
  float lastBallZ; 

  void createRigidBody(PxPhysics* physics, PxMaterial* material) override;

public:
  Barrier(const PxVec3& pos,
          float width_,
          float height_,
          float depth_,
          float jumpForce_,
          float detectionRadius_,
          const Vector4& col,
          PxScene* scene,
          PxPhysics* physics,
          PxMaterial* material);

  void update(double dt, const PxVec3& ballPos, bool ballInPlay) override;


  void repositionForGoal(float ballX);
};