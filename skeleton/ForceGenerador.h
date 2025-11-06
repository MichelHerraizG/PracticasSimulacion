#pragma once
#include "ForceType.h"
#include <vector>
class Particle;
class ForceType;
class ForceGenerador {
private:
  struct ForceEntry {
    Particle* particle;
    ForceType* fg;
    bool active;
  };

  std::vector<ForceEntry> registrations;

public:
  void add(Particle* particle, ForceType* fg, bool act)
  {
    registrations.push_back({ particle, fg, act });
  }

  void remove(Particle* particle, ForceType* fg)
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
  void setActive(ForceType* fg, bool act)
  {
    for (auto& r : registrations) {
      if (r.fg == fg) {
        r.active = act;
      }
    }
  }
  void updateForces(double dt)
  {
    for (auto& r : registrations) {
      if (r.active)
        r.fg->updateForce(r.particle, dt);
    }
  }
};