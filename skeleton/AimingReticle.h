#pragma once

#include "RenderUtils.hpp"
#include "core.hpp"
using namespace physx;
class AimingReticle {
private:
  RenderItem* circleItem;
  RenderItem* directionLine;
  PxTransform circleTransform;
  PxTransform lineTransform;
  bool visible;
  float baseRadius;
  float currentRadius;

public:
  AimingReticle(float radius = 1.0f);
  ~AimingReticle();

  void setVisible(bool isVisible);
  void update(const PxVec3& position,
              const PxVec3& direction,
              float power = 1.0f);
  void setColor(const Vector4& color);
};