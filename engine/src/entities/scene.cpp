#include <liminal/entities/scene.hpp>

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
#include <liminal/physics/rigidbody.hpp>
#include <liminal/physics/world.hpp>
#include <liminal/scripting/lua_state.hpp>
#include <nlohmann/json.hpp>

liminal::scene::scene(std::shared_ptr<liminal::assets> assets)
    : assets(assets)
{
    registry.on_construct<liminal::audio_source>().connect<&scene::on_audio_source_construct>(this);
    registry.on_construct<liminal::physical>().connect<&scene::on_physical_construct>(this);
    registry.on_destroy<liminal::physical>().connect<&scene::on_physical_destroy>(this);
    registry.on_construct<liminal::script>().connect<&scene::on_script_construct>(this);

    world = std::make_unique<liminal::world>();
}

liminal::scene::scene(const liminal::scene &)
{
    // TODO: copy all entities to new scene
    // oh boy
}

liminal::scene::~scene()
{
    registry.clear();
}

void liminal::scene::load(const std::string &filename)
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
                        entity.add_component<liminal::script>(component_json.at("filename"));
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
        script.lua_state->init();
    }
}

void liminal::scene::update(const std::uint64_t current_time, const float delta_time)
{
    // update scripts
    for (const auto [id, script] : get_entities_with<const liminal::script>().each())
    {
        script.lua_state->update(delta_time);
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

    {
        const auto physics_entities = get_entities_with<const liminal::physical, liminal::transform>().each();

        // update rigidbody in case the transform component changed
        for (const auto [id, physical, transform] : physics_entities)
        {
            physical.rigidbody->set_world_transform(transform.position, transform.rotation);
        }

        // update physics world
        world->update(delta_time);

        // update transforms to respect physics simulation
        for (const auto [id, physical, transform] : physics_entities)
        {
            const auto [position, rotation] = physical.rigidbody->get_world_transform();
            transform.position = position;
            transform.rotation = rotation;
        }
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
}

void liminal::scene::on_physical_construct(entt::registry &, entt::entity id)
{
    auto &physical = get_entity(id).get_component<liminal::physical>();
    physical.rigidbody = std::make_shared<liminal::rigidbody>(physical.mass);
    world->add_rigidbody(physical.rigidbody);
}

void liminal::scene::on_physical_destroy(entt::registry &, entt::entity id)
{
    auto &physical = get_entity(id).get_component<liminal::physical>();
    world->remove_rigidbody(physical.rigidbody);
}

void liminal::scene::on_script_construct(entt::registry &, entt::entity id)
{
    auto &script = get_entity(id).get_component<liminal::script>();
    script.lua_state = std::make_shared<liminal::lua_state>(
        script.filename,
        this,
        id,
        assets);
}
