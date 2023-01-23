#include <liminal/physics/world.hpp>

#include <bullet/btBulletDynamicsCommon.h>
#include <liminal/physics/rigidbody.hpp>

liminal::world::world()
{
    const auto collision_configuration = new btDefaultCollisionConfiguration();
    const auto dispatcher = new btCollisionDispatcher(collision_configuration);
    const auto pair_cache = new btDbvtBroadphase();
    const auto constraint_solver = new btSequentialImpulseConstraintSolver();
    bt_world = new btDiscreteDynamicsWorld(dispatcher, pair_cache, constraint_solver, collision_configuration);
    bt_world->setGravity(btVector3(0, -9.8f, 0));
}

liminal::world::~world()
{
    delete bt_world->getDispatcher();
    delete bt_world->getPairCache();
    delete bt_world->getConstraintSolver();
    delete bt_world;
}

void liminal::world::add_rigidbody(const std::shared_ptr<liminal::rigidbody> rigidbody)
{
    bt_world->addRigidBody(rigidbody->get_bt_rigidbody());
}

void liminal::world::remove_rigidbody(const std::shared_ptr<liminal::rigidbody> rigidbody)
{
    bt_world->removeRigidBody(rigidbody->get_bt_rigidbody());
}

void liminal::world::update(const float delta_time)
{
    static float physics_time_accumulator = 0.0f;
    physics_time_accumulator += delta_time;
    constexpr float physics_delta_time = 1.0f / 60.0f;
    if (physics_time_accumulator >= physics_delta_time)
    {
        physics_time_accumulator -= physics_delta_time;

        bt_world->stepSimulation(physics_delta_time);
    }
}
