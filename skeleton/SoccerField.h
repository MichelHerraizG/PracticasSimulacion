#pragma once
#include "core.hpp"
#include "RenderUtils.hpp"
#include <vector>
using namespace physx;
class SoccerField {
private:
  std::vector<const RenderItem*> m_renderItems;
  PxVec3 goalPos;
  float scaleFactor;

public:
  SoccerField(float scaleFactor = 1.0f);
  ~SoccerField();
  void createField();
  PxVec3 getGoalPosition() const { return goalPos; }
  bool isGoal(const PxVec3& ballPos) const;
};