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
    for(int i = 0; i < 4; i++){
      probs_.push_back(0.25);
    }
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
  SceneNode& root = scene_->GetRootNode();

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
  

  /*
    added TreeNode so that 
    - pressing R resets the generator + tree
    - pressing N advances one generator step 
  */
  std::string current = SetRules();
  auto tree_node = GLOO::make_unique<TreeNode>(current, rules_, 1);
  tree_ptr_ = tree_node.get();
  // tree_->LinkRules(rules_);
  root.AddChild(std::move(tree_node));
}

void SimulationApp::DrawGUI(){
  bool modified = false;
  ImGui::Begin("Probability Control Panel");
  modified |= ImGui::SliderFloat("Rule 1: * -> F[+*][-*][&*]", &probs_[0], 0, 1);
  modified |= ImGui::SliderFloat("Rule 2: * -> F[/*][\\*][+*]", &probs_[1], 0, 1);
  modified |= ImGui::SliderFloat("Rule 3: * -> F[&*][^*][/*]", &probs_[2], 0, 1);
  modified |= ImGui::SliderFloat("Rule 4: * -> F*", &probs_[3], 0, 1);
  ImGui::PopID();
  ImGui::End();

  if(modified){
    SetRules();
    tree_ptr_->UpdateRules(rules_);
  }
}

std::string SimulationApp::SetRules(){
  std::map<std::string, std::vector<Replacement>> prod_rules;
  Generator gen = Generator("*", prod_rules);

  // gen.AddRule("X", {"F[+X][-X*][&X]", 0.25});
  // gen.AddRule("X", {"F[/X][\\X*][+X*]", 0.25});
  // gen.AddRule("X", {"F[&X][^X*][/X]", 0.25});
  // gen.AddRule("X", {"FX*", 0.25});

  gen.AddRule("*", {"F[+*][-*][&*]", probs_[0]});
  gen.AddRule("*", {"F[/*][\\*][+*]", probs_[1]});
  gen.AddRule("*", {"F[&*][^*][/*]", probs_[2]});
  gen.AddRule("*", {"F*", probs_[3]});

  // gen.AddRule("X", {"*", 0.5});
  // gen.AddRule("X", {"X", 0.5});

  std::cout<<probs_[0]<<probs_[1]<<probs_[2]<<probs_[3]<<std::endl;
  gen.AddRule("F", {"FF", 0.25});
  gen.AddRule("F", {"F", 0.75});
  rules_ = gen.GetRules();

  return gen.GetCurrent();
}

}  // namespace GLOO
