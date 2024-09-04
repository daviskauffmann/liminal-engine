#ifndef LIMINAL_PHYSICS_CHARACTER_CONTROLLER_HPP
#define LIMINAL_PHYSICS_CHARACTER_CONTROLLER_HPP

#include <glm/vec3.hpp>
#include <tuple>

class btPairCachingGhostObject;
class btConvexShape;
class btKinematicCharacterController;

namespace liminal
{
    class character_controller
    {
    public:
        character_controller();
        ~character_controller();
        character_controller(const character_controller &other) = delete;
        character_controller &operator=(const character_controller &other) = delete;
        character_controller(character_controller &&other) = delete;
        character_controller &operator=(character_controller &&other) = delete;

        btKinematicCharacterController *get_bt_character_controller() const;

    private:
        btPairCachingGhostObject *bt_ghost_object;
        btConvexShape *bt_convex_shape;
        btKinematicCharacterController *bt_character_controller;
    };
}

#endif
