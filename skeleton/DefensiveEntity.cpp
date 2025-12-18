#include "DefensiveEntity.h"
#include <iostream>

DefensiveEntity::DefensiveEntity(const PxVec3& pos,
                                 float width_,
                                 float height_,
                                 float depth_,
                                 const Vector4& col,
                                 PxScene* scene)
  : rb(nullptr)
  , myScene(scene)
  , renderItem(nullptr)
  , tr(nullptr)
  , color(col)
  , width(width_)
  , height(height_)
  , depth(depth_)
  , basePos(pos)
{
}

DefensiveEntity::~DefensiveEntity()
{
  if (renderItem) {
    DeregisterRenderItem(renderItem);
    delete renderItem;
    renderItem = nullptr;
  }

  if (tr) {
    delete tr;
    tr = nullptr;
  }
}

void DefensiveEntity::updateRenderTransform()
{
  if (rb && tr) {
    *tr = rb->getGlobalPose();
  }
}

PxVec3 DefensiveEntity::getPos() const
{
  if (rb) {
    return rb->getGlobalPose().p;
  }
  return basePos;
}