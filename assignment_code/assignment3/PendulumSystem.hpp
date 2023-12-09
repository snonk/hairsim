#ifndef PENDULUM_SYSTEM_H
#define PENDULUM_SYSTEM_H

#include "ParticleSystemBase.hpp"
#include "glm/ext.hpp"
#include <set>

namespace GLOO {
class PendulumSystem : public ParticleSystemBase {
 public:

  struct Spring {
    int i;
    int j;
    float rest_len;
    float k;
  };

  PendulumSystem() {

  };
  PendulumSystem(float dragk) {
    k = dragk;
    wind_ = glm::vec3(0,0,0);

  };

  void SetWind(float wind) {
    wind_ = glm::vec3(0,0,wind);
  }

  ParticleState ComputeTimeDerivative(const ParticleState& state,
                                              float time) const override {
    ParticleState ret;

    glm::vec3 gravity;
    glm::vec3 drag;

    std::vector<glm::vec3> forces;
    
    // printf("RAHHH\n");
    if (masses.size() == 0) {
        return state;
    }
    // std::cout << std::endl;
    // printf("state.positions.size(): %d\n", state.positions.size());

    for (size_t i = 0; i < state.positions.size(); i++) {
        // std::cout << glm::to_string(state.positions[i]) << std::endl;
        gravity = masses[i] * glm::vec3(0,-10.,0);
        // std::cout << "grav: " << glm::to_string(gravity) << std::endl;
        drag = -k * state.velocities[i];
        // std::cout << "drag: " << glm::to_string(drag) << std::endl;
        // std::cout << "wind: " << glm::to_string(wind_) << std::endl;
        

        forces.push_back(gravity + drag + wind_);
        // std::cout << glm::to_string(forces[i]) << std::endl;
        
    }

    for (Spring s : springs_) {
        glm::vec3 d = state.positions[s.i] - state.positions[s.j];
        if (glm::length(d) == 0) {
            printf("UHOH\n");
            continue;
        }
        // std::cout << "springs: " << s.i << ", " << s.j << std::endl;
        forces[s.i] += -s.k * (glm::length(d) - s.rest_len) * glm::normalize(d);
        forces[s.j] += s.k * (glm::length(d) - s.rest_len) * glm::normalize(d);
        // std::cout << "force i: " << glm::to_string(forces[s.i]) << std::endl;
        // std::cout << "force j: " << glm::to_string(forces[s.j]) << std::endl;
    }
    for (size_t i = 0; i < state.positions.size(); i++) {
        if (fixed_.find(i) != fixed_.end()) {
            ret.positions.push_back(glm::vec3(0,0,0));
            ret.velocities.push_back(glm::vec3(0,0,0));
        }
        else {

            // if (masses[i] == 0) {
            //     std::cout << "broken" << std::endl;
            // }

            glm::vec3 a = forces[i] / masses[i];

            // if (glm::any(glm::isnan(a))) {
            // std::cout << "force: " << glm::to_string(forces[i]) << std::endl;
            // std::cout << masses[i] << std::endl;
            // std::cout << "acc: " << glm::to_string(a) << std::endl;

            // }
            ret.positions.push_back(state.velocities[i]);
            ret.velocities.push_back(a);
        }
    }

    return ret;
    
    };

    void SetFixed(int i) {
        fixed_.insert(i);
    };

    void AddParticle(float mass, glm::vec3 position) {
        particles_.positions.push_back(position);
        masses.push_back(mass);
    };

    void AddSpring(int i, int j, float stiffness) {
        springs_.push_back(Spring{
            i,
            j,
            glm::length(particles_.positions[i] - particles_.positions[j]),
            stiffness,
        });
    };

    private:
    std::set<int> fixed_;
    std::vector<Spring> springs_;
    ParticleState particles_;
    std::vector<float> masses;
    float k;
    glm::vec3 wind_;



};
}  // namespace GLOO

#endif
