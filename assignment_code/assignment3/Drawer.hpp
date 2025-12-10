#ifndef DRAWER_H_
#define DRAWER_H_

#include <string>
#include <stack>

#include <glm/glm.hpp>
#include "gloo/utils.hpp"
#include <glm/gtc/quaternion.hpp>

#include "gloo/SceneNode.hpp"
#include "gloo/VertexObject.hpp"
#include "gloo/shaders/ShaderProgram.hpp"
#include "gloo/Material.hpp"
#include "gloo/MeshData.hpp"

struct State
{
    glm::vec3 loc;
    glm::quat orientation;
};

namespace GLOO
{
    class Drawer
    {
    public:
        Drawer(SceneNode &root);
        void Move(const std::string &instruction);
        void ResetState(){
            position_ = glm::vec3(0);
            orientation_ = glm::quat(1, 0, 0, 0);
        };

    private:
        glm::vec3 position_;
        glm::quat orientation_; // full 3D orientation (quaternion)

        std::stack<State> stack_; // for branches

        SceneNode &scene_;
        std::shared_ptr<ShaderProgram> shader_;
        std::shared_ptr<VertexObject> cylinder_mesh_;
        std::shared_ptr<VertexObject> leaf_mesh_;
        std::shared_ptr<Material> leaf_mat_;
        std::shared_ptr<Material> material_;
        glm::vec3 min_val_ = glm::vec3(100000.0f);
        glm::vec3 max_val_ = glm::vec3(-100000.0f);
        float scale_ = 0.05f;
    };
} // namespace GLOO

#endif