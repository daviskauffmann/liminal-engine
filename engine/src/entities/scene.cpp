#include <liminal/entities/scene.hpp>

#include <fstream>
#include <liminal/audio/listener.hpp>
#include <liminal/audio/sound.hpp>
#include <liminal/audio/source.hpp>
#include <liminal/components/audio_listener.hpp>
#include <liminal/components/audio_source.hpp>
#include <liminal/components/camera.hpp>
#include <liminal/components/character.hpp>
#include <liminal/components/directional_light.hpp>
#include <liminal/components/physical.hpp>
#include <liminal/components/point_light.hpp>
#include <liminal/components/renderable.hpp>
#include <liminal/components/script.hpp>
#include <liminal/components/spot_light.hpp>
#include <liminal/components/terrain.hpp>
#include <liminal/components/transform.hpp>
#include <liminal/components/water.hpp>
#include <liminal/core/assets.hpp>
#include <liminal/entities/entity.hpp>
#include <liminal/graphics/model.hpp>
#include <liminal/graphics/skybox.hpp>
#include <liminal/physics/character_controller.hpp>
#include <liminal/physics/rigidbody.hpp>
#include <liminal/physics/world.hpp>
#include <liminal/scripting/lua_state.hpp>
#include <nlohmann/json.hpp>

liminal::entities::scene::scene(std::shared_ptr<liminal::core::assets> assets)
    : assets(assets)
{
    registry.on_construct<liminal::components::audio_source>().connect<&scene::on_audio_source_construct>(this);
    registry.on_construct<liminal::components::character>().connect<&scene::on_character_construct>(this);
    registry.on_destroy<liminal::components::character>().connect<&scene::on_character_destroy>(this);
    registry.on_construct<liminal::components::physical>().connect<&scene::on_physical_construct>(this);
    registry.on_destroy<liminal::components::physical>().connect<&scene::on_physical_destroy>(this);
    registry.on_construct<liminal::components::script>().connect<&scene::on_script_construct>(this);

    world = std::make_unique<liminal::physics::world>();
}

liminal::entities::scene::~scene()
{
    registry.clear();
}

void liminal::entities::scene::load(const std::string &filename)
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
                    if (component_type == "audio_listener")
                    {
                        entity.add_component<liminal::components::audio_listener>();
                    }

                    if (component_type == "camera")
                    {
                        entity.add_component<liminal::components::camera>(
                            component_json.at("fov"));
                    }

                    if (component_type == "directional_light")
                    {
                        entity.add_component<liminal::components::directional_light>(
                            glm::vec3(
                                component_json.at("color").at("r"),
                                component_json.at("color").at("g"),
                                component_json.at("color").at("b")));
                    }

                    if (component_type == "physical")
                    {
                        entity.add_component<liminal::components::physical>(
                            component_json.at("mass"));
                    }

                    if (component_type == "script")
                    {
                        entity.add_component<liminal::components::script>(component_json.at("filename"));
                    }

                    if (component_type == "spot_light")
                    {
                        entity.add_component<liminal::components::spot_light>(
                            glm::vec3(
                                component_json.at("color").at("r"),
                                component_json.at("color").at("g"),
                                component_json.at("color").at("b")),
                            component_json.at("inner_cutoff"),
                            component_json.at("outer_cutoff"));
                    }

                    if (component_type == "terrain")
                    {
                        entity.add_component<liminal::components::terrain>(
                            component_json.at("filename"),
                            component_json.at("tiling"),
                            assets);
                    }

                    if (component_type == "transform")
                    {
                        entity.add_component<liminal::components::transform>(
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

                    if (component_type == "renderable")
                    {
                        entity.add_component<liminal::components::renderable>(
                            assets->load_model(
                                component_json.at("filename"),
                                assets,
                                component_json.at("flip_uvs")));
                    }

                    if (component_type == "water")
                    {
                        entity.add_component<liminal::components::water>(component_json.at("tiling"));
                    }
                }
            }
        }
    }
}

void liminal::entities::scene::save(const std::string &)
{
}

liminal::entities::entity liminal::entities::scene::create_entity()
{
    const auto id = registry.create();
    return liminal::entities::entity(id, this);
}

liminal::entities::entity liminal::entities::scene::get_entity(const entt::entity id)
{
    return liminal::entities::entity(id, this);
}

void liminal::entities::scene::delete_entity(const liminal::entities::entity entity)
{
    registry.destroy(entity);
}

void liminal::entities::scene::start()
{
    // init scripts
    for (const auto [id, script] : get_entities_with<const liminal::components::script>().each())
    {
        script.lua_state->init();
    }
}

