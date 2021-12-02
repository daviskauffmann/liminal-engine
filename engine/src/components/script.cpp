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
    // lua = new sol::state;
    // lua->open_libraries(sol::lib::base, sol::lib::math);
    // lua->script_file(filename);

    // (*lua)["SetSkybox"] = [scene](const std::string &filename) -> void
    // {
    //     // scene->skybox = new liminal::skybox(filename);
    // };
    // (*lua)["AddEntity"] = [scene]() -> entt::entity
    // {
    //     return scene->registry.create();
    // };
    // (*lua)["GetEntity"] = [entity]() -> entt::entity
    // {
    //     return entity;
    // };
    // (*lua)["AddTransform"] = [scene](entt::entity entity, float x, float y, float z, float rx, float ry, float rz, float sx, float sy, float sz) -> void
    // {
    //     auto transform = scene->registry.emplace<liminal::transform>(entity, "NAME", nullptr, glm::vec3(x, y, z), glm::vec3(rx, ry, rz), glm::vec3(sx, sy, sz));
    // };
    // (*lua)["UpdateTransform"] = [scene](entt::entity entity, float x, float y, float z, float rx, float ry, float rz, float sx, float sy, float sz) -> void
    // {
    //     auto &transform = scene->registry.get<liminal::transform>(entity);
    //     transform.position = glm::vec3(x, y, z);
    //     transform.rotation = glm::vec3(rx, ry, rz);
    //     transform.scale = glm::vec3(sx, sy, sz);
    // };
    // (*lua)["AddMeshRenderer"] = [scene](entt::entity entity, const std::string &filename, bool flip_uvs) -> void
    // {
    //     scene->registry.emplace<liminal::mesh_renderer>(entity, new liminal::model(filename, flip_uvs));
    // };
    // (*lua)["UpdateMeshRenderer"] = [scene](entt::entity entity, const std::string &filename, bool flip_uvs) -> void
    // {
    //     auto &mesh_renderer = scene->registry.get<liminal::mesh_renderer>(entity);
    //     mesh_renderer.model = new liminal::model(filename, flip_uvs);
    // };
    // (*lua)["AddPointLight"] = [scene](entt::entity entity, float r, float g, float b) -> void
    // {
    //     scene->registry.emplace<liminal::point_light>(entity, glm::vec3(r, g, b));
    // };
    // (*lua)["UpdatePointLight"] = [scene](entt::entity entity, float r, float g, float b) -> void
    // {
    //     auto &point_light = scene->registry.get<liminal::point_light>(entity);
    //     point_light.color = glm::vec3(r, g, b);
    // };
    // (*lua)["AddScript"] = [scene](entt::entity entity, const std::string &filename) -> void
    // {
    //     scene->registry.emplace<liminal::script>(entity, filename, scene, entity);
    // };
}

void liminal::script::init()
{
    // (*lua)["Init"]();
}

void liminal::script::update(float delta_time)
{
    // (*lua)["Update"](delta_time);
}
