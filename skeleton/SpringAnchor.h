#pragma once
#include "RenderUtils.hpp"
#include "core.hpp"
#include <PxPhysicsAPI.h>

using namespace physx;

class SpringAnchor {
private:
  Vector3 position;
  PxTransform transform;
  RenderItem* renderItem;
  float size;
  Vector4 color;

public:
  SpringAnchor(const Vector3& pos,
               float cubeSize = 0.2f,
               const Vector4& col = Vector4(1.0f, 0.0f, 0.0f, 1.0f))
    : position(pos)
    , transform(PxTransform(PxVec3(pos.x, pos.y, pos.z)))
    , renderItem(nullptr)
    , size(cubeSize)
    , color(col)
  {
    PxBoxGeometry box(size, size, size);
    renderItem = new RenderItem(CreateShape(box), color);
    renderItem->transform = &transform;
  }

  ~SpringAnchor()
  {
    if (renderItem) {
      DeregisterRenderItem(renderItem);
      delete renderItem;
    }
  }

  const Vector3& getPosition() const { return position; }

  void setPosition(const Vector3& newPos)
  {
    position = newPos;
    transform = PxTransform(PxVec3(position.x, position.y, position.z));
  }
};