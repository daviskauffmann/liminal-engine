#ifndef LIMINAL_COMPONENTS_RENDERABLE_HPP
#define LIMINAL_COMPONENTS_RENDERABLE_HPP

#include <glm/matrix.hpp>
#include <glm/vec3.hpp>
#include <memory>
#include <vector>

namespace liminal
{
    class model;

    struct renderable
    {
        std::shared_ptr<liminal::model> model;
        glm::vec3 color;
        std::vector<glm::mat4> bone_transformations;

        renderable(std::shared_ptr<liminal::model> model = {}, const glm::vec3 &color = glm::vec3(1, 1, 1));
    };
}

#endif