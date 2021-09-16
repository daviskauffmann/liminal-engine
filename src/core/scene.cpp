#include "scene.hpp"

#include <fstream>
#include <nlohmann/json.hpp>

#include "../audio/sound.hpp"
#include "../components/audio_source.hpp"
#include "../components/directional_light.hpp"
#include "../components/mesh_renderer.hpp"
#include "../components/point_light.hpp"
#include "../components/script.hpp"
#include "../components/spot_light.hpp"
#include "../components/terrain.hpp"
#include "../components/transform.hpp"
#include "../components/water.hpp"
#include "../core/platform.hpp"
#include "../graphics/camera.hpp"
#include "../graphics/model.hpp"
#include "../graphics/skybox.hpp"

liminal::scene::scene(const std::string &filename)
{
    camera = nullptr;
    skybox = nullptr;

    // setup physics world
    btDefaultCollisionConfiguration *collision_configuration = new btDefaultCollisionConfiguration();
    btDispatcher *dispatcher = new btCollisionDispatcher(collision_configuration);
    btBroadphaseInterface *pair_cache = new btDbvtBroadphase();
    btConstraintSolver *constraint_solver = new btSequentialImpulseConstraintSolver();
    world = new btDiscreteDynamicsWorld(dispatcher, pair_cache, constraint_solver, collision_configuration);
    world->setGravity(btVector3(0.0f, -9.8f, 0.0f));

    // load scene from file
    std::ifstream stream(filename);
    nlohmann::json json;
    stream >> json;
    for (auto &[key, value] : json.items())
    {
        if (key == "skybox")
        {
            skybox = new liminal::skybox(json["skybox"]);
        }

        if (key == "entities")
        {
            for (auto &element : value)
            {
                auto entity = registry.create();

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
                        registry.emplace<liminal::directional_light>(entity, direction, color, 4096);
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
                        auto transform = registry.emplace<liminal::transform>(entity, nullptr, position, rotation, scale, mass);

                        if (mass > 0.0f)
                        {
                            world->addRigidBody(transform.rigidbody);
                        }
                    }

                    if (key == "mesh_renderer")
                    {
                        std::string filename(value["filename"]);
                        bool flip_uvs = value["flip_uvs"];
                        registry.emplace<liminal::mesh_renderer>(entity, new liminal::model(filename, flip_uvs));
                    }

                    if (key == "script")
                    {
                        registry.emplace<liminal::script>(entity, value["filename"], this, entity);
                    }

                    if (key == "water")
                    {
                        glm::vec3 position(
                            value["position"]["x"],
                            value["position"]["y"],
                            value["position"]["z"]);
                        registry.emplace<liminal::water>(entity, position, value["size"]);
                    }

                    if (key == "terrain")
                    {
                        auto terrain = registry.emplace<liminal::terrain>(entity, "assets/images/heightmap.png", glm::vec3(0.0f, 0.0f, 0.0f), 100.0f, 5.0f);
                        world->addRigidBody(terrain.rigidbody);
                    }
                }
            }
        }
    }

    // run init scripts
    for (auto [entity, script] : registry.view<liminal::script>().each())
    {
        script.init();
    }
}

liminal::scene::~scene()
{
    registry.clear();

    delete world;
}
