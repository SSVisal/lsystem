#ifndef CURVE_NODE_H_
#define CURVE_NODE_H_

#include <string>
#include <vector>

#include "gloo/SceneNode.hpp"
#include "Generator_zhi.hpp"
#include "Drawer.hpp"

namespace GLOO
{
    class TreeNode : public SceneNode
    {
    public:
        TreeNode(std::string axiom, RulesMap &rules, int step);
        void Update(double delta_time) override;

    private:
        void ResetTree();
        void NextStep();

        std::string axiom_; 
        RulesMap rules_;
        int step_size_;
        int current_step_;

        Generator generator_;
    };
} // namespace GLOO

#endif
