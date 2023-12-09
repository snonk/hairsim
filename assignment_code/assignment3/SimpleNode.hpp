#ifndef SIMPLE_NODE_H_
#define SIMPLE_NODE_H_

#include <string>
#include <vector>

#include "gloo/SceneNode.hpp"
#include "gloo/debug/PrimitiveFactory.hpp"
#include "gloo/VertexObject.hpp"
#include "gloo/shaders/ShaderProgram.hpp"
#include "ForwardEulerIntegrator.hpp"
#include "TrapezoidalIntegrator.hpp"
#include "IntegratorFactory.hpp"
#include "SimpleSystem.hpp"

namespace GLOO {

class SimpleNode : public SceneNode {
 public:
  SimpleNode(ParticleState state, IntegratorType type, SimpleSystem system, float step_size) {
    state_ = state;
    integrator_ = IntegratorFactory::CreateIntegrator<SimpleSystem, ParticleState>(type);
    system_ = system;
    integration_step_ = step_size;
    auto point = make_unique<SceneNode>();
    shader_ = std::make_shared<PhongShader>();
    sphere_mesh_ = PrimitiveFactory::CreateSphere(0.1f, 25, 25);
    point->CreateComponent<ShadingComponent>(shader_);
    point->CreateComponent<RenderingComponent>(sphere_mesh_);
    point_ = point.get();
    AddChild(std::move(point));
    time_elapsed_ = 0;
  };
  void Update(double delta_time) override {
    for (double i = 0; i < delta_time; i += integration_step_) {
        state_ = integrator_->Integrate(system_, state_, 0, integration_step_);
    }
    point_->GetTransform().SetPosition(state_.positions[0]);


  };

 private:
  ParticleState state_;
  std::unique_ptr<IntegratorBase<SimpleSystem, ParticleState>> integrator_;
  SimpleSystem system_;
  float integration_step_;
  double time_elapsed_;

  std::shared_ptr<VertexObject> sphere_mesh_;
  std::shared_ptr<ShaderProgram> shader_;
  SceneNode* point_;



};
}  // namespace GLOO

#endif
