#include "Drawer.hpp"
#include <iostream>

#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "gloo/debug/PrimitiveFactory.hpp"
#include "gloo/shaders/PhongShader.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/components/MaterialComponent.hpp"

#include "gloo/MeshLoader.hpp"

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
    Drawer::Drawer(SceneNode &root) : position_(glm::vec3(0)), orientation_(glm::quat(1, 0, 0, 0)), scene_(root)
    {
        cylinder_mesh_ = PrimitiveFactory::CreateCylinder(0.01f, 1.0f, 20);
        shader_ = std::make_shared<PhongShader>();
        material_ = std::make_shared<Material>();
        material_->SetAmbientColor(glm::vec3(0.4f, 0.25f, 0.1f));
        material_->SetDiffuseColor(glm::vec3(0.6f, 0.4f, 0.2f));

        MeshData leaf_data = MeshLoader::Import("low_poly_big_leaf.obj");
        leaf_mesh_ = std::move(leaf_data.vertex_obj);
        leaf_mat_ = std::make_shared<Material>();
        leaf_mat_->SetDiffuseColor(glm::vec3(0.007987,0.964009,0.001954));
        leaf_mat_->SetAmbientColor(glm::vec3(0.0f,1.0f,0.0f));
        leaf_mat_->SetSpecularColor(glm::vec3(0.5f));
    }

    void Drawer::Move(const std::string &instruction)
    {
        // step length for a single forward instruction
        const float kStep = 0.25f;
        const float kTurnAngle = glm::radians(25.0f);

        // get the forward direction from current orientation
        glm::vec3 y_axis(0.0f, 1.0f, 0.0f);
        glm::vec3 direction = glm::normalize(orientation_ * y_axis);

        if (instruction == "F" || instruction == "f")
        {
            glm::vec3 start = position_;
            position_ += direction * kStep;

            // add cylinder (copied from my skeleton thing)
            auto bone_node = make_unique<SceneNode>();
            bone_node->CreateComponent<ShadingComponent>(shader_);
            bone_node->CreateComponent<RenderingComponent>(cylinder_mesh_);
            bone_node->CreateComponent<MaterialComponent>(material_);

            bone_node->GetTransform().SetPosition(start);
            bone_node->GetTransform().SetScale(glm::vec3(1.0f, kStep, 1.0f));

            float dot = glm::dot(y_axis, direction);
            float angle = acosf(dot);
            glm::vec3 rot_axis = glm::cross(y_axis, direction);

            // avoid parallel to y
            if (glm::length(rot_axis) > 1e-6f)
            {
                bone_node->GetTransform().SetRotation(glm::normalize(rot_axis), angle);
            }

            scene_.AddChild(std::move(bone_node));

            // std::cout << "Drawing " + glm::to_string(start) + " to " + glm::to_string(position_) << std::endl;
        }
        else if (instruction == "g")
        {
            // move forward without drawing
            position_ += direction * kStep;
        }
        else if (instruction == "+")
        {
            // turn right (z)
            glm::quat q = glm::angleAxis(-kTurnAngle, glm::vec3(0, 0, 1));
            orientation_ = glm::normalize(orientation_ * q);
        }
        else if (instruction == "-")
        {
            // turn left (z)
            glm::quat q = glm::angleAxis(kTurnAngle, glm::vec3(0, 0, 1));
            orientation_ = glm::normalize(orientation_ * q);
            
        }
        else if (instruction == "&")
        {
            // pitch down (x)
            glm::quat pitch = glm::angleAxis(kTurnAngle, glm::vec3(1, 0, 0));
            orientation_ = glm::normalize(orientation_ * pitch);
        }
        else if (instruction == "^")
        {
            // pitch up (x)
            glm::quat pitch = glm::angleAxis(-kTurnAngle, glm::vec3(1, 0, 0));
            orientation_ = glm::normalize(orientation_ * pitch);
        }
        else if (instruction == "\\")
        {
            // roll left (y)
            glm::quat roll = glm::angleAxis(kTurnAngle, glm::vec3(0, 1, 0));
            orientation_ = glm::normalize(orientation_ * roll);
        }
        else if (instruction == "/")
        {
            // roll right (y)
            glm::quat roll = glm::angleAxis(-kTurnAngle, glm::vec3(0, 1, 0));
            orientation_ = glm::normalize(orientation_ * roll);
        }
        else if (instruction == "|")
        {
            // turn 180 degrees (z)
            glm::quat turn = glm::angleAxis(glm::pi<float>(), glm::vec3(0, 0, 1));
            orientation_ = glm::normalize(orientation_ * turn);
        }
        else if (instruction == "[")
        {
            State s;
            s.loc = position_;
            s.orientation = orientation_;
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
                orientation_ = s.orientation;
            }
        }
        else if (instruction == "*")
        {
            glm::vec3 start = position_;
            position_ += direction * kStep;

            // add cylinder (copied from my skeleton thing)
            auto bone_node = make_unique<SceneNode>();
            bone_node->CreateComponent<ShadingComponent>(shader_);
            bone_node->CreateComponent<RenderingComponent>(leaf_mesh_);
            bone_node->CreateComponent<MaterialComponent>(leaf_mat_);

            bone_node->GetTransform().SetPosition(start);
            bone_node->GetTransform().SetScale(glm::vec3(0.1f, 0.1f*kStep, 0.1f));

            float dot = glm::dot(y_axis, direction);
            float angle = acosf(dot);
            glm::vec3 rot_axis = glm::cross(y_axis, direction);

            // avoid parallel to y
            if (glm::length(rot_axis) > 1e-6f)
            {
                bone_node->GetTransform().SetRotation(glm::normalize(rot_axis), angle);
            }

            scene_.AddChild(std::move(bone_node));
        }
    }
} // namespace GLOO