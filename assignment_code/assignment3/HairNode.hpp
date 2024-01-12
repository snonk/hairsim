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
#include "CurveNode.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/components/MaterialComponent.hpp"

namespace GLOO {

class HairNode : public SceneNode {
 public:
  HairNode(std::vector<ParticleState> state, IntegratorType type, HairSystem system, float step_size, int num_strands, int num_nodes) {
    state_ = state;
    initial_state_ = state;
    integrator_ = IntegratorFactory::CreateIntegrator<HairSystem, ParticleState>(type);
    system_ = system;
    integration_step_ = step_size;
    shader_ = std::make_shared<PhongShader>();
    sphere_mesh_ = PrimitiveFactory::CreateSphere(0.03f, 25, 25);
    num_nodes_ = num_nodes;
    num_strands_ = num_strands;
    wind_= true;

    InitNodes();

  };

  void InitNodes() {
    for (int strand = 0; strand < num_strands_; strand++ ){
      std::cout << "strand " << strand << std::endl;
      system_.AddStrand();
      for (int i = 0; i < num_nodes_; i++) {
          auto point = make_unique<SceneNode>();
          point->CreateComponent<ShadingComponent>(shader_);
          point->CreateComponent<RenderingComponent>(sphere_mesh_);
          points_.push_back(point.get());
          system_.AddParticle(strand, 1, point->GetTransform().GetWorldPosition());
          AddChild(std::move(point));
      }
      std::cout << "nodes added to system" << std::endl;
      auto curve_node = make_unique<CurveNode>(GLOO::SplineBasis::Bezier, state_[strand].positions);
      curves_.push_back(curve_node.get());
      AddChild(std::move(curve_node));
      std::cout << "curves added to app" << std::endl;

    }
    system_.SetFixed(0);
    printf("node inited\n");

  }

  void Update(double delta_time) override {
    static bool prev_released = true;
    if (InputManager::GetInstance().IsKeyPressed('R')) {
        if (prev_released) {
            state_ = initial_state_;
        }
        prev_released = false;
    } else if (InputManager::GetInstance().IsKeyPressed('W')) {
        if (prev_released) {
            if (wind_) {
                system_.SetWind(-10.);
            }
            else {
                system_.SetWind(0);
            }
            wind_ = !wind_;
        }
        prev_released = false;
    }
    else {
        prev_released = true;
    }

    state_ = system_.ComputeNextState(state_, delta_time);
    // for (size_t i = 0; i < points_.size(); i++) {
    //     // std::cout << "position " << i << ": " << glm::to_string(state_.positions[i]) << std::endl;
    //     // std::cout << "velocity " << i << ": " << glm::to_string(state_.velocities[i]) << std::endl;
    //     // std::cout << std::endl;
    // }
    for (size_t strand = 0; strand < num_strands_; strand++) {
      curves_[strand]->UpdateControlPoints(state_[strand].positions);
      for (size_t node = 0; node < num_nodes_; node++)
        points_[strand * num_nodes_ + node]->GetTransform().SetPosition(state_[strand].positions[node]);
    }
  };

 private:
  std::vector<ParticleState> state_;
  std::vector<ParticleState> initial_state_;
  std::unique_ptr<IntegratorBase<HairSystem, ParticleState>> integrator_;
  HairSystem system_;
  float integration_step_;
  int num_strands_;
  int num_nodes_;

  std::shared_ptr<VertexObject> sphere_mesh_;
  std::shared_ptr<ShaderProgram> shader_;
  std::vector<SceneNode*> points_;

  std::vector<CurveNode*> curves_;

  bool wind_;

};
}  // namespace GLOO

#endif
