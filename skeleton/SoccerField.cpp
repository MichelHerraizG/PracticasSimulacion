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

  for (auto t : m_transforms) {
    delete t;
  }
  m_transforms.clear();

  for (auto item : m_renderItems) {
    DeregisterRenderItem(item);
    delete item;
  }
}

void SoccerField::createField()
{
  // SUELO
  PxTransform* groundTransform = new PxTransform(PxVec3(0, -0.1f, 0));
  PxBoxGeometry groundGeometry(30.0f, 0.1f, 40.0f);
  RenderItem* ground = new RenderItem(CreateShape(groundGeometry, gMaterial),
                                      Vector4(0.2f, 0.8f, 0.2f, 1.0f));
  ground->transform = groundTransform;
  m_renderItems.push_back(ground);
  m_transforms.push_back(groundTransform); 

  // POSTE IZQUIERDO
  PxTransform* leftPostTransform =
    new PxTransform(goalPos + PxVec3(-3.66f * scaleFactor, 0, 0));
  PxBoxGeometry postGeometry(
    0.1f * scaleFactor, 2.5f * scaleFactor, 0.1f * scaleFactor);
  RenderItem* leftPost =
    new RenderItem(CreateShape(postGeometry, gMaterial), Vector4(1, 1, 1, 1));
  leftPost->transform = leftPostTransform;
  m_renderItems.push_back(leftPost);
  m_transforms.push_back(leftPostTransform); 

  // POSTE DERECHO
  PxTransform* rightPostTransform =
    new PxTransform(goalPos + PxVec3(3.66f * scaleFactor, 0, 0));
  RenderItem* rightPost =
    new RenderItem(CreateShape(postGeometry, gMaterial), Vector4(1, 1, 1, 1));
  rightPost->transform = rightPostTransform;
  m_renderItems.push_back(rightPost);
  m_transforms.push_back(rightPostTransform); 

  // LARGUERO
  PxTransform* crossbarTransform =
    new PxTransform(goalPos + PxVec3(0, 2.44f * scaleFactor, 0));
  PxBoxGeometry crossbarGeometry(
    3.8f * scaleFactor, 0.1f * scaleFactor, 0.1f * scaleFactor);
  RenderItem* crossbar = new RenderItem(
    CreateShape(crossbarGeometry, gMaterial), Vector4(1, 1, 1, 1));
  crossbar->transform = crossbarTransform;
  m_renderItems.push_back(crossbar);
  m_transforms.push_back(crossbarTransform); 
}