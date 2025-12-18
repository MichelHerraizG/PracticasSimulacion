#include "SoccerField.h"
#include <iostream>
extern physx::PxPhysics* gPhysics;
extern physx::PxMaterial* gMaterial;

SoccerField::SoccerField(float scale, PxScene* scene)
  : goalPos(0, 2.5f, -35.0f)
  , scaleFactor(scale)
  , myScene(scene)
  , triggerDeGol(nullptr)
{
  createField();
}

SoccerField::~SoccerField()
{
  for (auto t : mTrs) {
    delete t;
  }
  mTrs.clear();

  for (auto item : mRIs) {
    DeregisterRenderItem(item);
    delete item;
  }
  mRIs.clear();
  mActors.clear();
  triggerDeGol = nullptr;
}

void SoccerField::createField()
{
  createGround();
  createGoalPosts();
  createGoalTrigger();

}
void SoccerField::createGround()
{
  // CREO ACTOR
  PxTransform groundTransform(PxVec3(0, -0.1f, 0));
  PxRigidStatic* groundActor = gPhysics->createRigidStatic(groundTransform);
  
  // CREO GEOMETRÍA
  PxBoxGeometry groundGeometry(30.0f, 0.1f, 40.0f);
  PxShape* groundShape = CreateShape(groundGeometry, gMaterial);

  groundActor->attachShape(*groundShape);

  myScene->addActor(*groundActor);
  mActors.push_back(groundActor);

  // CREO EL RENDERITEM
  PxTransform* renderTransform = new PxTransform(groundTransform);
  RenderItem* ground =
    new RenderItem(groundShape, Vector4(0.2f, 0.8f, 0.2f, 1.0f));
  ground->transform = renderTransform;

  mRIs.push_back(ground);
  mTrs.push_back(renderTransform);

  groundShape->release();
}

void SoccerField::createGoalPosts()
{
  PxBoxGeometry postGeometry(
    0.1f * scaleFactor, 2.5f * scaleFactor, 0.1f * scaleFactor);
  PxBoxGeometry crossbarGeometry(
    3.8f * scaleFactor, 0.1f * scaleFactor, 0.1f * scaleFactor);


  PxVec3 leftPostPos = goalPos + PxVec3(-3.66f * scaleFactor, 0, 0);
  PxTransform leftPostTransform(leftPostPos);
  PxRigidStatic* leftPostActor = gPhysics->createRigidStatic(leftPostTransform);

  PxShape* leftPostShape = CreateShape(postGeometry, gMaterial);
  leftPostActor->attachShape(*leftPostShape);
  myScene->addActor(*leftPostActor);
  mActors.push_back(leftPostActor);

  PxTransform* leftPostRenderTransform = new PxTransform(leftPostTransform);
  RenderItem* leftPost = new RenderItem(leftPostShape, Vector4(1, 1, 1, 1));
  leftPost->transform = leftPostRenderTransform;
  mRIs.push_back(leftPost);
  mTrs.push_back(leftPostRenderTransform);
  leftPostShape->release();

  PxVec3 rightPostPos = goalPos + PxVec3(3.66f * scaleFactor, 0, 0);
  PxTransform rightPostTransform(rightPostPos);
  PxRigidStatic* rightPostActor =
    gPhysics->createRigidStatic(rightPostTransform);

  PxShape* rightPostShape = CreateShape(postGeometry, gMaterial);
  rightPostActor->attachShape(*rightPostShape);
  myScene->addActor(*rightPostActor);
  mActors.push_back(rightPostActor);

  PxTransform* rightPostRenderTransform = new PxTransform(rightPostTransform);
  RenderItem* rightPost = new RenderItem(rightPostShape, Vector4(1, 1, 1, 1));
  rightPost->transform = rightPostRenderTransform;
  mRIs.push_back(rightPost);
  mTrs.push_back(rightPostRenderTransform);
  rightPostShape->release();

    PxVec3 crossbarPos = goalPos + PxVec3(0, 2.44f * scaleFactor, 0);
  PxTransform crossbarTransform(crossbarPos);
  PxRigidStatic* crossbarActor = gPhysics->createRigidStatic(crossbarTransform);

  PxShape* crossbarShape = CreateShape(crossbarGeometry, gMaterial);
  crossbarActor->attachShape(*crossbarShape);
  myScene->addActor(*crossbarActor);
  mActors.push_back(crossbarActor);

  PxTransform* crossbarRenderTransform = new PxTransform(crossbarTransform);
  RenderItem* crossbar = new RenderItem(crossbarShape, Vector4(1, 1, 1, 1));
  crossbar->transform = crossbarRenderTransform;
  mRIs.push_back(crossbar);
  mTrs.push_back(crossbarRenderTransform);
  crossbarShape->release();
}

void SoccerField::createGoalTrigger()
{
  PxVec3 triggerPos = goalPos + PxVec3(0, 0.8f * scaleFactor, -0.5f);
  PxTransform triggerTransform(triggerPos);


  triggerDeGol = gPhysics->createRigidStatic(triggerTransform);


  PxBoxGeometry triggerGeometry(3.66f * scaleFactor,  
                                1.8f * scaleFactor,  
                                0.5f);                

  PxShape* triggerShape = CreateShape(triggerGeometry, gMaterial);


  triggerShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
  triggerShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);


  triggerDeGol->attachShape(*triggerShape);


  myScene->addActor(*triggerDeGol);
  PxTransform* triggerRenderTransform = new PxTransform(triggerTransform);
  RenderItem* triggerRender = new RenderItem(
    triggerShape, Vector4(1.0f, 1.0f, 0.0f, 0.3f));  
  triggerRender->transform = triggerRenderTransform;
  mRIs.push_back(triggerRender);
  mTrs.push_back(triggerRenderTransform);

  triggerShape->release();
}