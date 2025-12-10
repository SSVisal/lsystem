#include "TreeNode.hpp"

#include "gloo/InputManager.hpp"


namespace GLOO
{
    TreeNode::TreeNode(std::string axiom, RulesMap &rules, int step = 1) : axiom_(axiom), rules_(rules), step_size_(step), generator_(axiom, rules), current_step_(0)
    {
        ResetTree();
    }

    void TreeNode::Update(double delta_time)
    {

        // Prevent multiple toggle.
        static bool prev_released = true;

        if (InputManager::GetInstance().IsKeyPressed('R'))
        {
            if (prev_released)
            {
                ResetTree();
            }
            prev_released = false;
        }
        else if (InputManager::GetInstance().IsKeyPressed('N'))
        {
            if (prev_released)
            {
                NextStep();
            }
            prev_released = false;
        }
        else
        {
            prev_released = true;
        }
    }

    void TreeNode::ResetTree(){
        this->Clear();
        generator_ = Generator(axiom_, rules_);
        current_step_ = 0;

        auto origin_string = generator_.GetCurrent();
        auto turtle = Drawer(*this);

        for (char c : origin_string) {
            std::string key(1, c);
            turtle.Move(key);
        }

        // std::cout << "Restart Tree: " << origin_string << std::endl; 
        std::cout << "Restart Tree: " << std::endl; 
    }

    void TreeNode::NextStep(){
        this->Clear();
        auto step_string = generator_.Generate(step_size_);
        auto turtle = Drawer(*this);

        for (char c : step_string) {
            std::string key(1, c);
            turtle.Move(key);
        }

        // std::cout << "Tree at step " << step_size_*current_step_ << ": " << step_string << std::endl;
        std::cout << "Tree at step " << step_size_*current_step_ << std::endl;

        current_step_ += 1;
    }

} // namespace GLOO
