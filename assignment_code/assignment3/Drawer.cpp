#include "Drawer.hpp"
#include <iostream>

#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "gloo/debug/PrimitiveFactory.hpp"
#include "gloo/shaders/PhongShader.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"

/*
    Based on: https://github.com/abiusx/L3D:
    + = turn right
    - = turn left
    & = pitch down
    ^ = pitch up
    \ = roll left
    / = roll right
    | = turn 180 degree
    F = draw branch (and go forward)
    g = go forward (no drawing)
    [ = save state (i.e. start branch)
    ] = restore state (i.e. end branch and go back to where it started)
    * : draw leaf
    // it has more for thickness and stuff but i choose to ignore

    where turn, pitch, roll are in terms of different axis

*/

namespace GLOO
{
    Drawer::Drawer(SceneNode &root) : position_(glm::vec3(0)), direction_(glm::vec3(0, 1, 0)), scene_(root)
    {
        cylinder_mesh_ = PrimitiveFactory::CreateCylinder(0.1f, 1.0f, 20);
        shader_ = std::make_shared<PhongShader>();
    }

    void Drawer::Move(const std::string &instruction)
    {
        // step length for a single forward instruction
        const float kStep = 1.0f;
        const float kTurnAngle = glm::radians(25.0f);

        if (instruction == "F" || instruction == "f")
        {

            glm::vec3 start = position_;
            position_ += direction_ * kStep;

            // add cylinder (copied from my skeleton thing)
            auto bone_node = make_unique<SceneNode>();
            bone_node->CreateComponent<ShadingComponent>(shader_);
            bone_node->CreateComponent<RenderingComponent>(cylinder_mesh_);

            bone_node->GetTransform().SetPosition(start);
            bone_node->GetTransform().SetScale(glm::vec3(1.0f, kStep, 1.0f));

            glm::vec3 y_axis(0.0f, 1.0f, 0.0f);
            float dot = glm::dot(y_axis, direction_);
            float angle = acosf(dot);
            glm::vec3 rot_axis = glm::cross(y_axis, direction_);

            // avoid parallel to y
            if (glm::length(rot_axis) > 1e-6f)
            {
                bone_node->GetTransform().SetRotation(glm::normalize(rot_axis), angle);
            }

            scene_.AddChild(std::move(bone_node));

            // std::cout << "Drawing " + glm::to_string(start) + " to " + glm::to_string(position_) << std::endl;
        }
        else if (instruction == "+")
        {
            // turn right (z)
            auto dir = direction_;

            // TODO: use turtle orientation instead of fixed (0, 0, 1)
            glm::quat q = glm::angleAxis(-kTurnAngle, glm::vec3(0, 0, 1));
            direction_ = glm::normalize(q * direction_);

            // std::cout << "Turn right " + glm::to_string(dir) + " to " + glm::to_string(direction_) << std::endl;
        }
        else if (instruction == "-")
        {
            // turn left (z)
            auto dir = direction_;

            // TODO: use turtle orientation instead of fixed (0, 0, 1)
            glm::quat q = glm::angleAxis(kTurnAngle, glm::vec3(0, 0, 1));
            direction_ = glm::normalize(q * direction_);

            // std::cout << "Turn left " + glm::to_string(dir) + " to " + glm::to_string(direction_) << std::endl;
        }
        else if (instruction == "[")
        {
            State s;
            s.loc = position_;
            s.dir = direction_;
            stack_.push(s);
        }
        else if (instruction == "]")
        {
            // go back
            if (!stack_.empty())
            {
                State s = stack_.top();
                stack_.pop();
                position_ = s.loc;
                direction_ = s.dir;
            }
        }
    }
} // namespace GLOO