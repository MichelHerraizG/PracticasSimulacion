#include "Goalkeeper.h"
#include <iostream>

Goalkeeper::Goalkeeper(const PxVec3& pos,
                       float width_,
                       float height_,
                       float depth_,
                       float moveRange_,
                       float moveSpeed_,
                       const Vector4& col,
                       PxScene* scene,
                       PxPhysics* physics,
                       PxMaterial* material)
  : DefensiveEntity(pos, width_, height_, depth_, col, scene)
  , moveRange(moveRange_)
  , moveSpeed(moveSpeed_)
  , currentDirection(1.0f)
  , targetX(pos.x)
{
  if (physics && material) {
    createRigidBody(physics, material);
  }



}

void Goalkeeper::createRigidBody(PxPhysics* physics, PxMaterial* material)
{
  PxTransform startTransform(basePos);
  rb = physics->createRigidDynamic(startTransform);

  PxBoxGeometry boxGeom(width * 0.5f, height * 0.5f, depth * 0.5f);
  PxShape* shape = physics->createShape(boxGeom, *material);
  rb->attachShape(*shape);

  rb->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

  if (myScene) {
    myScene->addActor(*rb);
  }

  tr = new PxTransform(startTransform);
  renderItem = new RenderItem(shape, color);
  renderItem->transform = tr;
  RegisterRenderItem(renderItem);

  shape->release();
}

void Goalkeeper::setTargetX(float x)
{
  targetX = x;
}

void Goalkeeper::update(double dt, const PxVec3& ballPos, bool ballInPlay)
{
  if (!rb)
    return;

  PxTransform pose = rb->getGlobalPose();
  float desiredX = ballInPlay ? ballPos.x : basePos.x;
  float minX = basePos.x - moveRange;
  float maxX = basePos.x + moveRange;
  desiredX = PxClamp(desiredX, minX, maxX);
  float diff = desiredX - pose.p.x;
  float maxStep = moveSpeed * static_cast<float>(dt);
  diff = PxClamp(diff, -maxStep, maxStep);

  pose.p.x += diff;

  rb->setKinematicTarget(pose);
  updateRenderTransform();
}