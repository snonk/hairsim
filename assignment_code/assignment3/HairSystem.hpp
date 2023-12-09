#ifndef HAIR_SYSTEM_H
#define HAIR_SYSTEM_H

#include "ParticleSystemBase.hpp"
#include "glm/ext.hpp"
#include <set>

// CONSTANTS
#define g 9.8
#define E 3.65 * 10e9


namespace GLOO {
class HairSystem : public ParticleSystemBase {
 public:

  struct Spring {
    int i;
    int j;
    float rest_len;
    float k;
  };

  HairSystem() {

  };
  HairSystem(float dragk) {
    k = dragk;
    wind_ = glm::vec3(0,0,0);
    s_damping = 1.0;
    l0 = 0.3;
  };

  void SetWind(float wind) {
    wind_ = glm::vec3(0,0,wind);
  }

  ParticleState ComputeTimeDerivative(const ParticleState& state,
                                              float time) const override {
    return state;
  }

  ParticleState ComputeNextState(const ParticleState& state,
                                              float time) const {
    ParticleState ret;

    glm::vec3 gravity;
    glm::vec3 drag;

    std::vector<glm::vec3> forces;
    
    if (masses.size() == 0) {
        return state;
    }

    // Add forces for each node of hair.
    for (size_t i = 0; i < state.positions.size(); i++) {
        gravity = masses[i] * glm::vec3(0,-10.,0);
        // drag = -k * state.velocities[i];
        
        // forces.push_back(gravity + drag + wind_);
        forces.push_back(gravity + wind_);
    }


    ParticleState tmp;
    std::vector<glm::vec3> d;
    for (size_t i = 0; i < state.positions.size(); i++) {
        ret.positions.push_back(state.positions[i] + state.velocities[i] * time + forces[i] * time * time);
        // solve constraints if not the first node
        if (i > 0) {
            glm::vec3 pos_vec = l0 * glm::normalize(ret.positions[i] - ret.positions[i - 1]);
            d.push_back(ret.positions[i - 1] + pos_vec - ret.positions[i]);
            ret.positions[i] = ret.positions[i - 1]
                               + pos_vec;
        }
        else {
            d.push_back(glm::vec3(0));
        }
    }
    for (size_t i = 0; i < state.velocities.size(); i++) {
        if (i < state.velocities.size() - 1) {
            ret.velocities.push_back((ret.positions[i] - state.positions[i]) / time
                                + s_damping * (-d[i+1])/time);
        }
        else {
            ret.velocities.push_back((ret.positions[i] - state.positions[i]) / time);
        }
    }
    

    // if fixed, set position to original position and zero out velocity
    for (size_t i = 0; i < state.positions.size(); i++) {
        if (fixed_.find(i) != fixed_.end()) {
            ret.positions[i] = state.positions[i];
            ret.velocities[i] = glm::vec3(0);
        }
    }

    return ret;
    
    };

    void SetFixed(int i) {
        fixed_.insert(i);
    };

    void AddParticle(float mass) {
        // particles_.positions.push_back(position);
        masses.push_back(mass);
    };

    // void AddSpring(int i, int j, float stiffness) {
    //     springs_.push_back(Spring{
    //         i,
    //         j,
    //         glm::length(particles_.positions[i] - particles_.positions[j]),
    //         stiffness,
    //     });
    // };

    private:
    std::set<int> fixed_;
    // std::vector<Spring> springs_;
    // ParticleState particles_;

    // mass values for each node
    std::vector<float> masses;

    // drag constant
    float k;

    // wind
    glm::vec3 wind_;

    // distance between nodes of hair cantilever beam
    float d_;

    // number of nodes in a hair strand (k + 1)
    // int num_nodes;

    // damping coefficient
    float s_damping;

    // enforced distance between every node and its predecessor
    float l0;

};
}  // namespace GLOO

#endif
