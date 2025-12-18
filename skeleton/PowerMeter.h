#pragma once
#include "core.hpp"
#include "RenderUtils.hpp"
#include <PxPhysicsAPI.h>
using namespace physx;

class PowerMeter {
public:
  bool getIsStopped() const;
  void setPosition(const Vector3& pos);
  void setBarColor(const Vector4& color);
  void setIndicatorColor(const Vector4& color);
  float getCurrentHeight() const;
  float getNormalizedValue() const;

private:
  float minHeight;
float maxHeight;
  float currentHeight;
  float speed;
  bool movingUp;
  bool isActive;
  bool isStopped;
  PxTransform* barTransform;
  PxTransform* indicatorTransform;
  RenderItem* barItem;

  RenderItem* indicatorItem;

public:
  PowerMeter(float minH = 0.5f, float maxH = 3.0f, float spd = 2.0f);
  ~PowerMeter();
  void update(float dt);
  void stop();
  void reset();
  void setActive(bool active);

};