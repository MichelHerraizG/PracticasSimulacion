#pragma once
#include "ForceType.h"
#include "Particle.h"
#include "core.hpp"
#include <PxPhysicsAPI.h>

using namespace physx;

class AnchoredSpring : public ForceType {
private:
  Vector3 anchor;
  float k;
  float restingLength;

public:
  AnchoredSpring(const Vector3& anchorPos,
                 float springConstant,
                 float restLength)
    : anchor(anchorPos)
    , k(springConstant)
    , restingLength(restLength)
  {
  }

  virtual ~AnchoredSpring() = default;

  virtual void updateForce(Particle* particle, double dt) override
  {
    if (!particle)
      return;

    Vector3 particlePos = particle->getPos();
    Vector3 d = particlePos - anchor;
    float distance = d.magnitude();

    if (distance < 0.0001f)
      return;

    float deformation = distance - restingLength;
    Vector3 forceDir = d.getNormalized();
    Vector3 force = forceDir * (-k * deformation);

    particle->addForce(force);
  }

  virtual void updateForceRigid(PxRigidDynamic* rigid, double dt) override
  {
    if (!rigid)
      return;

    PxVec3 rigidPos = rigid->getGlobalPose().p;
    PxVec3 anchorPx(anchor.x, anchor.y, anchor.z);

    PxVec3 d = rigidPos - anchorPx;
    float distance = d.magnitude();

    if (distance < 0.0001f)
      return;

    float deformation = distance - restingLength;
    PxVec3 forceDir = d.getNormalized();
    PxVec3 force = forceDir * (-k * deformation);

    rigid->addForce(force, PxForceMode::eFORCE);
  }

  void setAnchor(const Vector3& newAnchor) { anchor = newAnchor; }
  const Vector3& getAnchor() const { return anchor; }

  void setSpringConstant(float newK) { k = newK; }
  float getSpringConstant() const { return k; }

  void setRestingLength(float newLength) { restingLength = newLength; }
  float getRestingLength() const { return restingLength; }
};