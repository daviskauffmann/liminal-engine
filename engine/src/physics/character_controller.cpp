#include <liminal/physics/character_controller.hpp>

#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>
#include <bullet/BulletDynamics/Character/btKinematicCharacterController.h>
#include <bullet/btBulletDynamicsCommon.h>

liminal::character_controller::character_controller()
{
    bt_ghost_object = new btPairCachingGhostObject();

    bt_convex_shape = new btBoxShape(btVector3(0.5f, 1.0f, 0.5f));

    bt_character_controller = new btKinematicCharacterController(bt_ghost_object, bt_convex_shape, 0.5f);
}

liminal::character_controller::~character_controller()
{
    delete bt_ghost_object;
    delete bt_convex_shape;
    delete bt_character_controller;
}

btKinematicCharacterController *liminal::character_controller::get_bt_character_controller() const
{
    return bt_character_controller;
}
