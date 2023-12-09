#ifndef HAIR_NODE_H
#define HAIR_NODE_H

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
#include "HairSystem.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/components/MaterialComponent.hpp"

namespace GLOO {

class HairNode : public SceneNode {
 public:
  HairNode(ParticleState state, IntegratorType type, HairSystem system, float step_size, int num_nodes) {
    state_ = state;
    initial_state_ = state;
    integrator_ = IntegratorFactory::CreateIntegrator<HairSystem, ParticleState>(type);
    system_ = system;
    integration_step_ = step_size;
    shader_ = std::make_shared<PhongShader>();
    sphere_mesh_ = PrimitiveFactory::CreateSphere(0.03f, 25, 25);
    num_nodes_ = num_nodes;

    InitNode();

  };

  void InitNode() {
    for (int i = 0; i < num_nodes_; i++) {
        auto point = make_unique<SceneNode>();
        point->CreateComponent<ShadingComponent>(shader_);
        point->CreateComponent<RenderingComponent>(sphere_mesh_);
        points_.push_back(point.get());
        AddChild(std::move(point));
        system_.AddParticle(1);
    }
    system_.SetFixed(0);
    printf("node inited\n");
  }

  void Update(double delta_time) override {

    state_ = system_.ComputeNextState(state_, delta_time);
    for (size_t i = 0; i < points_.size(); i++) {
        // std::cout << "position " << i << ": " << glm::to_string(state_.positions[i]) << std::endl;
        // std::cout << "velocity " << i << ": " << glm::to_string(state_.velocities[i]) << std::endl;
        // std::cout << std::endl;
        points_[i]->GetTransform().SetPosition(state_.positions[i]);
    }
  };

 private:
  ParticleState state_;
  ParticleState initial_state_;
  std::unique_ptr<IntegratorBase<HairSystem, ParticleState>> integrator_;
  HairSystem system_;
  float integration_step_;
  int num_nodes_;

  std::shared_ptr<VertexObject> sphere_mesh_;
  std::shared_ptr<ShaderProgram> shader_;
  std::vector<SceneNode*> points_;

};
}  // namespace GLOO

#endif
