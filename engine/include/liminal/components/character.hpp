#ifndef LIMINAL_COMPONENTS_CHARACTER_HPP
#define LIMINAL_COMPONENTS_CHARACTER_HPP

#include <memory>

namespace liminal
{
    namespace physics
    {
        class character_controller;
    }

    namespace components
    {
        struct character
        {
            std::shared_ptr<liminal::physics::character_controller> character_controller;
        };
    }
}

#endif
