#include "Goal.h"

Goal::Goal(PxPhysics* phys,
           PxScene* scn,
           PxMaterial* mat,
           const Vector3& pos,
           float width,
           float height,
           float postRad)
  : physics(phys)
  , scene(scn)
  , material(mat)
  , position(pos)
  , goalWidth(width)
  , postHeight(height)
  , postRadius(postRad)
{
  // Crear poste izquierdo
  PxTransform leftPostTransform(
    PxVec3(pos.x - width / 2.0f, pos.y + height / 2.0f, pos.z));
  leftPost = physics->createRigidStatic(leftPostTransform);
  PxShape* leftShape =
    CreateShape(PxCapsuleGeometry(postRad, height / 2.0f), material);
  leftShape->setLocalPose(PxTransform(PxQuat(PxHalfPi, PxVec3(0, 0, 1))));
  leftPost->attachShape(*leftShape);
  scene->addActor(*leftPost);
  leftPostRender = new RenderItem(leftShape, Vector4(1, 1, 1, 1));

  // Crear poste derecho
  PxTransform rightPostTransform(
    PxVec3(pos.x + width / 2.0f, pos.y + height / 2.0f, pos.z));
  rightPost = physics->createRigidStatic(rightPostTransform);
  PxShape* rightShape =
    CreateShape(PxCapsuleGeometry(postRad, height / 2.0f), material);
  rightShape->setLocalPose(PxTransform(PxQuat(PxHalfPi, PxVec3(0, 0, 1))));
  rightPost->attachShape(*rightShape);
  scene->addActor(*rightPost);
  rightPostRender = new RenderItem(rightShape, Vector4(1, 1, 1, 1));

  // Crear larguero
  PxTransform crossbarTransform(PxVec3(pos.x, pos.y + height, pos.z));
  crossbar = physics->createRigidStatic(crossbarTransform);
  PxShape* crossbarShape =
    CreateShape(PxCapsuleGeometry(postRad, width / 2.0f), material);
  crossbarShape->setLocalPose(PxTransform(PxQuat(PxHalfPi, PxVec3(0, 1, 0))));
  crossbar->attachShape(*crossbarShape);
  scene->addActor(*crossbar);
  crossbarRender = new RenderItem(crossbarShape, Vector4(1, 1, 1, 1));

  // Crear detección de gol
  PxTransform triggerTransform(
    PxVec3(pos.x, pos.y + height / 2.0f, pos.z - 0.5f));
  goalTrigger = physics->createRigidStatic(triggerTransform);
  PxShape* triggerShape =
    CreateShape(PxBoxGeometry(width / 2.0f, height / 2.0f, 0.5f), material);
  triggerShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false); 
  triggerShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true); 
  goalTrigger->attachShape(*triggerShape);
  scene->addActor(*goalTrigger);
}

Goal::~Goal()
{
  if (leftPostRender) {
    DeregisterRenderItem(leftPostRender);
    delete leftPostRender;
  }
  if (rightPostRender) {
    DeregisterRenderItem(rightPostRender);
    delete rightPostRender;
  }
  if (crossbarRender) {
    DeregisterRenderItem(crossbarRender);
    delete crossbarRender;
  }

}