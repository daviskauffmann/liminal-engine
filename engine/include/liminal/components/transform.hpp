#ifndef LIMINAL_COMPONENTS_TRANSFORM_HPP
#define LIMINAL_COMPONENTS_TRANSFORM_HPP

#include <glm/matrix.hpp>
#include <glm/vec3.hpp>
#include <string>

namespace liminal
{
    class transform
    {
    public:
        std::string name = "New Entity";
        liminal::transform *parent = nullptr;
        glm::vec3 position = {};
        glm::vec3 rotation = {};
        glm::vec3 scale = {1, 1, 1};

        glm::mat4 get_model_matrix() const;
    };
}

#endif
