#pragma once

#include "core.hpp"


class Particle;
class ForceType {
public:
  virtual ~ForceType() = default;
  virtual void updateForce(Particle* particle, double dt) = 0;
};



