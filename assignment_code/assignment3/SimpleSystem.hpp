#ifndef SIMPLE_SYSTEM_H
#define SIMPLE_SYSTEM_H

#include "ParticleSystemBase.hpp"
#include "glm/ext.hpp"

namespace GLOO {
class SimpleSystem : public ParticleSystemBase {
 public:

  ParticleState ComputeTimeDerivative(const ParticleState& state,
                                              float time) const override {
    ParticleState ret = ParticleState();
    ret.positions.push_back(glm::vec3(-state.positions[0][1], state.positions[0][0], 0.f));
    ret.velocities.push_back(glm::vec3(0,0,0));
    return ret;
    
    };
};
}  // namespace GLOO

#endif
