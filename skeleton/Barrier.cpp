#include "Barrier.h"
#include <iostream>

const float ALTOFFSET = 0.5f;
Barrier::Barrier(const PxVec3& pos,
                 float width_,
                 float height_,
                 float depth_,
                 float jumpForce_,
                 float detectionRadius_,
                 const Vector4& col,
                 PxScene* scene,
                 PxPhysics* physics,
                 PxMaterial* material)
  : DefensiveEntity(pos, width_, height_, depth_, col, scene)
  , jumpForce(jumpForce_)
  , detectionRadius(detectionRadius_)
  , isJumping(false)
  , groundCheckTimer(0.0f)
  , lastBallZ(1000.0f) 
{
  if (physics && material) {
    createRigidBody(physics, material);
  }
}

void Barrier::createRigidBody(PxPhysics* physics, PxMaterial* material)
{
  PxTransform startTransform(basePos);
  rb = physics->createRigidDynamic(startTransform);

  PxBoxGeometry boxGeom(width * 0.5f, height * 0.5f, depth * 0.5f);
  PxShape* shape = physics->createShape(boxGeom, *material);
  rb->attachShape(*shape);
  shape->release();
  rb->setMass(50.0f);
  rb->setLinearDamping(0.3f);
  rb->setAngularDamping(0.8f);

  rb->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
  rb->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y,
                              true);  
  rb->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true);

  myScene->addActor(*rb);

  tr = new PxTransform(startTransform);
  renderItem = new RenderItem(shape, rb, color);
  renderItem->transform = tr;
  RegisterRenderItem(renderItem);
}

void Barrier::update(double dt, const PxVec3& ballPos, bool ballInPlay)
{
  if (!rb)
    return;

  PxVec3 currentPos = rb->getGlobalPose().p;

  if (!ballInPlay) {
    isJumping = false;
    groundCheckTimer = 0.0f;
    lastBallZ = 1000.0f;

    PxVec3 toBase = basePos - currentPos;
    if (toBase.magnitude() > 0.1f) {
      rb->setLinearVelocity(toBase * 2.0f);
    }

    updateRenderTransform();
    return;
  }

  PxVec3 toBalFlat = ballPos - currentPos;
  toBalFlat.y = 0.0f;
  float distance = toBalFlat.magnitude();

  bool ballApproaching = ballPos.z < lastBallZ;
  lastBallZ = ballPos.z;

  if (!isJumping && ballApproaching && distance <= detectionRadius) {
    if (currentPos.y < basePos.y + 1.0f) {
      rb->setLinearVelocity(PxVec3(0, 0, 0));
      rb->addForce(PxVec3(0, jumpForce, 0), PxForceMode::eIMPULSE);
      isJumping = true;
      groundCheckTimer = 0.0f;
    }
  }

  if (isJumping) {
    groundCheckTimer += dt;

    if (groundCheckTimer > 0.3f && currentPos.y <= basePos.y + ALTOFFSET) {
      isJumping = false;
    }
  }

  updateRenderTransform();
}

void Barrier::repositionForGoal(float ballX)
{
  if (!rb)
    return;

  float barrierOffset = ballX * 0.6f;


  const float maxOffset = 4.0f;
  if (barrierOffset > maxOffset)
    barrierOffset = maxOffset;
  if (barrierOffset < -maxOffset)
    barrierOffset = -maxOffset;


  PxVec3 newBasePos = PxVec3(barrierOffset, basePos.y, basePos.z);
  basePos = newBasePos;


  PxTransform newTransform(newBasePos);
  rb->setGlobalPose(newTransform);
  rb->setLinearVelocity(PxVec3(0, 0, 0));
  rb->setAngularVelocity(PxVec3(0, 0, 0));

  isJumping = false;
  groundCheckTimer = 0.0f;
  lastBallZ = 1000.0f;


}