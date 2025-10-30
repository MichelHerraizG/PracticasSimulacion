#pragma once

#include "core.hpp"


class Particle;
class ForceGenerator {
public:
  virtual ~ForceGenerator() = default;
  virtual void updateForce(Particle* particle, double dt) = 0;
};