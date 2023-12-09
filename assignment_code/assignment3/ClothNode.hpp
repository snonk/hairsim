#ifndef CLOTH_NODE_H
#define CLOTH_NODE_H

#include <string>
#include <vector>

#include "gloo/SceneNode.hpp"
#include "gloo/debug/PrimitiveFactory.hpp"
#include "gloo/VertexObject.hpp"
#include "gloo/shaders/ShaderProgram.hpp"
#include "gloo/shaders/PhongShader.hpp"
#include "gloo/InputManager.hpp"
#include "gloo/shaders/SimpleShader.hpp"
#include "ForwardEulerIntegrator.hpp"
#include "TrapezoidalIntegrator.hpp"
#include "IntegratorFactory.hpp"
#include "PendulumSystem.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/components/MaterialComponent.hpp"

namespace GLOO {

class ClothNode : public SceneNode {
 public:
  ClothNode(ParticleState state, IntegratorType type, PendulumSystem system, float step_size, int dim) {
    state_ = state;
    initial_state_ = state;
    dim_ = dim;
    integrator_ = IntegratorFactory::CreateIntegrator<PendulumSystem, ParticleState>(type);
    system_ = system;
    integration_step_ = step_size;
    shader_ = std::make_shared<PhongShader>();
    polyline_shader_ = std::make_shared<SimpleShader>();

    sphere_mesh_ = PrimitiveFactory::CreateSphere(0.1f, 25, 25);
    auto t = std::make_shared<VertexObject>();
    auto positions = make_unique<PositionArray>();
    for (size_t i = 0; i < state_.positions.size(); i++) {
        positions->push_back(state_.positions[i]);
        system_.AddParticle(10, state.positions[i]);
        auto point = make_unique<SceneNode>();
        point->CreateComponent<ShadingComponent>(shader_);
        point->CreateComponent<RenderingComponent>(sphere_mesh_);
        points_.push_back(point.get());
        AddChild(std::move(point));
    }
    auto indices = make_unique<IndexArray>();
    int k = 2000;
    for (int i = 0; i < dim_; i++) {
        for (int j = 0; j < dim_; j++) {
            
            if (j < dim_ - 1){ 
                system_.AddSpring(i * dim_ + j, i * dim_ + j + 1, k);
                indices->push_back(i * dim_ + j);
                indices->push_back(i * dim_ + j + 1);
            }
            if (i < dim_ - 1) {
                system_.AddSpring(i * dim_ + j, (i + 1) * dim_ + j, k);
                indices->push_back(i * dim_ + j);
                indices->push_back((i + 1) * dim_ + j);
            }
            if (i < dim_ - 1 && j < dim_ - 1) {
                system_.AddSpring(i*dim_ + j, (i+1)*dim_ + j + 1, k);
                indices->push_back(i*dim_ + j);
                indices->push_back((i+1)*dim_ + j + 1);
            }
            if (i < dim_ - 1 && j > 0) {
                system_.AddSpring(i*dim_ + j, (i+1)*dim_ + j - 1, k);
                indices->push_back(i*dim_ + j);
                indices->push_back((i+1)*dim_ + j - 1);
            }
            if (j < dim_ - 2){ 
                system_.AddSpring(i * dim_ + j, i * dim_ + j + 2, k);
                indices->push_back(i * dim_ + j);
                indices->push_back(i * dim_ + j + 2);
            }
            if (i < dim_ - 2) {
                system_.AddSpring(i * dim_ + j, (i + 2) * dim_ + j, k);
                indices->push_back(i * dim_ + j);
                indices->push_back((i + 2) * dim_ + j);
            }
        }

    }
    t->UpdatePositions(std::move(positions));
    t->UpdateIndices(std::move(indices));
    springs_ = t.get();
    auto spring = make_unique<SceneNode>();
    spring->CreateComponent<ShadingComponent>(polyline_shader_);
    auto component = spring->CreateComponent<RenderingComponent>(t);
    component.SetDrawMode(DrawMode::Lines);
    spring_nodes_.push_back(spring.get());
    AddChild(std::move(spring));
    system_.SetFixed(0);
    system_.SetFixed(dim_ - 1);
    system_.SetFixed(dim_ /2);

  };


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

    for (double i = 0; i < delta_time; i += integration_step_) {
        state_ = integrator_->Integrate(system_, state_, 0, integration_step_);
    }
    for (size_t i = 0; i < points_.size(); i++) {
        points_[i]->GetTransform().SetPosition(state_.positions[i]);
    }
    auto positions = make_unique<PositionArray>();
    for (size_t i = 0; i < state_.positions.size(); i++) {
        positions->push_back(state_.positions[i]);
    }
    springs_->UpdatePositions(std::move(positions));
  };

 private:
  ParticleState state_;
  ParticleState initial_state_;
  std::unique_ptr<IntegratorBase<PendulumSystem, ParticleState>> integrator_;
  PendulumSystem system_;
  float integration_step_;
  int dim_;

  std::shared_ptr<VertexObject> sphere_mesh_;
  std::shared_ptr<ShaderProgram> shader_;
  std::shared_ptr<ShaderProgram> polyline_shader_;
  std::vector<SceneNode*> points_;
  std::vector<SceneNode*> spring_nodes_;
  VertexObject* springs_;
  bool wind_ = false;

};
}  // namespace GLOO

#endif
