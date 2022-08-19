#include "block.hpp"
#include "blocks/air_block.hpp"
#include "blocks/grass_block.hpp"
#include "chunk.hpp"
#include "mesh_data.hpp"
#include "world.hpp"
#include <glm/glm.hpp>
#include <imgui.h>
#include <liminal/liminal.hpp>
#include <liminal/main.hpp>
#include <memory>
#include <unordered_map>

namespace minecraft
{
    class app : public liminal::app
    {
    public:
        app(int argc, char *argv[])
            : liminal::app(argc, argv)
        {
            sdl->set_relative_mouse_mode(true);

            scene = std::make_shared<liminal::scene>(assets);
            scene->start();
            scene->skybox = std::make_shared<liminal::skybox>("assets/images/white.png");

            player_entity = scene->create_entity();
            player_entity.add_component<liminal::transform>("Player");
            player_entity.add_component<liminal::camera>(45.0f);

            world = std::make_unique<minecraft::world>(scene, assets);
        }

        ~app() override
        {
            scene->stop();
        }

        void update(const std::uint64_t current_time, const float delta_time) override
        {
            const auto &io = ImGui::GetIO();

            if (liminal::input::key_down(liminal::keycode::TAB))
            {
                sdl->set_relative_mouse_mode(!sdl->get_relative_mouse_mode());
            }

            auto &transform = player_entity.get_component<liminal::transform>();
            auto &camera = player_entity.get_component<liminal::camera>();
            glm::vec3 camera_front = camera.calc_front(transform);
            glm::vec3 camera_right = camera.calc_right(transform);

            static auto velocity = glm::vec3(0, 0, 0);
            auto acceleration = glm::vec3(0, 0, 0);
            constexpr auto speed = 50.0f;
            constexpr auto drag = 10.0f;
            bool sprint = false;
            if (!io.WantCaptureKeyboard)
            {
                if (liminal::input::key(liminal::keycode::W))
                {
                    acceleration += camera_front;
                }
                if (liminal::input::key(liminal::keycode::A))
                {
                    acceleration -= camera_right;
                }
                if (liminal::input::key(liminal::keycode::S))
                {
                    acceleration -= camera_front;
                }
                if (liminal::input::key(liminal::keycode::D))
                {
                    acceleration += camera_right;
                }
                if (liminal::input::key(liminal::keycode::SPACE))
                {
                    acceleration.y = 1;
                }
                if (liminal::input::key(liminal::keycode::LCTRL))
                {
                    acceleration.y = -1;
                }
                if (liminal::input::key(liminal::keycode::LSHIFT))
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
                if (sdl->get_relative_mouse_mode())
                {
                    constexpr auto sensitivity = 0.1f;
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

                if (liminal::input::mouse_button(liminal::mouse_button::LEFT))
                {
                    for (std::size_t i = 0; i < 100; i++)
                    {
                        const auto x = static_cast<int>(glm::round(transform.position.x + (camera.calc_front(transform).x * (i * 0.1f))));
                        const auto y = static_cast<int>(glm::round(transform.position.y + (camera.calc_front(transform).y * (i * 0.1f))));
                        const auto z = static_cast<int>(glm::round(transform.position.z + (camera.calc_front(transform).z * (i * 0.1f))));
                        world->set_block(x, y, z, new minecraft::air_block());
                    }
                }
            }

            world->update();
            scene->update(current_time, delta_time);
            renderer->render(*scene, current_time, delta_time);
        }

    private:
        std::shared_ptr<liminal::scene> scene;

        liminal::entity player_entity;

        std::unique_ptr<minecraft::world> world;
    };
}

liminal::app *liminal::create_app(int argc, char *argv[])
{
    return new minecraft::app(argc, argv);
}
