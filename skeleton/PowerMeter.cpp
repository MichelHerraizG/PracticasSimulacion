#include "PowerMeter.h"
#include <iostream>

extern physx::PxPhysics* gPhysics;
extern physx::PxMaterial* gMaterial;

PowerMeter::PowerMeter(float minH, float maxH, float spd)
  : minHeight(minH)
  , maxHeight(maxH)
  , currentHeight(minH)
  , speed(spd)
  , movingUp(true)
  , isActive(true)
  , isStopped(false)
  , barTransform(nullptr)
  , indicatorTransform(nullptr)
  , barItem(nullptr)
  , indicatorItem(nullptr)
{

  PxBoxGeometry barGeom(0.05f, (maxHeight - minHeight) * 0.5f, 0.05f);
  float barCenterY = minHeight + (maxHeight - minHeight) * 0.5f;

  barTransform = new PxTransform(PxVec3(-5.0f, barCenterY, 0.0f));
  barItem = new RenderItem(CreateShape(barGeom, gMaterial),
                           Vector4(0.3f, 0.3f, 0.3f, 1.0f));
  barItem->transform = barTransform;


  PxBoxGeometry indicatorGeom(0.15f, 0.05f, 0.15f);
  indicatorTransform = new PxTransform(PxVec3(-5.0f, currentHeight, 0.0f));
  indicatorItem = new RenderItem(CreateShape(indicatorGeom, gMaterial),
                                 Vector4(1.0f, 0.0f, 0.0f, 1.0f));
  indicatorItem->transform = indicatorTransform;
}

PowerMeter::~PowerMeter()
{
  if (barItem) {
    DeregisterRenderItem(barItem);
    delete barItem;
  }
  if (indicatorItem) {
    DeregisterRenderItem(indicatorItem);
    delete indicatorItem;
  }
  delete barTransform;
  delete indicatorTransform;
}

void PowerMeter::update(float dt)
{
  if (!isActive || isStopped)
    return;

  if (movingUp) {
    currentHeight += speed * dt;
    if (currentHeight >= maxHeight) {
      currentHeight = maxHeight;
      movingUp = false;
    }
  }
  else {
    currentHeight -= speed * dt;
    if (currentHeight <= minHeight) {
      currentHeight = minHeight;
      movingUp = true;
    }
  }

  if (indicatorTransform) {
    indicatorTransform->p.y = currentHeight;
  }
}

void PowerMeter::stop()
{
  isStopped = true;
  if (indicatorItem) {
    indicatorItem->color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
  }
}

void PowerMeter::reset()
{
  isStopped = false;
  currentHeight = minHeight;
  movingUp = true;

  if (indicatorItem) {
    indicatorItem->color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
  }
}

float PowerMeter::getCurrentHeight() const
{
  return currentHeight;
}

float PowerMeter::getNormalizedValue() const
{
  return (currentHeight - minHeight) / (maxHeight - minHeight);
}

void PowerMeter::setActive(bool active)
{
  isActive = active;

  if (active) {
    if (barItem)
      RegisterRenderItem(barItem);
    if (indicatorItem)
      RegisterRenderItem(indicatorItem);
  }
  else {
    if (barItem)
      DeregisterRenderItem(barItem);
    if (indicatorItem)
      DeregisterRenderItem(indicatorItem);
  }
}


bool PowerMeter::getIsStopped() const
{
  return isStopped;
}

void PowerMeter::setPosition(const Vector3& pos)
{
  if (barTransform) {
    barTransform->p =
      PxVec3(pos.x, pos.y + (maxHeight - minHeight) * 0.5f, pos.z);
  }
  if (indicatorTransform) {
    indicatorTransform->p = PxVec3(pos.x, currentHeight, pos.z);
  }
}

void PowerMeter::setBarColor(const Vector4& color)
{
  if (barItem)
    barItem->color = color;
}

void PowerMeter::setIndicatorColor(const Vector4& color)
{
  if (indicatorItem)
    indicatorItem->color = color;
}