void liminal::entities::scene::update(const std::uint64_t current_time, const float delta_time)
{
    // update scripts
    for (const auto [id, script] : get_entities_with<const liminal::components::script>().each())
    {
        script.lua_state->update(delta_time);
    }

    // update animations
    for (const auto [id, renderable] : get_entities_with<liminal::components::renderable>().each())
    {
        if (renderable.model && renderable.model->has_animations())
        {
            // TODO: put current animation_index on the renderable component
            renderable.bone_transformations = renderable.model->calc_bone_transformations(0, current_time);
        }
    }

    // update audio listener positions
    for (const auto [id, audio_listener, camera, transform] : get_entities_with<liminal::components::audio_listener, const liminal::components::camera, const liminal::components::transform>().each())
    {
        liminal::audio::listener::set_position(transform.position);
        liminal::audio::listener::set_velocity(audio_listener.last_position - transform.position);
        liminal::audio::listener::set_orientation(camera.calc_front(transform), glm::vec3(0, 1, 0));

        audio_listener.last_position = transform.position;
    }

    // update audio source positions
    for (const auto [id, audio_source, transform] : get_entities_with<liminal::components::audio_source, const liminal::components::transform>().each())
    {
        audio_source.source->set_position(transform.position);
        audio_source.source->set_velocity(audio_source.last_position - transform.position);

        audio_source.last_position = transform.position;
    }

    {
        const auto physics_entities = get_entities_with<const liminal::components::physical, liminal::components::transform>().each();

        // update rigidbody in case the transform component changed
        for (const auto [id, physical, transform] : physics_entities)
        {
            physical.rigidbody->set_world_transform(
                transform.position,
                transform.rotation,
                transform.scale,
                physical.mass);
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

void liminal::entities::scene::stop()
{
}

void liminal::entities::scene::reload_scripts()
{
    // TODO:
    // for (const auto [id, script] : get_entities_with<const liminal::components::script>().each())
    // {
    // }
}

liminal::entities::scene *liminal::entities::scene::copy()
{
    auto new_scene = new liminal::entities::scene(assets);

    new_scene->skybox = skybox;

    for (const auto [id, transform] : get_entities_with<liminal::components::transform>().each())
    {
        const auto entity = get_entity(id);
        auto new_entity = new_scene->create_entity();

        if (entity.has_components<liminal::components::audio_listener>())
        {
            new_entity.add_component<liminal::components::audio_listener>();
        }

        if (entity.has_components<liminal::components::audio_source>())
        {
            const auto audio_source = entity.get_component<liminal::components::audio_source>();

            new_entity.add_component<liminal::components::audio_source>();
        }

        if (entity.has_components<liminal::components::camera>())
        {
            const auto camera = entity.get_component<liminal::components::camera>();

            new_entity.add_component<liminal::components::camera>(camera.fov);
        }

        if (entity.has_components<liminal::components::directional_light>())
        {
            const auto directional_light = entity.get_component<liminal::components::directional_light>();

            new_entity.add_component<liminal::components::directional_light>(directional_light.color);
        }

        if (entity.has_components<liminal::components::physical>())
        {
            const auto physical = entity.get_component<liminal::components::physical>();

            new_entity.add_component<liminal::components::physical>(physical.mass);
        }

        if (entity.has_components<liminal::components::renderable>())
        {
            const auto renderable = entity.get_component<liminal::components::renderable>();

            new_entity.add_component<liminal::components::renderable>(renderable.model);
        }

        if (entity.has_components<liminal::components::script>())
        {
            const auto script = entity.get_component<liminal::components::script>();

            new_entity.add_component<liminal::components::script>(script.filename);
        }

        if (entity.has_components<liminal::components::spot_light>())
        {
            const auto spot_light = entity.get_component<liminal::components::spot_light>();

            new_entity.add_component<liminal::components::spot_light>(spot_light.color, spot_light.inner_cutoff, spot_light.outer_cutoff);
        }

        // if (entity.has_components<liminal::terrain>())
        // {
        //     const auto terrain = entity.get_component<liminal::terrain>();

        //     new_entity.add_component<liminal::terrain>(terrain.filename, terrain.tiling, assets);
        // }

        new_entity.add_component<liminal::components::transform>(
            transform.name,
            nullptr,
            transform.position,
            transform.rotation,
            transform.scale);

        if (entity.has_components<liminal::components::water>())
        {
            const auto water = entity.get_component<liminal::components::water>();

            new_entity.add_component<liminal::components::water>(water.tiling);
        }
    }

    return new_scene;
}

void liminal::entities::scene::on_audio_source_construct(entt::registry &, entt::entity id)
{
    auto &audio_source = get_entity(id).get_component<liminal::components::audio_source>();

    audio_source.source = std::make_shared<liminal::audio::source>();
}

void liminal::entities::scene::on_character_construct(entt::registry &, entt::entity id)
{
    auto &character = get_entity(id).get_component<liminal::components::character>();

    character.character_controller = std::make_shared<liminal::physics::character_controller>();

    world->add_character_controller(character.character_controller);
}

void liminal::entities::scene::on_character_destroy(entt::registry &, entt::entity id)
{
    auto &character = get_entity(id).get_component<liminal::components::character>();

    world->remove_character_controller(character.character_controller);
}

void liminal::entities::scene::on_physical_construct(entt::registry &, entt::entity id)
{
    auto &physical = get_entity(id).get_component<liminal::components::physical>();

    physical.rigidbody = std::make_shared<liminal::physics::rigidbody>(physical.mass);

    world->add_rigidbody(physical.rigidbody);
}

void liminal::entities::scene::on_physical_destroy(entt::registry &, entt::entity id)
{
    auto &physical = get_entity(id).get_component<liminal::components::physical>();

    world->remove_rigidbody(physical.rigidbody);
}

void liminal::entities::scene::on_script_construct(entt::registry &, entt::entity id)
{
    auto &script = get_entity(id).get_component<liminal::components::script>();

    script.lua_state = std::make_shared<liminal::scripting::lua_state>(
        script.filename,
        this,
        id,
        assets);
}
