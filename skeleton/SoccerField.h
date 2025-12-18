#pragma once
#include "PxPhysicsAPI.h"
#include "RenderUtils.hpp"
#include <vector>

using namespace physx;

class SoccerField {
public:
  SoccerField(float scale, PxScene* scene);
  ~SoccerField();

  PxVec3 getGoalPosition() const { return goalPos; }
  PxRigidStatic* getGoalTrigger() const { return triggerDeGol; }

private:
  void createField();
  void createGround();
  void createGoalPosts();
  void createGoalTrigger();

  PxVec3 goalPos;
  float scaleFactor;
  PxScene* myScene;

  std::vector<PxTransform*> mTrs;
  std::vector<RenderItem*> mRIs;
  std::vector<PxRigidStatic*> mActors;

  PxRigidStatic* triggerDeGol;  
};