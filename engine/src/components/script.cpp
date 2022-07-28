#include <liminal/components/script.hpp>

#include <liminal/components/camera.hpp>
#include <liminal/components/mesh_renderer.hpp>
#include <liminal/components/point_light.hpp>
#include <liminal/components/transform.hpp>
#include <liminal/core/assets.hpp>
#include <liminal/core/entity.hpp>
#include <liminal/core/scene.hpp>
#include <liminal/graphics/model.hpp>
#include <liminal/graphics/skybox.hpp>
#include <liminal/input/input.hpp>

liminal::script::script(const std::string &filename, liminal::scene *const scene, const entt::entity id)
{
    lua.open_libraries(sol::lib::base, sol::lib::math);
    const auto result = lua.script_file(filename);
    if (!result.valid())
    {
        std::cerr << "Error: Failed to load script: " << filename << std::endl;
        return;
    }

    lua["GetKeyDown"] = [](liminal::keycode keycode) -> bool
    {
        return liminal::input::key_down(keycode);
    };
    lua["SetSkybox"] = [scene](const std::string &filename) -> void
    {
        scene->skybox = liminal::assets::instance->load<liminal::skybox>(filename);
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
        for (const auto [id, transform] : scene->get_entities_with<const liminal::transform>().each())
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
        entity.add_component<liminal::transform>(name, nullptr, glm::vec3(x, y, z), glm::vec3(rx, ry, rz), glm::vec3(sx, sy, sz));
    };
    lua["GetTransformX"] = [scene](entt::entity id) -> float
    {
        const auto entity = scene->get_entity(id);
        const auto transform = entity.get_component<liminal::transform>();
        return transform.position.x;
    };
    lua["GetTransformY"] = [scene](entt::entity id) -> float
    {
        const auto entity = scene->get_entity(id);
        const auto transform = entity.get_component<liminal::transform>();
        return transform.position.y;
    };
    lua["GetTransformZ"] = [scene](entt::entity id) -> float
    {
        const auto entity = scene->get_entity(id);
        const auto transform = entity.get_component<liminal::transform>();
        return transform.position.z;
    };
    lua["GetTransformRotX"] = [scene](entt::entity id) -> float
    {
        const auto entity = scene->get_entity(id);
        const auto transform = entity.get_component<liminal::transform>();
        return transform.rotation.x;
    };
    lua["GetTransformRotY"] = [scene](entt::entity id) -> float
    {
        const auto entity = scene->get_entity(id);
        const auto transform = entity.get_component<liminal::transform>();
        return transform.rotation.y;
    };
    lua["GetTransformRotZ"] = [scene](entt::entity id) -> float
    {
        const auto entity = scene->get_entity(id);
        const auto transform = entity.get_component<liminal::transform>();
        return transform.rotation.z;
    };
    lua["UpdateTransform"] = [scene](entt::entity id, float x, float y, float z, float rx, float ry, float rz, float sx, float sy, float sz) -> void
    {
        const auto entity = scene->get_entity(id);
        auto &transform = entity.get_component<liminal::transform>();
        transform.position = {x, y, z};
        transform.rotation = {rx, ry, rz};
        transform.scale = {sx, sy, sz};
    };
    lua["AddMeshRenderer"] = [scene](entt::entity id, const std::string &filename, bool flip_uvs) -> void
    {
        auto entity = scene->get_entity(id);
        entity.add_component<liminal::mesh_renderer>(liminal::assets::instance->load<liminal::model>(filename, flip_uvs));
    };
    lua["UpdateMeshRenderer"] = [scene](entt::entity id, const std::string &filename, bool flip_uvs) -> void
    {
        auto entity = scene->get_entity(id);
        auto &mesh_renderer = entity.get_component<liminal::mesh_renderer>();
        mesh_renderer.model = liminal::assets::instance->load<liminal::model>(filename, flip_uvs);
    };
    lua["AddPointLight"] = [scene](entt::entity id, float r, float g, float b) -> void
    {
        auto entity = scene->get_entity(id);
        entity.add_component<liminal::point_light>(glm::vec3(r, g, b));
    };
    lua["UpdatePointLight"] = [scene](entt::entity id, float r, float g, float b) -> void
    {
        auto entity = scene->get_entity(id);
        auto &point_light = entity.get_component<liminal::point_light>();
        point_light.color = {r, g, b};
    };
    lua["GetCameraFrontX"] = [scene](entt::entity id) -> float
    {
        const auto entity = scene->get_entity(id);
        const auto camera = entity.get_component<liminal::camera>();
        const auto transform = entity.get_component<liminal::transform>();
        return camera.calc_front(transform).x;
    };
    lua["GetCameraFrontY"] = [scene](entt::entity id) -> float
    {
        const auto entity = scene->get_entity(id);
        const auto camera = entity.get_component<liminal::camera>();
        const auto transform = entity.get_component<liminal::transform>();
        return camera.calc_front(transform).y;
    };
    lua["GetCameraFrontZ"] = [scene](entt::entity id) -> float
    {
        const auto entity = scene->get_entity(id);
        const auto camera = entity.get_component<liminal::camera>();
        const auto transform = entity.get_component<liminal::transform>();
        return camera.calc_front(transform).z;
    };
    lua["AddScript"] = [scene](entt::entity id, const std::string &filename) -> void
    {
        auto entity = scene->get_entity(id);
        entity.add_component<liminal::script>(filename, scene, id);
    };
}

void liminal::script::init() const
{
    lua["Init"]();
}

void liminal::script::update(const float delta_time) const
{
    lua["Update"](delta_time);
}
