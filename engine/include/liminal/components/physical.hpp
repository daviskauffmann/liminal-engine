#ifndef LIMINAL_COMPONENTS_PHYSICAL_HPP
#define LIMINAL_COMPONENTS_PHYSICAL_HPP

#include <glm/vec3.hpp>
#include <memory>

namespace liminal
{
    class rigidbody;

    struct physical
    {
        glm::vec3 scale = glm::vec3(1, 1, 1);
        float mass = 1.0f;
        std::shared_ptr<liminal::rigidbody> rigidbody;
    };
}

#endif
