#include <liminal/core/scene.hpp>

#include <bullet/btBulletDynamicsCommon.h>
#include <fstream>
#include <liminal/audio/sound.hpp>
#include <liminal/components/audio_listener.hpp>
#include <liminal/components/audio_source.hpp>
#include <liminal/components/camera.hpp>
#include <liminal/components/directional_light.hpp>
#include <liminal/components/mesh_renderer.hpp>
#include <liminal/components/physical.hpp>
#include <liminal/components/point_light.hpp>
#include <liminal/components/script.hpp>
#include <liminal/components/spot_light.hpp>
#include <liminal/components/terrain.hpp>
#include <liminal/components/transform.hpp>
#include <liminal/components/water.hpp>
#include <liminal/core/assets.hpp>
#include <liminal/core/entity.hpp>
#include <liminal/core/platform.hpp>
#include <liminal/graphics/model.hpp>
#include <liminal/graphics/skybox.hpp>
#include <nlohmann/json.hpp>
#include <sol/sol.hpp>
#include <string>

liminal::scene::scene()
{
    skybox = nullptr;

    const auto collision_configuration = new btDefaultCollisionConfiguration();
    const auto dispatcher = new btCollisionDispatcher(collision_configuration);
    const auto pair_cache = new btDbvtBroadphase();
    const auto constraint_solver = new btSequentialImpulseConstraintSolver();
    world = new btDiscreteDynamicsWorld(dispatcher, pair_cache, constraint_solver, collision_configuration);
    world->setGravity(btVector3(0, -9.8f, 0));
}

liminal::scene::scene(const liminal::scene &other)
{
}

liminal::scene::~scene()
{
    for (auto [id, physical] : get_entities_with<liminal::physical>().each())
    {
        world->removeRigidBody(physical.rigidbody);
    }

    for (auto [id, terrain] : get_entities_with<liminal::terrain>().each())
    {
        world->removeRigidBody(terrain.rigidbody);
    }

    registry.clear();

    delete world;
}

void liminal::scene::load(const std::string &filename)
{
    std::ifstream stream(filename);
    nlohmann::json json;
    stream >> json;

    for (auto &[key, value] : json.items())
    {
        if (key == "skybox")
        {
            skybox = liminal::assets::instance->load_skybox(json["skybox"]);
        }

        if (key == "entities")
        {
            for (auto &element : value)
            {
                auto entity = create_entity();

                for (auto &[key, value] : element.items())
                {
                    if (key == "directional_light")
                    {
                        glm::vec3 color(
                            value["color"]["r"],
                            value["color"]["g"],
                            value["color"]["b"]);
                        entity.add_component<liminal::directional_light>(color);
                    }

                    if (key == "spot_light")
                    {
                        glm::vec3 color(
                            value["color"]["r"],
                            value["color"]["g"],
                            value["color"]["b"]);
                        entity.add_component<liminal::spot_light>(color, value["inner_cutoff"], value["outer_cutoff"]);
                    }

                    if (key == "transform")
                    {
                        std::string name(value["name"]);
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
                        entity.add_component<liminal::transform>(name, nullptr, position, rotation, scale);
                    }

                    if (key == "physical")
                    {
                        auto &physical = entity.add_component<liminal::physical>(value["mass"]);
                        world->addRigidBody(physical.rigidbody);
                    }

                    if (key == "mesh_renderer")
                    {
                        std::string filename(value["filename"]);
                        bool flip_uvs = value["flip_uvs"];
                        entity.add_component<liminal::mesh_renderer>(liminal::assets::instance->load_model(filename, flip_uvs));
                    }

                    if (key == "script")
                    {
                        entity.add_component<liminal::script>(value["filename"], this, (entt::entity)entity);
                    }

                    if (key == "water")
                    {
                        entity.add_component<liminal::water>(value["tiling"]);
                    }

                    if (key == "terrain")
                    {
                        auto &terrain = entity.add_component<liminal::terrain>("assets/images/heightmap.png", glm::vec3(0, 0, 0), 100.f, 5.f);
                        world->addRigidBody(terrain.rigidbody);
                    }
                }
            }
        }
    }
}

void liminal::scene::save(const std::string &filename)
{
}

liminal::entity liminal::scene::create_entity()
{
    const auto id = registry.create();
    return liminal::entity(id, this);
}

liminal::entity liminal::scene::get_entity(entt::entity id)
{
    return liminal::entity(id, this);
}

void liminal::scene::delete_entity(liminal::entity entity)
{
    registry.destroy(entity);
}

void liminal::scene::start()
{
    // init scripts
    for (auto [id, script] : get_entities_with<liminal::script>().each())
    {
        script.init();
    }
}

void liminal::scene::update(unsigned int current_time, float delta_time)
{
    // update scripts
    for (auto [id, script] : get_entities_with<liminal::script>().each())
    {
        script.update(delta_time);
    }

    // update animations
    for (auto [id, mesh_renderer] : get_entities_with<liminal::mesh_renderer>().each())
    {
        if (mesh_renderer.model && mesh_renderer.model->has_animations())
        {
            // TODO: put current animation_index on the mesh_renderer component
            mesh_renderer.model->update_bone_transformations(0, current_time);
        }
    }

    // update audio listener positions
    for (auto [id, audio_listener, camera, transform] : get_entities_with<liminal::audio_listener, liminal::camera, liminal::transform>().each())
    {
        audio_listener.set_position(transform.position, camera.calc_front(transform));
    }

    // update audio source positions
    for (auto [id, audio_source, transform] : get_entities_with<liminal::audio_source, liminal::transform>().each())
    {
        audio_source.set_position(transform.position);
    }

    // update world transforms in case the transform component changed
    // TODO: remove this, should just have a function that sets up the initial state of rigidbodies and then lets bullet be in charge
    // this would make it so that you can't move rigidbodies around in the editor (unless physics is turned off), but maybe this is okay
    for (auto [id, physical, transform] : get_entities_with<liminal::physical, liminal::transform>().each())
    {
        btTransform world_transform;
        world_transform.setIdentity();
        world_transform.setOrigin(btVector3(transform.position.x, transform.position.y, transform.position.z));
        world_transform.setRotation(btQuaternion(transform.rotation.y, transform.rotation.x, transform.rotation.z));

        physical.rigidbody->setWorldTransform(world_transform);
    }

    // update physics world
    world->stepSimulation(delta_time);

    // update transforms to respect physics simulation
    for (auto [id, physical, transform] : get_entities_with<liminal::physical, liminal::transform>().each())
    {
        btTransform world_transform = physical.rigidbody->getWorldTransform();

        transform.position = {
            world_transform.getOrigin().x(),
            world_transform.getOrigin().y(),
            world_transform.getOrigin().z()};

        world_transform.getRotation().getEulerZYX(transform.rotation.z, transform.rotation.y, transform.rotation.x);
    }
}

void liminal::scene::stop()
{
}

void liminal::scene::reload_scripts()
{
    for (auto [id, script] : get_entities_with<liminal::script>().each())
    {
        // TODO:
    }
}
