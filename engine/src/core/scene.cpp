#include <liminal/core/scene.hpp>

#include <bullet/btBulletDynamicsCommon.h>
#include <fstream>
#include <liminal/audio/sound.hpp>
#include <liminal/components/audio_listener.hpp>
#include <liminal/components/audio_source.hpp>
#include <liminal/components/directional_light.hpp>
#include <liminal/components/mesh_renderer.hpp>
#include <liminal/components/physical.hpp>
#include <liminal/components/point_light.hpp>
#include <liminal/components/script.hpp>
#include <liminal/components/spot_light.hpp>
#include <liminal/components/terrain.hpp>
#include <liminal/components/transform.hpp>
#include <liminal/components/water.hpp>
#include <liminal/core/engine.hpp>
#include <liminal/core/entity.hpp>
#include <liminal/core/platform.hpp>
#include <liminal/graphics/camera.hpp>
#include <liminal/graphics/model.hpp>
#include <liminal/graphics/skybox.hpp>
#include <nlohmann/json.hpp>
#include <sol/sol.hpp>
#include <string>

liminal::scene::scene(const std::string &filename)
{
    camera = nullptr;
    skybox = nullptr;

    // create physics world
    btDefaultCollisionConfiguration *collision_configuration = new btDefaultCollisionConfiguration();
    btDispatcher *dispatcher = new btCollisionDispatcher(collision_configuration);
    btBroadphaseInterface *pair_cache = new btDbvtBroadphase();
    btConstraintSolver *constraint_solver = new btSequentialImpulseConstraintSolver();
    world = new btDiscreteDynamicsWorld(dispatcher, pair_cache, constraint_solver, collision_configuration);
    world->setGravity(btVector3(0.0f, -9.8f, 0.0f));

    // load scene from file
    std::ifstream stream(filename);
    nlohmann::json json;
    stream >> json;
    for (auto &[key, value] : json.items())
    {
        if (key == "skybox")
        {
            skybox = new liminal::skybox(json["skybox"]);
        }

        if (key == "entities")
        {
            for (auto &element : value)
            {
                auto entity = registry.create();

                for (auto &[key, value] : element.items())
                {
                    if (key == "directional_light")
                    {
                        glm::vec3 color(
                            value["color"]["r"],
                            value["color"]["g"],
                            value["color"]["b"]);
                        registry.emplace<liminal::directional_light>(entity, color);
                    }

                    if (key == "transform")
                    {
                        glm::vec3 position(
                            value["position"]["x"],
                            value["position"]["y"],
                            value["position"]["z"]);
                        glm::vec3 rotation(
                            value["rotation"]["x"],
                            value["rotation"]["y"],
                            value["rotation"]["z"]);
                        glm::vec3 scale(
                            value["scale"]["x"],
                            value["scale"]["y"],
                            value["scale"]["z"]);
                        auto transform = registry.emplace<liminal::transform>(entity, std::string(value["name"]).c_str(), nullptr, position, rotation, scale);
                    }

                    if (key == "physical")
                    {
                        auto physical = registry.emplace<liminal::physical>(entity, value["mass"]);
                        world->addRigidBody(physical.rigidbody);
                    }

                    if (key == "mesh_renderer")
                    {
                        std::string filename(value["filename"]);
                        bool flip_uvs = value["flip_uvs"];
                        registry.emplace<liminal::mesh_renderer>(entity, new liminal::model(filename, flip_uvs));
                    }

                    if (key == "script")
                    {
                        registry.emplace<liminal::script>(entity, value["filename"], this, entity);
                    }

                    if (key == "water")
                    {
                        registry.emplace<liminal::water>(entity, value["tiling"]);
                    }

                    if (key == "terrain")
                    {
                        auto terrain = registry.emplace<liminal::terrain>(entity, "assets/images/heightmap.png", glm::vec3(0.0f, 0.0f, 0.0f), 100.0f, 5.0f);
                        world->addRigidBody(terrain.rigidbody);
                    }
                }
            }
        }
    }

    // run init scripts
    for (auto [entity, script] : registry.view<liminal::script>().each())
    {
        script.init();
    }
}

liminal::scene::~scene()
{
    for (auto [entity, script] : registry.view<liminal::script>().each())
    {
        delete script.lua;
    }

    for (auto [entity, physical] : registry.view<liminal::physical>().each())
    {
        world->removeRigidBody(physical.rigidbody);
        delete physical.rigidbody;
    }

    for (auto [entity, terrain] : registry.view<liminal::terrain>().each())
    {
        delete terrain.mesh;

        world->removeRigidBody(terrain.rigidbody);
        delete terrain.rigidbody;
    }

    for (auto [entity, audio_source] : registry.view<liminal::audio_source>().each())
    {
        delete audio_source.source;
    }

    registry.clear();

    delete world;
}

liminal::entity liminal::scene::create_entity()
{
    return liminal::entity(registry.create(), this);
}

void liminal::scene::update(unsigned int current_time, float delta_time)
{
    // update scripts
    for (auto [entity, script] : registry.view<liminal::script>().each())
    {
        script.update(delta_time);
    }

    // update animations
    for (auto [entity, mesh_renderer] : registry.view<liminal::mesh_renderer>().each())
    {
        if (mesh_renderer.model->has_animations())
        {
            mesh_renderer.model->update_bone_transformations(0, current_time);
        }
    }

    // update audio listener positions
    for (auto [entity, audio_listener, transform] : registry.view<liminal::audio_listener, liminal::transform>().each())
    {
        audio_listener.set_position(transform.position, transform.rotation);
    }

    // update audio source positions
    for (auto [entity, audio_source, transform] : registry.view<liminal::audio_source, liminal::transform>().each())
    {
        audio_source.source->set_position(transform.position);
    }

    // update world transforms in case the transform component changed
    // TODO: remove this, should just have a function that sets up the initial state of rigidbodies and then lets bullet be in charge
    // this would make it so that you can't move rigidbodies around in the editor (unless physics is turned off), but maybe this is okay
    for (auto [entity, physical, transform] : registry.view<liminal::physical, liminal::transform>().each())
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
    for (auto [entity, physical, transform] : registry.view<liminal::physical, liminal::transform>().each())
    {
        btTransform world_transform = physical.rigidbody->getWorldTransform();

        transform.position = {
            world_transform.getOrigin().x(),
            world_transform.getOrigin().y(),
            world_transform.getOrigin().z()};

        world_transform.getRotation().getEulerZYX(transform.rotation.z, transform.rotation.y, transform.rotation.x);
    }
}
