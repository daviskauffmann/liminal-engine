#include <liminal/components/script.hpp>

#include <liminal/components/mesh_renderer.hpp>
#include <liminal/components/point_light.hpp>
#include <liminal/components/transform.hpp>
#include <liminal/core/engine.hpp>
#include <liminal/core/entity.hpp>
#include <liminal/core/scene.hpp>
#include <liminal/graphics/model.hpp>
#include <liminal/graphics/skybox.hpp>
#include <liminal/input/input.hpp>
#include <sol/sol.hpp>

liminal::script::script(const std::string &filename, liminal::scene *scene, entt::entity id)
{
    lua = new sol::state;
    lua->open_libraries(sol::lib::base, sol::lib::math);
    lua->script_file(filename);

    (*lua)["GetKeyDown"] = [](liminal::keycode keycode) -> bool
    {
        return liminal::input::key_down(keycode);
    };
    (*lua)["SetSkybox"] = [scene](const std::string &filename) -> void
    {
        if (scene->skybox)
        {
            delete scene->skybox;
        }

        scene->skybox = new liminal::skybox(filename);
    };
    (*lua)["AddEntity"] = [scene]() -> entt::entity
    {
        return scene->create_entity().get_id();
    };
    (*lua)["GetEntity"] = [id]() -> entt::entity
    {
        return id;
    };
    (*lua)["GetEntityByName"] = [scene](const std::string &name) -> entt::entity
    {
        for (auto [entity, transform] : scene->registry.view<liminal::transform>().each())
        {
            if (transform.name == name)
            {
                return entity;
            }
        }
        return entt::entity();
    };
    (*lua)["AddTransform"] = [scene](entt::entity id, float x, float y, float z, float rx, float ry, float rz, float sx, float sy, float sz) -> void
    {
        auto entity = scene->get_entity(id);
        auto transform = entity.add_component<liminal::transform>("NAME", nullptr, glm::vec3(x, y, z), glm::vec3(rx, ry, rz), glm::vec3(sx, sy, sz));
    };
    (*lua)["GetTransformX"] = [scene](entt::entity id) -> float
    {
        auto entity = scene->get_entity(id);
        auto &transform = entity.get_component<liminal::transform>();
        return transform.position.x;
    };
    (*lua)["GetTransformY"] = [scene](entt::entity id) -> float
    {
        auto entity = scene->get_entity(id);
        auto &transform = entity.get_component<liminal::transform>();
        return transform.position.y;
    };
    (*lua)["GetTransformZ"] = [scene](entt::entity id) -> float
    {
        auto entity = scene->get_entity(id);
        auto &transform = entity.get_component<liminal::transform>();
        return transform.position.z;
    };
    (*lua)["GetTransformRotX"] = [scene](entt::entity id) -> float
    {
        auto entity = scene->get_entity(id);
        auto &transform = entity.get_component<liminal::transform>();
        return transform.rotation.x;
    };
    (*lua)["GetTransformRotY"] = [scene](entt::entity id) -> float
    {
        auto entity = scene->get_entity(id);
        auto &transform = entity.get_component<liminal::transform>();
        return transform.rotation.y;
    };
    (*lua)["GetTransformRotZ"] = [scene](entt::entity id) -> float
    {
        auto entity = scene->get_entity(id);
        auto &transform = entity.get_component<liminal::transform>();
        return transform.rotation.z;
    };
    (*lua)["UpdateTransform"] = [scene](entt::entity id, float x, float y, float z, float rx, float ry, float rz, float sx, float sy, float sz) -> void
    {
        auto entity = scene->get_entity(id);
        auto &transform = entity.get_component<liminal::transform>();
        transform.position = glm::vec3(x, y, z);
        transform.rotation = glm::vec3(rx, ry, rz);
        transform.scale = glm::vec3(sx, sy, sz);
    };
    (*lua)["AddMeshRenderer"] = [scene](entt::entity id, const std::string &filename, bool flip_uvs) -> void
    {
        auto entity = scene->get_entity(id);
        entity.add_component<liminal::mesh_renderer>(new liminal::model(filename, flip_uvs)); // TODO: asset manager
    };
    (*lua)["UpdateMeshRenderer"] = [scene](entt::entity id, const std::string &filename, bool flip_uvs) -> void
    {
        auto entity = scene->get_entity(id);
        auto &mesh_renderer = entity.get_component<liminal::mesh_renderer>();
        mesh_renderer.model = new liminal::model(filename, flip_uvs); // TODO: asset manager
    };
    (*lua)["AddPointLight"] = [scene](entt::entity id, float r, float g, float b) -> void
    {
        auto entity = scene->get_entity(id);
        entity.add_component<liminal::point_light>(glm::vec3(r, g, b));
    };
    (*lua)["UpdatePointLight"] = [scene](entt::entity id, float r, float g, float b) -> void
    {
        auto entity = scene->get_entity(id);
        auto &point_light = entity.get_component<liminal::point_light>();
        point_light.color = glm::vec3(r, g, b);
    };
    (*lua)["AddScript"] = [scene](entt::entity id, const std::string &filename) -> void
    {
        auto entity = scene->get_entity(id);
        entity.add_component<liminal::script>(filename, scene, id);
    };
}

liminal::script::~script()
{
    delete lua;
}

void liminal::script::init()
{
    (*lua)["Init"]();
}

void liminal::script::update(float delta_time)
{
    (*lua)["Update"](delta_time);
}
