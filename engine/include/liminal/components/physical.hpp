#ifndef LIMINAL_COMPONENTS_PHYSICAL_HPP
#define LIMINAL_COMPONENTS_PHYSICAL_HPP

#include <memory>

namespace liminal
{
    class rigidbody;

    struct physical
    {
        float mass = 1.0f;
        std::shared_ptr<liminal::rigidbody> rigidbody;
    };
}

#endif
