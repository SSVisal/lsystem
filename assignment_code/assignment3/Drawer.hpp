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

struct State
{
    glm::vec3 loc;
    glm::vec3 dir;
};

namespace GLOO
{
    class Drawer
    {
    public:
        Drawer(SceneNode &root);
        void Move(const std::string &instruction);

    private:
        glm::vec3 position_;
        glm::vec3 direction_; // normalized

        std::stack<State> stack_; // for branches

        SceneNode &scene_;
        std::shared_ptr<ShaderProgram> shader_;
        std::shared_ptr<VertexObject> cylinder_mesh_;
    };
} // namespace GLOO

#endif