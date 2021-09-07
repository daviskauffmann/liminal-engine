#include "scene.hpp"

#include <fstream>
#include <nlohmann/json.hpp>

#include "components/audio_source.hpp"
#include "components/directional_light.hpp"
#include "components/mesh_renderer.hpp"
#include "components/point_light.hpp"
#include "components/script.hpp"
#include "components/spot_light.hpp"
#include "components/terrain.hpp"
#include "components/transform.hpp"
#include "components/water.hpp"
#include "audio.hpp"
#include "camera.hpp"
#include "engine.hpp"
#include "model.hpp"
#include "platform.hpp"
#include "renderer.hpp"
#include "skybox.hpp"
#include "sound.hpp"

void load_scene(const std::string &filename, liminal::engine *engine, liminal::scene *scene)
{
    std::ifstream stream(filename);
    nlohmann::json json;
    stream >> json;
    for (auto &[key, value] : json.items())
    {
        if (key == "skybox")
        {
            engine->renderer->skybox = new liminal::skybox(json["skybox"]);
        }

        if (key == "entities")
        {
            for (auto &element : value)
            {
                auto entity = scene->registry.create();

                for (auto &[key, value] : element.items())
                {
                    if (key == "directional_light")
                    {
                        glm::vec3 direction(
                            value["direction"]["x"],
                            value["direction"]["y"],
                            value["direction"]["z"]);
                        glm::vec3 color(
                            value["color"]["r"],
                            value["color"]["g"],
                            value["color"]["b"]);
                        scene->registry.emplace<liminal::directional_light>(entity, direction, color, 4096);
                    }

                    if (key == "transform")
                    {
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
                        float mass = value["mass"];
                        auto transform = scene->registry.emplace<liminal::transform>(entity, nullptr, position, rotation, scale, mass);

                        if (mass > 0.0f)
                        {
                            scene->world->addRigidBody(transform.rigidbody);
                        }
                    }

                    if (key == "mesh_renderer")
                    {
                        std::string filename(value["filename"]);
                        bool flip_uvs = value["flip_uvs"];
                        scene->registry.emplace<liminal::mesh_renderer>(entity, new liminal::model(filename, flip_uvs));
                    }

                    if (key == "script")
                    {
                        scene->registry.emplace<liminal::script>(entity, value["filename"], entity);
                    }

                    if (key == "water")
                    {
                        glm::vec3 position(
                            value["position"]["x"],
                            value["position"]["y"],
                            value["position"]["z"]);
                        scene->registry.emplace<liminal::water>(entity, position, value["size"]);
                    }
                }
            }
        }
    }
}

liminal::scene::scene(const std::string &filename, liminal::engine *engine)
{
    btDefaultCollisionConfiguration *collision_configuration = new btDefaultCollisionConfiguration();
    btDispatcher *dispatcher = new btCollisionDispatcher(collision_configuration);
    btBroadphaseInterface *pair_cache = new btDbvtBroadphase();
    btConstraintSolver *constraint_solver = new btSequentialImpulseConstraintSolver();
    world = new btDiscreteDynamicsWorld(dispatcher, pair_cache, constraint_solver, collision_configuration);
    world->setGravity(btVector3(0.0f, -9.8f, 0.0f));

    load_scene(filename, engine, this);
}

liminal::scene::~scene()
{
    registry.clear();

    delete world;
}
