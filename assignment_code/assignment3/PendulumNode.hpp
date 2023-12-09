#ifndef PENDULUM_NODE_H
#define PENDULUM_NODE_H

#include <string>
#include <vector>

#include "gloo/SceneNode.hpp"
#include "gloo/debug/PrimitiveFactory.hpp"
#include "gloo/VertexObject.hpp"
#include "gloo/shaders/ShaderProgram.hpp"
#include "gloo/shaders/PhongShader.hpp"
#include "ForwardEulerIntegrator.hpp"
#include "TrapezoidalIntegrator.hpp"
#include "IntegratorFactory.hpp"
#include "PendulumSystem.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/components/MaterialComponent.hpp"

namespace GLOO {

class PendulumNode : public SceneNode {
 public:
  PendulumNode(ParticleState state, IntegratorType type, PendulumSystem system, float step_size) {
    state_ = state;
    initial_state_ = state;
    integrator_ = IntegratorFactory::CreateIntegrator<PendulumSystem, ParticleState>(type);
    system_ = system;
    integration_step_ = step_size;
    shader_ = std::make_shared<PhongShader>();
    sphere_mesh_ = PrimitiveFactory::CreateSphere(0.1f, 25, 25);
    for (int i = 0; i < 4; i++) {
        auto point = make_unique<SceneNode>();
        point->CreateComponent<ShadingComponent>(shader_);
        point->CreateComponent<RenderingComponent>(sphere_mesh_);
        points_.push_back(point.get());
        AddChild(std::move(point));
        system_.AddParticle(3, state.positions[i]);
        // if (i < 3){ 
        //     system_.AddSpring(i, i + 1, 500);
        //     // system_.AddSpring(i + 1, i, 100);
        // }
    }
    system_.AddSpring(0, 1, 500);
    
    system_.AddSpring(1, 2, 500);
    system_.AddSpring(2, 3, 500);
    system_.SetFixed(0);
    printf("node inited\n");

  };

  void Update(double delta_time) override {
    for (double i = 0; i < delta_time; i += integration_step_) {
        state_ = integrator_->Integrate(system_, state_, 0, integration_step_);
        if ((std::abs(state_.positions[2][0]) > 100) || 
            (std::abs(state_.positions[2][1]) > 100) ||
            (std::abs(state_.positions[2][2]) > 100) ||
            glm::any(glm::isnan(state_.positions[2]))) {
              // std::cout << glm::to_string(state_.positions[2]) << std::endl;
              // std::cout << "!!!" << glm::to_string(initial_state_.positions[2]) << std::endl;
              // std::cout << "Failed, please try again." << std::endl;
              // std::cout << glm::to_string(glm::isnan(state_.positions[2]));
              // state_ = initial_state_;
            }
    }
    for (size_t i = 0; i < points_.size(); i++) {
        points_[i]->GetTransform().SetPosition(state_.positions[i]);
    }
  };

 private:
  ParticleState state_;
  ParticleState initial_state_;
  std::unique_ptr<IntegratorBase<PendulumSystem, ParticleState>> integrator_;
  PendulumSystem system_;
  float integration_step_;

  std::shared_ptr<VertexObject> sphere_mesh_;
  std::shared_ptr<ShaderProgram> shader_;
  std::vector<SceneNode*> points_;

};
}  // namespace GLOO

#endif
