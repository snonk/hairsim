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

  std::vector<ParticleState> ComputeNextState(const std::vector<ParticleState>& state,
                                              float time) const {
    std::vector<ParticleState> ret = std::vector<ParticleState>();

    
    // if (masses.size() == 0) {
    //     return state;
    // }

    // Add forces for each node of hair.
    for (size_t strand = 0; strand < state.size(); strand++) {
        glm::vec3 gravity;
        glm::vec3 drag;

        std::vector<glm::vec3> forces;

        ParticleState cur_strand;

        for (size_t i = 0; i < state[strand].positions.size(); i++) {
            gravity = masses[strand][i] * glm::vec3(0,-10.,0);
            // drag = -k * state[strand].velocities[i];
            
            // forces.push_back(gravity + drag + wind_);
            forces.push_back(gravity + wind_);
            // forces.push_back(state[strand].positions[i]);
        }

        ParticleState tmp;
        std::vector<glm::vec3> d;
        for (size_t i = 0; i < state[strand].positions.size(); i++) {
            cur_strand.positions.push_back(state[strand].positions[i] + state[strand].velocities[i] * time + forces[i] * time * time);
            // solve constraints if not the first node
            if (i > 0) {
                glm::vec3 pos_vec = l0 * glm::normalize(cur_strand.positions[i] - cur_strand.positions[i - 1]);
                d.push_back(cur_strand.positions[i - 1] + pos_vec - cur_strand.positions[i]);
                cur_strand.positions[i] = cur_strand.positions[i - 1]
                                + pos_vec;
            }
            else {
                d.push_back(glm::vec3(0));
            }
        }
        for (size_t i = 0; i < state[strand].velocities.size(); i++) {
            if (i < state[strand].velocities.size() - 1) {
                cur_strand.velocities.push_back((cur_strand.positions[i] - state[strand].positions[i]) / time
                                    + s_damping * (-d[i+1])/time);
            }
            else {
                cur_strand.velocities.push_back((cur_strand.positions[i] - state[strand].positions[i]) / time);
            }
        }
        

        // if fixed, set position to original position and zero out velocity
        for (size_t i = 0; i < state[strand].positions.size(); i++) {
            if (fixed_.find(i) != fixed_.end()) {
                cur_strand.positions[i] = state[strand].positions[i];
                cur_strand.velocities[i] = glm::vec3(0);
            }
        }

        ret.push_back(cur_strand);
    }
    return ret;


    
    };

    void SetFixed(int i) {
        fixed_.insert(i);
    };

    void AddStrand() {
        ParticleState tmp = ParticleState();
        std::vector<float> mass_tmp = std::vector<float>();

        particles_.push_back(tmp);
        masses.push_back(mass_tmp);
    }

    void AddParticle(int strand, float mass, glm::vec3 position) {
        particles_[strand].positions.push_back(position);
        masses[strand].push_back(mass);
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

    // running list of current particle states for each hair strand
    std::vector<ParticleState> particles_ = std::vector<ParticleState>();

    // mass values for each node
    std::vector<std::vector<float>> masses;

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
