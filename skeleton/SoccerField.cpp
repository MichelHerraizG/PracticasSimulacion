#include "SoccerField.h"
#include <iostream>
extern physx::PxPhysics* gPhysics;
extern physx::PxMaterial* gMaterial;

SoccerField::SoccerField(float scale)
  : goalPos(0, 2.5f, -35.0f)
  , scaleFactor(scale)
{
  createField();
}
SoccerField::~SoccerField()
{
  for (auto item : m_renderItems) {
    DeregisterRenderItem(item);
    delete item;
  }
}
void SoccerField::createField()
{
  PxTransform groundTransform(PxVec3(0, -0.1f, 0));
  PxBoxGeometry groundGeometry(30.0f, 0.1f, 40.0f);
  RenderItem* ground = new RenderItem(CreateShape(groundGeometry, gMaterial),
                                      Vector4(0.2f, 0.8f, 0.2f, 1.0f));
  ground->transform = new PxTransform(groundTransform);
  m_renderItems.push_back(ground);
  PxTransform leftPostTransform(
    goalPos + PxVec3(-3.66f*scaleFactor, 0, 0)); 
  PxTransform rightPostTransform(goalPos + PxVec3(3.66f * scaleFactor, 0, 0));
  PxTransform crossbarTransform(goalPos + PxVec3(0, 2.44f * scaleFactor, 0)); 

  PxBoxGeometry postGeometry(
    0.1f * scaleFactor, 2.5f * scaleFactor, 0.1f * scaleFactor);      
  PxBoxGeometry crossbarGeometry(
    3.8f * scaleFactor, 0.1f * scaleFactor, 0.1f * scaleFactor); 



  RenderItem* leftPost =
    new RenderItem(CreateShape(postGeometry, gMaterial), Vector4(1, 1, 1, 1));
  leftPost->transform = new PxTransform(leftPostTransform);
  m_renderItems.push_back(leftPost);

  RenderItem* rightPost =
    new RenderItem(CreateShape(postGeometry, gMaterial), Vector4(1, 1, 1, 1));
  rightPost->transform = new PxTransform(rightPostTransform);
  m_renderItems.push_back(rightPost);

  RenderItem* crossbar = new RenderItem(
    CreateShape(crossbarGeometry, gMaterial), Vector4(1, 1, 1, 1));
  crossbar->transform = new PxTransform(crossbarTransform);
  m_renderItems.push_back(crossbar);
}