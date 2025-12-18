#pragma once

#include "RenderUtils.hpp"
#include "core.hpp"
#include <PxPhysicsAPI.h>

using namespace physx;

class DefensiveEntity {
protected:
  PxRigidDynamic* rb;
  PxScene* myScene;
  RenderItem* renderItem;
  PxTransform* tr;
  Vector4 color;
  float width;
  float height;
  float depth;
  PxVec3 basePos;

  virtual void createRigidBody(PxPhysics* physics, PxMaterial* material) = 0;

public:
  DefensiveEntity(const PxVec3& pos,
                  float width_,
                  float height_,
                  float depth_,
                  const Vector4& col,
                  PxScene* scene);

  virtual ~DefensiveEntity();

  virtual void update(double dt, const PxVec3& ballPos, bool ballInPlay) = 0;

  void updateRenderTransform(); 

  PxVec3 getPos() const;
  PxVec3 getBasePos() const { return basePos; }
  PxRigidDynamic* getRigidBody() const { return rb; }

  float getWidth() const { return width; }
  float getHeight() const { return height; }
  float getDepth() const { return depth; }
};