#include <liminal/scripting/lua_state.hpp>

#include <liminal/components/camera.hpp>
#include <liminal/components/point_light.hpp>
#include <liminal/components/renderable.hpp>
#include <liminal/components/script.hpp>
#include <liminal/components/transform.hpp>
#include <liminal/core/assets.hpp>
#include <liminal/entities/entity.hpp>
#include <liminal/entities/scene.hpp>
#include <liminal/graphics/model.hpp>
#include <liminal/graphics/skybox.hpp>
#include <liminal/input/input.hpp>
#include <stdexcept>
#include <string>

liminal::scripting::lua_state::lua_state(
    const std::string &filename,
    liminal::entities::scene *const scene,
    const entt::entity id,
    std::shared_ptr<liminal::core::assets> assets)
{
    lua.open_libraries(sol::lib::base, sol::lib::math);
    const auto result = lua.script_file(filename);
    if (!result.valid())
    {
        sol::error error = result;
        throw std::runtime_error(error.what());
    }

    lua["GetKeyDown"] = [](liminal::input::keycode keycode) -> bool
    {
        return liminal::input::input::key_down(keycode);
    };
    lua["SetSkybox"] = [scene, assets](const std::string &filename) -> void
    {
        scene->skybox = assets->load_skybox(filename.c_str());
    };
    lua["AddEntity"] = [scene]() -> entt::entity
    {
        return scene->create_entity();
    };
    lua["GetEntity"] = [id]() -> entt::entity
    {
        return id;
    };
    lua["GetEntityByName"] = [scene](const std::string &name) -> entt::entity
    {
        for (const auto [id, transform] : scene->get_entities_with<const liminal::components::transform>().each())
        {
            if (transform.name == name)
            {
                return id;
            }
        }
        return entt::null;
    };
    lua["AddTransform"] = [scene](entt::entity id, const std::string &name, float x, float y, float z, float rx, float ry, float rz, float sx, float sy, float sz) -> void
    {
        auto entity = scene->get_entity(id);
        entity.add_component<liminal::components::transform>(name, nullptr, glm::vec3(x, y, z), glm::vec3(rx, ry, rz), glm::vec3(sx, sy, sz));
    };
    lua["GetTransformX"] = [scene](entt::entity id) -> float
    {
        const auto entity = scene->get_entity(id);
        const auto transform = entity.get_component<liminal::components::transform>();
        return transform.position.x;
    };
    lua["GetTransformY"] = [scene](entt::entity id) -> float
    {
        const auto entity = scene->get_entity(id);
        const auto transform = entity.get_component<liminal::components::transform>();
        return transform.position.y;
    };
    lua["GetTransformZ"] = [scene](entt::entity id) -> float
    {
        const auto entity = scene->get_entity(id);
        const auto transform = entity.get_component<liminal::components::transform>();
        return transform.position.z;
    };
    lua["GetTransformRotX"] = [scene](entt::entity id) -> float
    {
        const auto entity = scene->get_entity(id);
        const auto transform = entity.get_component<liminal::components::transform>();
        return transform.rotation.x;
    };
    lua["GetTransformRotY"] = [scene](entt::entity id) -> float
    {
        const auto entity = scene->get_entity(id);
        const auto transform = entity.get_component<liminal::components::transform>();
        return transform.rotation.y;
    };
    lua["GetTransformRotZ"] = [scene](entt::entity id) -> float
    {
        const auto entity = scene->get_entity(id);
        const auto transform = entity.get_component<liminal::components::transform>();
        return transform.rotation.z;
    };
    lua["UpdateTransform"] = [scene](entt::entity id, float x, float y, float z, float rx, float ry, float rz, float sx, float sy, float sz) -> void
    {
        const auto entity = scene->get_entity(id);
        auto &transform = entity.get_component<liminal::components::transform>();
        transform.position = {x, y, z};
        transform.rotation = {rx, ry, rz};
        transform.scale = {sx, sy, sz};
    };
    lua["AddMeshRenderer"] = [scene, assets](entt::entity id, const std::string &filename, bool flip_uvs) -> void
    {
        auto entity = scene->get_entity(id);
        entity.add_component<liminal::components::renderable>(assets->load_model(filename.c_str(), assets, flip_uvs));
    };
    lua["UpdateMeshRenderer"] = [scene, assets](entt::entity id, const std::string &filename, bool flip_uvs) -> void
    {
        auto entity = scene->get_entity(id);
        auto &renderable = entity.get_component<liminal::components::renderable>();
        renderable.model = assets->load_model(filename.c_str(), assets, flip_uvs);
    };
    lua["AddPointLight"] = [scene](entt::entity id, float r, float g, float b) -> void
    {
        auto entity = scene->get_entity(id);
        entity.add_component<liminal::components::point_light>(glm::vec3(r, g, b));
    };
    lua["UpdatePointLight"] = [scene](entt::entity id, float r, float g, float b) -> void
    {
        auto entity = scene->get_entity(id);
        auto &point_light = entity.get_component<liminal::components::point_light>();
        point_light.color = {r, g, b};
    };
    lua["GetCameraFrontX"] = [scene](entt::entity id) -> float
    {
        const auto entity = scene->get_entity(id);
        const auto camera = entity.get_component<liminal::components::camera>();
        const auto transform = entity.get_component<liminal::components::transform>();
        return camera.calc_front(transform).x;
    };
    lua["GetCameraFrontY"] = [scene](entt::entity id) -> float
    {
        const auto entity = scene->get_entity(id);
        const auto camera = entity.get_component<liminal::components::camera>();
        const auto transform = entity.get_component<liminal::components::transform>();
        return camera.calc_front(transform).y;
    };
    lua["GetCameraFrontZ"] = [scene](entt::entity id) -> float
    {
        const auto entity = scene->get_entity(id);
        const auto camera = entity.get_component<liminal::components::camera>();
        const auto transform = entity.get_component<liminal::components::transform>();
        return camera.calc_front(transform).z;
    };
    lua["AddScript"] = [scene](entt::entity id, const std::string &filename) -> void
    {
        auto entity = scene->get_entity(id);
        entity.add_component<liminal::components::script>(filename);
    };
}

void liminal::scripting::lua_state::init() const
{
    lua["Init"]();
}

void liminal::scripting::lua_state::update(const float delta_time) const
{
    lua["Update"](delta_time);
}
