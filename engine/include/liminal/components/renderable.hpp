#ifndef LIMINAL_COMPONENTS_RENDERABLE_HPP
#define LIMINAL_COMPONENTS_RENDERABLE_HPP

#include <glm/matrix.hpp>
#include <glm/vec3.hpp>
#include <memory>
#include <vector>

namespace liminal
{
    namespace graphics
    {
        class model;
    }

    namespace components
    {
        struct renderable
        {
            std::shared_ptr<liminal::graphics::model> model;
            glm::vec3 color = glm::vec3(1, 1, 1);
            float metallic = 0;
            float roughness = 1;
            std::vector<glm::mat4> bone_transformations;

            renderable(std::shared_ptr<liminal::graphics::model> model = {});
        };
    }
}

#endif
