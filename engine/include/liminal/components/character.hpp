#ifndef LIMINAL_COMPONENTS_CHARACTER_HPP
#define LIMINAL_COMPONENTS_CHARACTER_HPP

#include <memory>

namespace liminal
{
    class character_controller;

    struct character
    {
        std::shared_ptr<liminal::character_controller> character_controller;
    };
}

#endif
