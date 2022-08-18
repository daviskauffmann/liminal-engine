#include <liminal/entities/scene.hpp>

#include <bullet/btBulletDynamicsCommon.h>
#include <fstream>
#include <liminal/audio/listener.hpp>
#include <liminal/audio/sound.hpp>
#include <liminal/audio/source.hpp>
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
#include <liminal/entities/entity.hpp>
#include <liminal/graphics/model.hpp>
#include <liminal/graphics/skybox.hpp>
#include <nlohmann/json.hpp>
#include <sol/sol.hpp>

liminal::scene::scene()
{
    skybox = nullptr;

    registry.on_construct<liminal::audio_source>().connect<&scene::on_audio_source_construct>(this);
    registry.on_destroy<liminal::audio_source>().connect<&scene::on_audio_source_destroy>(this);
    registry.on_construct<liminal::physical>().connect<&scene::on_physical_construct>(this);
    registry.on_destroy<liminal::physical>().connect<&scene::on_physical_destroy>(this);

    const auto collision_configuration = new btDefaultCollisionConfiguration();
    const auto dispatcher = new btCollisionDispatcher(collision_configuration);
    const auto pair_cache = new btDbvtBroadphase();
    const auto constraint_solver = new btSequentialImpulseConstraintSolver();
    world = std::make_unique<btDiscreteDynamicsWorld>(dispatcher, pair_cache, constraint_solver, collision_configuration);
    world->setGravity(btVector3(0, -9.8f, 0));
}

liminal::scene::scene(const liminal::scene &)
{
}

liminal::scene::~scene()
{
    registry.clear();

    delete world->getDispatcher();
    delete world->getPairCache();
    delete world->getConstraintSolver();
}

void liminal::scene::load(const std::string &filename, std::shared_ptr<liminal::assets> assets)
{
    const auto scene_json = nlohmann::json::parse(std::ifstream(filename));

    for (const auto &[key, value] : scene_json.items())
    {
        if (key == "skybox")
        {
            skybox = assets->load_skybox(value);
        }

        if (key == "entities")
        {
            for (const auto &entity_json : value)
            {
                auto entity = create_entity();

                for (const auto &[component_type, component_json] : entity_json.items())
                {
                    if (component_type == "directional_light")
                    {
                        entity.add_component<liminal::directional_light>(
                            glm::vec3(
                                component_json.at("color").at("r"),
                                component_json.at("color").at("g"),
                                component_json.at("color").at("b")));
                    }

                    if (component_type == "spot_light")
                    {
                        entity.add_component<liminal::spot_light>(
                            glm::vec3(
                                component_json.at("color").at("r"),
                                component_json.at("color").at("g"),
                                component_json.at("color").at("b")),
                            component_json.at("inner_cutoff"),
                            component_json.at("outer_cutoff"));
                    }

                    if (component_type == "transform")
                    {
                        entity.add_component<liminal::transform>(
                            component_json.at("name"),
                            nullptr, // TODO: support parenting from JSON file
                            glm::vec3(
                                component_json.at("position").at("x"),
                                component_json.at("position").at("y"),
                                component_json.at("position").at("z")),
                            glm::vec3(
                                component_json.at("rotation").at("x"),
                                component_json.at("rotation").at("y"),
                                component_json.at("rotation").at("z")),
                            glm::vec3(
                                component_json.at("scale").at("x"),
                                component_json.at("scale").at("y"),
                                component_json.at("scale").at("z")));
                    }

                    if (component_type == "physical")
                    {
                        entity.add_component<liminal::physical>(component_json.at("mass"));
                    }

                    if (component_type == "mesh_renderer")
                    {
                        entity.add_component<liminal::mesh_renderer>(
                            assets->load_model(
                                component_json.at("filename"),
                                assets,
                                component_json.at("flip_uvs")));
                    }

                    if (component_type == "script")
                    {
                        entity.add_component<liminal::script>(
                            component_json.at("filename"),
                            this,
                            (entt::entity)entity,
                            assets);
                    }

                    if (component_type == "water")
                    {
                        entity.add_component<liminal::water>(component_json.at("tiling"));
                    }

                    if (component_type == "terrain")
                    {
                        entity.add_component<liminal::terrain>(
                            "assets/images/heightmap.png",
                            glm::vec3(0, 0, 0),
                            100.0f,
                            5.0f,
                            assets);
                    }
                }
            }
        }
    }
}

