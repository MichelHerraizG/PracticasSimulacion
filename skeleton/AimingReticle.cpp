#include "AimingReticle.h"

extern physx::PxPhysics* gPhysics;
extern physx::PxMaterial* gMaterial;

AimingReticle::AimingReticle(float radius)
  : baseRadius(radius)
  , currentRadius(radius)
  , visible(true)
{
  
  PxSphereGeometry sphereGeom(0.05f);
  circleItem = new RenderItem(CreateShape(sphereGeom, gMaterial),
                              Vector4(1.0f, 1.0f, 0.0f, 1.0f)); 

  PxBoxGeometry lineGeom(0.02f, 0.02f, 1.0f); 
  directionLine = new RenderItem(CreateShape(lineGeom, gMaterial),
                                 Vector4(1.0f, 0.0f, 0.0f, 1.0f)); 

  circleTransform = PxTransform(PxVec3(0, 0, 0));
  lineTransform = PxTransform(PxVec3(0, 0, 0));

  circleItem->transform = &circleTransform;
  directionLine->transform = &lineTransform;
}

AimingReticle::~AimingReticle()
{
  if (circleItem) {
    DeregisterRenderItem(circleItem);
    delete circleItem;
  }
  if (directionLine) {
    DeregisterRenderItem(directionLine);
    delete directionLine;
  }
}

void AimingReticle::setVisible(bool isVisible)
{
  visible = isVisible;
}

void AimingReticle::update(const PxVec3& position,
                           const PxVec3& direction,
                           float power)
{
  if (!visible)
    return;


  currentRadius = baseRadius * (0.5f + power * 0.5f);


  PxVec3 reticlePos =
    position + PxVec3(0, 0.1f, 0);  
  circleTransform = PxTransform(reticlePos);


  PxVec3 lineEnd = reticlePos + direction * (currentRadius + 0.5f);
  PxVec3 lineCenter = (reticlePos + lineEnd) * 0.5f;


  PxVec3 lineDir = direction.getNormalized();
  float lineLength = (lineEnd - reticlePos).magnitude();


  PxVec3 up(0, 1, 0);
  PxVec3 axis = up.cross(lineDir);
  float angle = acos(up.dot(lineDir));

  lineTransform = PxTransform(lineCenter, PxQuat(angle, axis));

  
}

void AimingReticle::setColor(const Vector4& color)
{
  if (circleItem) {
    circleItem->color = color;
  }

  Vector4 lineColor(color.x * 0.7f, color.y * 0.7f, color.z * 0.7f, color.w);
  if (directionLine) {
    directionLine->color = lineColor;
  }
}