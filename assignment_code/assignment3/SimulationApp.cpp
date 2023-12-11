#include "SimulationApp.hpp"

#include "glm/gtx/string_cast.hpp"

#include "gloo/shaders/PhongShader.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/components/CameraComponent.hpp"
#include "gloo/components/LightComponent.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/MeshLoader.hpp"
#include "gloo/lights/PointLight.hpp"
#include "gloo/lights/AmbientLight.hpp"
#include "gloo/cameras/ArcBallCameraNode.hpp"
#include "gloo/debug/AxisNode.hpp"

#include "SimpleNode.hpp"
#include "ClothNode.hpp"
#include "ForwardEulerIntegrator.hpp"
#include "TrapezoidalIntegrator.hpp"
#include "IntegratorType.hpp"
#include "PendulumSystem.hpp"
#include "PendulumNode.hpp"
#include "HairSystem.hpp"
#include "HairNode.hpp"

namespace GLOO {
SimulationApp::SimulationApp(const std::string& app_name,
                             glm::ivec2 window_size,
                             IntegratorType integrator_type,
                             float integration_step)
    : Application(app_name, window_size),
      integrator_type_(integrator_type),
      integration_step_(integration_step) {
  // TODO: remove the following two lines and use integrator type and step to
  // create integrators; the lines below exist only to suppress compiler
  // warnings.
  // UNUSED(integrator_type_);
  // UNUSED(integration_step_);
}

void SimulationApp::SetupScene() {
  SceneNode& root = scene_->GetRootNode();

  auto camera_node = make_unique<ArcBallCameraNode>(45.f, 0.75f, 5.0f);
  scene_->ActivateCamera(camera_node->GetComponentPtr<CameraComponent>());
  root.AddChild(std::move(camera_node));

  root.AddChild(make_unique<AxisNode>('A'));

  auto ambient_light = std::make_shared<AmbientLight>();
  ambient_light->SetAmbientColor(glm::vec3(0.2f));
  root.CreateComponent<LightComponent>(ambient_light);

  auto point_light = std::make_shared<PointLight>();
  point_light->SetDiffuseColor(glm::vec3(0.8f, 0.8f, 0.8f));
  point_light->SetSpecularColor(glm::vec3(1.0f, 1.0f, 1.0f));
  point_light->SetAttenuation(glm::vec3(1.0f, 0.09f, 0.032f));
  auto point_light_node = make_unique<SceneNode>();
  point_light_node->CreateComponent<LightComponent>(point_light);
  point_light_node->GetTransform().SetPosition(glm::vec3(0.0f, 2.0f, 4.f));
  root.AddChild(std::move(point_light_node));


  // ParticleState simple_state = ParticleState();
  // simple_state.positions.push_back(glm::vec3(-1,0,0));
  // simple_state.velocities.push_back(glm::vec3(0,0,0));
  // SimpleSystem system = SimpleSystem();
  // auto simple_node = make_unique<SimpleNode>(simple_state, integrator_type_, system, integration_step_);
  // root.AddChild(std::move(simple_node));


  int num_strands = 5;
  int nodes_per_strand = 4;

  std::cout << "1" << std::endl;

  std::vector<ParticleState> hair_state = std::vector<ParticleState>();
  for (int i = 0; i < num_strands; i++) {
    ParticleState strand_state = ParticleState();
    for (int j = 0; j < nodes_per_strand; j++) {
      strand_state.positions.push_back(glm::vec3((i+j+static_cast <float> (rand()) / static_cast <float> (RAND_MAX))*0.1,-j*0.2,0));
      strand_state.velocities.push_back(glm::vec3(0,0,0));
    }
    hair_state.push_back(strand_state);
  }
  std::cout << "2" << std::endl;

  HairSystem pendulum_system = HairSystem(1.);
  auto pendulum_node = make_unique<HairNode>(hair_state, 
                                                integrator_type_, pendulum_system,
                                                integration_step_, num_strands, nodes_per_strand);
  std::cout << "3" << std::endl;
  
  root.AddChild(std::move(pendulum_node));



  // ParticleState cloth_state = ParticleState();
  // int dims = 8;
  // for (int i = 0; i < dims; i++) {
  //   for (int j = 0; j < dims; j++) {
  //   cloth_state.positions.push_back(glm::vec3(-j/4.,-i/4.,0));
  //   cloth_state.velocities.push_back(glm::vec3(0,0,0));
  //   }
  // }
  // PendulumSystem cloth_system = PendulumSystem(5.);
  // auto cloth_node = make_unique<ClothNode>(cloth_state, integrator_type_, cloth_system, integration_step_, dims);
  // root.AddChild(std::move(cloth_node));
}
}  // namespace GLOO
