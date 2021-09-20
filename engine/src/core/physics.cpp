#include <liminal/core/physics.hpp>

#include <bullet/btBulletDynamicsCommon.h>
#include <liminal/components/physical.hpp>
#include <liminal/components/transform.hpp>

liminal::physics::physics()
{
    btDefaultCollisionConfiguration *collision_configuration = new btDefaultCollisionConfiguration();
    btDispatcher *dispatcher = new btCollisionDispatcher(collision_configuration);
    btBroadphaseInterface *pair_cache = new btDbvtBroadphase();
    btConstraintSolver *constraint_solver = new btSequentialImpulseConstraintSolver();
    world = new btDiscreteDynamicsWorld(dispatcher, pair_cache, constraint_solver, collision_configuration);
    world->setGravity(btVector3(0.0f, -9.8f, 0.0f));
}

liminal::physics::~physics()
{
    delete world;
}

void liminal::physics::update(liminal::scene &scene, float delta_time)
{
    // update world transforms in case the transform component changed
    // TODO: remove this, should just have a function that sets up the initial state of rigidbodies and then lets bullet be in charge
    // this would make it so that you can't move rigidbodies around in the editor (unless physics is turned off), but maybe this is okay
    for (auto [entity, physical, transform] : scene.registry.view<liminal::physical, liminal::transform>().each())
    {
        btTransform world_transform;
        world_transform.setIdentity();
        world_transform.setOrigin(btVector3(transform.position.x, transform.position.y, transform.position.z));
        world_transform.setRotation(btQuaternion(transform.rotation.y, transform.rotation.x, transform.rotation.z));

        physical.rigidbody->setWorldTransform(world_transform);
    }

    // update physics world
    world->stepSimulation(delta_time);

    // update transforms
    for (auto [entity, physical, transform] : scene.registry.view<liminal::physical, liminal::transform>().each())
    {
        btTransform world_transform = physical.rigidbody->getWorldTransform();

        transform.position = {
            world_transform.getOrigin().x(),
            world_transform.getOrigin().y(),
            world_transform.getOrigin().z()};

        world_transform.getRotation().getEulerZYX(transform.rotation.z, transform.rotation.y, transform.rotation.x);
    }
}
