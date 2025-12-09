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
#include "gloo/lights/DirectionalLight.hpp"
#include "gloo/lights/AmbientLight.hpp"
#include "gloo/cameras/ArcBallCameraNode.hpp"
#include "gloo/debug/AxisNode.hpp"

#include "Generator_zhi.hpp"

#include "Drawer.hpp"

namespace GLOO {
SimulationApp::SimulationApp(const std::string& app_name,
                             glm::ivec2 window_size,
                             IntegratorType integrator_type,
                             float integration_step)
    : Application(app_name, window_size),
      integrator_type_(integrator_type),
      integration_step_(integration_step) {
}

void SimulationApp::SetupScene() {
  SceneNode& root = scene_->GetRootNode();

  auto camera_node = make_unique<ArcBallCameraNode>(45.f, 0.75f, 5.0f);
  scene_->ActivateCamera(camera_node->GetComponentPtr<CameraComponent>());
  root.AddChild(std::move(camera_node));

  root.AddChild(make_unique<AxisNode>('A'));

  auto ambient_light = std::make_shared<AmbientLight>();
  ambient_light->SetAmbientColor(glm::vec3(0.85f));
  root.CreateComponent<LightComponent>(ambient_light);

  // auto point_light = std::make_shared<PointLight>();
  // point_light->SetDiffuseColor(glm::vec3(0.8f, 0.8f, 0.8f));
  // point_light->SetSpecularColor(glm::vec3(1.0f, 1.0f, 1.0f));
  // point_light->SetAttenuation(glm::vec3(1.0f, 0.09f, 0.032f));
  // auto point_light_node = make_unique<SceneNode>();
  // point_light_node->CreateComponent<LightComponent>(point_light);
  // point_light_node->GetTransform().SetPosition(glm::vec3(0.0f, 10.0f, 4.f));
  // root.AddChild(std::move(point_light_node));

  RenderTree();
}

void SimulationApp::RenderTree(){
  // Generator gen = Generator("10");
  // std::vector<std::string> tokens = gen.Generate();
  // for(std::string token: tokens){
  //   std::cout<<token<<std::endl;
  // }

  // 2D plant
  // std::map<std::string, std::vector<Replacement>> prod_rules;
  // Generator gen = Generator("-X", prod_rules);
  // gen.AddRule("X", {"F+[[X]-X]-F[-FX]+X", 1});
  // gen.AddRule("F", {"FF", 1});
  

  // 3D maybe
  std::map<std::string, std::vector<Replacement>> prod_rules;
  Generator gen = Generator("X", prod_rules);

  gen.AddRule("X", {"F[+X][-X][&X]",     0.25});
  gen.AddRule("X", {"F[/X][\\X][+X]",        0.25});
  gen.AddRule("X", {"F[&X][^X][/X]",     0.25});
  gen.AddRule("X", {"FX",                0.25});

  gen.AddRule("F", {"FF", 0.25});
  gen.AddRule("F", {"F", 0.75});

  auto final_string = gen.Generate(10);

  // std::cout << final_string << std::endl; 

  SceneNode& root = scene_->GetRootNode();
  auto turtle = Drawer(root);

  for (char c : final_string) {
    std::string key(1, c);
    turtle.Move(key);
  }
}


}  // namespace GLOO