void liminal::scene::save(const std::string &)
{
}

liminal::entity liminal::scene::create_entity()
{
    const auto id = registry.create();
    return liminal::entity(id, this);
}

liminal::entity liminal::scene::get_entity(const entt::entity id)
{
    return liminal::entity(id, this);
}

void liminal::scene::delete_entity(const liminal::entity entity)
{
    registry.destroy(entity);
}

void liminal::scene::start()
{
    // init scripts
    for (const auto [id, script] : get_entities_with<const liminal::script>().each())
    {
        script.init();
    }
}

void liminal::scene::update(const std::uint64_t current_time, const float delta_time)
{
    // update scripts
    for (const auto [id, script] : get_entities_with<const liminal::script>().each())
    {
        script.update(delta_time);
    }

    // update animations
    for (const auto [id, mesh_renderer] : get_entities_with<liminal::mesh_renderer>().each())
    {
        if (mesh_renderer.model && mesh_renderer.model->has_animations())
        {
            // TODO: put current animation_index on the mesh_renderer component
            mesh_renderer.bone_transformations = mesh_renderer.model->calc_bone_transformations(0, current_time);
        }
    }

    // update audio listener positions
    for (const auto [id, audio_listener, camera, transform] : get_entities_with<liminal::audio_listener, const liminal::camera, const liminal::transform>().each())
    {
        liminal::listener::set_position(transform.position);
        liminal::listener::set_velocity(audio_listener.last_position - transform.position);
        liminal::listener::set_orientation(camera.calc_front(transform), glm::vec3(0, 1, 0));

        audio_listener.last_position = transform.position;
    }

    // update audio source positions
    for (const auto [id, audio_source, transform] : get_entities_with<liminal::audio_source, const liminal::transform>().each())
    {
        audio_source.source->set_position(transform.position);
        audio_source.source->set_velocity(audio_source.last_position - transform.position);

        audio_source.last_position = transform.position;
    }

    // update world transforms in case the transform component changed
    for (const auto [id, physical, transform] : get_entities_with<const liminal::physical, const liminal::transform>().each())
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
    for (const auto [id, physical, transform] : get_entities_with<const liminal::physical, liminal::transform>().each())
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
    // TODO:
    // for (const auto [id, script] : get_entities_with<const liminal::script>().each())
    // {
    // }
}

void liminal::scene::on_audio_source_construct(entt::registry &, entt::entity id)
{
    auto &audio_source = get_entity(id).get_component<liminal::audio_source>();
    audio_source.source = std::make_shared<liminal::source>();
    sources.push_back(audio_source.source);
}

void liminal::scene::on_audio_source_destroy(entt::registry &, entt::entity id)
{
    auto &audio_source = get_entity(id).get_component<liminal::audio_source>();
    sources.erase(std::find(sources.begin(), sources.end(), audio_source.source));
    audio_source.source = {};
}

void liminal::scene::on_physical_construct(entt::registry &, entt::entity id)
{
    auto &physical = get_entity(id).get_component<liminal::physical>();

    const auto motion_state = new btDefaultMotionState();
    const auto collision_shape = new btBoxShape(btVector3(1, 1, 1));
    btVector3 local_inertia;
    collision_shape->calculateLocalInertia(physical.mass, local_inertia);
    const auto construction_info = btRigidBody::btRigidBodyConstructionInfo(physical.mass, motion_state, collision_shape, local_inertia);
    physical.rigidbody = std::make_shared<btRigidBody>(construction_info);
    world->addRigidBody(physical.rigidbody.get());
}

void liminal::scene::on_physical_destroy(entt::registry &, entt::entity id)
{
    auto &physical = get_entity(id).get_component<liminal::physical>();
    world->removeRigidBody(physical.rigidbody.get());
    physical.rigidbody = {};
}