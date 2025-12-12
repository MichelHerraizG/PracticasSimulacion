#pragma once

#include "core.hpp"
#include <PxPhysicsAPI.h>

class Particle;

class ForceType {
public:
	virtual ~ForceType() = default;
	virtual void updateForce(Particle* particle, double dt) = 0;
	virtual void updateForceRigid(physx::PxRigidDynamic* rigid, double dt) = 0;
};