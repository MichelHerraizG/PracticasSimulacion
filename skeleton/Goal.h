#pragma once
#pragma once
#include "RenderUtils.hpp"
#include <PxPhysicsAPI.h>
#include <vector>

using namespace physx;

class Goal
{
private:
  PxRigidStatic* leftPost;
  PxRigidStatic* rightPost;
  PxRigidStatic* crossbar;
  PxRigidStatic* goalTrigger;

  RenderItem* leftPostRender;
  RenderItem* rightPostRender;
  RenderItem* crossbarRender;

   float postHeight;
  float postRadius;
  float goalWidth;
  Vector3 position;

  PxPhysics* physics;
  PxScene* scene;
  PxMaterial* material;

public:
  Goal(PxPhysics* phys,
       PxScene* scn,
       PxMaterial* mat,
       const Vector3& pos,
       float width = 7.32f,
       float height = 2.44f,
       float postRad = 0.12f);
  ~Goal();
  PxRigidStatic* getTrigger() const { return goalTrigger; }
  Vector3 getPosition() const { return position; }
  float getWidth() const { return goalWidth; }
  float getHeight() const { return postHeight; }
  bool isInsideGoal(const PxVec3& ballPos) const;
};