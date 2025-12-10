#ifndef SIMULATION_APP_H_
#define SIMULATION_APP_H_

#include "gloo/Application.hpp"

#include "TreeNode.hpp"

#include "IntegratorType.hpp"

namespace GLOO {
class SimulationApp : public Application {
 public:
  SimulationApp(const std::string& app_name,
                glm::ivec2 window_size,
                IntegratorType integrator_type,
                float integration_step);
  void SetupScene() override;
  void RenderTree();
  std::string SetRules();
protected:
  void DrawGUI() override;

 private:
  IntegratorType integrator_type_;
  float integration_step_;
  std::vector<float> probs_;
  RulesMap rules_;
  std::unique_ptr<TreeNode> tree_;
};
}  // namespace GLOO

#endif
