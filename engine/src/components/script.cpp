#include <liminal/components/script.hpp>

#include <liminal/components/mesh_renderer.hpp>
#include <liminal/components/point_light.hpp>
#include <liminal/components/transform.hpp>
#include <liminal/core/engine.hpp>
#include <liminal/core/entity.hpp>
#include <liminal/core/scene.hpp>
#include <liminal/graphics/model.hpp>
#include <liminal/graphics/skybox.hpp>
#include <sol/sol.hpp>

liminal::script::script(const std::string &filename, liminal::scene *scene, liminal::entity *entity)
    : entity(entity)
{
    lua = new sol::state;
    lua->open_libraries(sol::lib::base, sol::lib::math);
    lua->script_file(filename);

    (*lua)["SetSkybox"] = [scene](const std::string &filename) -> void
    {
        // scene->skybox = new liminal::skybox(filename);
    };
    (*lua)["AddEntity"] = [scene]() -> entt::entity
    {
        return scene->create_entity().get_id();
    };
    (*lua)["GetEntity"] = [entity]() -> entt::entity
    {
        return entity->get_id();
    };
    (*lua)["AddTransform"] = [scene](entt::entity id, float x, float y, float z, float rx, float ry, float rz, float sx, float sy, float sz) -> void
    {
        auto entity = scene->get_entity(id);
        auto transform = entity.add_component<liminal::transform>("NAME", nullptr, glm::vec3(x, y, z), glm::vec3(rx, ry, rz), glm::vec3(sx, sy, sz));
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
        entity.add_component<liminal::script>(filename, scene, &entity);
    };
}

void liminal::script::init()
{
    (*lua)["Init"]();
}

void liminal::script::update(float delta_time)
{
    (*lua)["Update"](delta_time);
}
