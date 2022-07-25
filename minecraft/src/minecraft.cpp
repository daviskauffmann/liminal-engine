#include "block.hpp"
#include "blocks/air_block.hpp"
#include "blocks/grass_block.hpp"
#include "chunk.hpp"
#include "mesh_data.hpp"
#include "world.hpp"
#include <glm/glm.hpp>
#include <imgui.h>
#include <iostream>
#include <liminal/liminal.hpp>
#include <liminal/main.hpp>
#include <unordered_map>

namespace minecraft
{
    class app : public liminal::app
    {
    public:
        app(int argc, char *argv[])
            : liminal::app(argc, argv)
        {
            liminal::platform::instance->set_relative_mouse_mode(true);

            scene = new liminal::scene();
            scene->start();
            scene->skybox = new liminal::skybox("assets/images/GCanyon_C_YumaPoint_Env.hdr");

            auto sun = scene->create_entity();
            sun.add_component<liminal::transform>(
                "Sun",
                nullptr,
                glm::vec3(0, 0, 0),
                glm::vec3(0.352286f, -0.547564f, -0.758992f),
                glm::vec3(1, 1, 1));
            sun.add_component<liminal::directional_light>(glm::vec3(1, 1, 1));

            player_entity = scene->create_entity();
            player_entity.add_component<liminal::transform>("Player");
            player_entity.add_component<liminal::camera>(45.f);

            world = new minecraft::world(scene);
        }

        ~app()
        {
            delete world;

            if (scene)
            {
                scene->stop();
                delete scene;
            }
        }

        void update(unsigned int current_time, float delta_time) override
        {
            auto &io = ImGui::GetIO();

            if (liminal::input::key_down(liminal::KEYCODE_TAB))
            {
                liminal::platform::instance->set_relative_mouse_mode(!liminal::platform::instance->get_relative_mouse_mode());
            }

            auto &transform = player_entity.get_component<liminal::transform>();
            auto &camera = player_entity.get_component<liminal::camera>();
            glm::vec3 camera_front = camera.calc_front(transform);
            glm::vec3 camera_right = camera.calc_right(transform);

            static glm::vec3 velocity(0, 0, 0);
            glm::vec3 acceleration(0, 0, 0);
            const float speed = 50;
            const float drag = 10;
            bool sprint = false;
            if (!io.WantCaptureKeyboard)
            {
                if (liminal::input::key(liminal::keycode::KEYCODE_W))
                {
                    acceleration += camera_front;
                }
                if (liminal::input::key(liminal::keycode::KEYCODE_A))
                {
                    acceleration -= camera_right;
                }
                if (liminal::input::key(liminal::keycode::KEYCODE_S))
                {
                    acceleration -= camera_front;
                }
                if (liminal::input::key(liminal::keycode::KEYCODE_D))
                {
                    acceleration += camera_right;
                }
                if (liminal::input::key(liminal::keycode::KEYCODE_SPACE))
                {
                    acceleration.y = 1;
                }
                if (liminal::input::key(liminal::keycode::KEYCODE_LCTRL))
                {
                    acceleration.y = -1;
                }
                if (liminal::input::key(liminal::keycode::KEYCODE_LSHIFT))
                {
                    sprint = true;
                }
            }
            if (glm::length(acceleration) > 1)
            {
                acceleration = glm::normalize(acceleration);
            }
            acceleration *= speed * (sprint ? 2 : 1);
            acceleration -= velocity * drag;
            transform.position = 0.5f * acceleration * powf(delta_time, 2) + velocity * delta_time + transform.position;
            velocity = acceleration * delta_time + velocity;
            transform.rotation.z = glm::dot(camera_right, velocity);

            if (!io.WantCaptureMouse)
            {
                if (liminal::platform::instance->get_relative_mouse_mode())
                {
                    const auto sensitivity = 0.1f;
                    transform.rotation.x -= liminal::input::mouse_dy * sensitivity;
                    transform.rotation.y += liminal::input::mouse_dx * sensitivity;
                    if (transform.rotation.x > 89)
                    {
                        transform.rotation.x = 89;
                    }
                    if (transform.rotation.x < -89)
                    {
                        transform.rotation.x = -89;
                    }

                    camera.fov -= liminal::input::mouse_wheel_y;
                    if (camera.fov <= 1)
                    {
                        camera.fov = 1;
                    }
                    if (camera.fov >= 120)
                    {
                        camera.fov = 120;
                    }
                }
            }

            scene->update(current_time, delta_time);
            liminal::renderer::instance->render(*scene, current_time, delta_time);
        }

    private:
        liminal::scene *scene = nullptr;

        liminal::entity player_entity;

        minecraft::world *world;
    };
}

liminal::app *liminal::create_app(int argc, char *argv[])
{
    return new minecraft::app(argc, argv);
}
