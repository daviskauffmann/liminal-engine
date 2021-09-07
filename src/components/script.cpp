#include "script.hpp"

#include "../model.hpp"
#include "../skybox.hpp"
#include "mesh_renderer.hpp"
#include "point_light.hpp"
#include "script.hpp"
#include "transform.hpp"

void load_functions(sol::state *lua, entt::entity entity)
{
    // (*lua)["SetSkybox"] = [](const std::string &filename) -> void
    // {
    //     renderer->skybox = new liminal::skybox(filename);
    // };
    // (*lua)["AddEntity"] = []() -> entt::entity
    // {
    //     return registry->create();
    // };
    // (*lua)["GetEntity"] = [entity]() -> entt::entity
    // {
    //     return entity;
    // };
    // (*lua)["AddTransform"] = [](entt::entity entity, float x, float y, float z, float rx, float ry, float rz, float sx, float sy, float sz, float mass) -> void
    // {
    //     auto transform = registry->emplace<liminal::transform>(entity, nullptr, glm::vec3(x, y, z), glm::vec3(rx, ry, rz), glm::vec3(sx, sy, sz), mass);

    //     if (mass > 0.0f)
    //     {
    //         physics->world->addRigidBody(transform.rigidbody);
    //     }
    // };
    // (*lua)["UpdateTransform"] = [](entt::entity entity, float x, float y, float z, float rx, float ry, float rz, float sx, float sy, float sz) -> void
    // {
    //     auto &transform = registry->get<liminal::transform>(entity);
    //     transform.position = glm::vec3(x, y, z);
    //     transform.rotation = glm::vec3(rx, ry, rz);
    //     transform.scale = glm::vec3(sx, sy, sz);
    // };
    // (*lua)["AddMeshRenderer"] = [](entt::entity entity, const std::string &filename, bool flip_uvs) -> void
    // {
    //     registry->emplace<liminal::mesh_renderer>(entity, new liminal::model(filename, flip_uvs));
    // };
    // (*lua)["UpdateMeshRenderer"] = [](entt::entity entity, const std::string &filename, bool flip_uvs) -> void
    // {
    //     auto &mesh_renderer = registry->get<liminal::mesh_renderer>(entity);
    //     mesh_renderer.model = new liminal::model(filename, flip_uvs);
    // };
    // (*lua)["AddPointLight"] = [](entt::entity entity, float r, float g, float b) -> void
    // {
    //     registry->emplace<liminal::point_light>(entity, glm::vec3(r, g, b), 512);
    // };
    // (*lua)["UpdatePointLight"] = [](entt::entity entity, float r, float g, float b) -> void
    // {
    //     auto &point_light = registry->get<liminal::point_light>(entity);
    //     point_light.color = glm::vec3(r, g, b);
    // };
    // (*lua)["AddScript"] = [](entt::entity entity, const std::string &filename) -> void
    // {
    //     registry->emplace<liminal::script>(entity, filename, entity);
    // };
}

liminal::script::script(const std::string &filename, entt::entity entity)
    : entity(entity)
{
    lua = new sol::state;
    lua->open_libraries(sol::lib::base, sol::lib::math);
    load_functions(lua, entity);
    lua->script_file(filename);
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
