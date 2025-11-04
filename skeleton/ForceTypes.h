#pragma once
#include "ForceGenerator.h"
#include "Particle.h"
#include <vector>

class ForceTypes {
private:
  struct ForceEntry {
    Particle* particle;
    ForceGenerator* fg;
  };

  std::vector<ForceEntry> registrations;

public:
  void add(Particle* particle, ForceGenerator* fg)
  {
    registrations.push_back({ particle, fg });
  }

  void remove(Particle* particle, ForceGenerator* fg)
  {
    registrations.erase(std::remove_if(registrations.begin(),
                                       registrations.end(),
                                       [=](const ForceEntry& r) {
                                         return r.particle == particle &&
                                                r.fg == fg;
                                       }),
                        registrations.end());
  }

  void clear() { registrations.clear(); }

  void updateForces(double dt)
  {
    for (auto& r : registrations) {
      r.fg->updateForce(r.particle, dt);
    }
  }
};