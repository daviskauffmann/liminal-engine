#include "block.hpp"
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
    class app : public liminal::core::app
    {
    public:
        app(int argc, char *argv[])
            : liminal::core::app(argc, argv)
        {
            sdl->set_relative_mouse_mode(true);

            scene = std::make_shared<liminal::entities::scene>(assets);
            scene->start();
            scene->skybox = assets->load_skybox("minecraft/data/white.png");

            player_entity = scene->create_entity();
            player_transform = &player_entity.add_component<liminal::components::transform>("Player");
            player_camera = &player_entity.add_component<liminal::components::camera>(45.0f);

            world = std::make_unique<minecraft::world>(scene, assets);
        }

        ~app() override
        {
            scene->stop();
        }

        void update(const std::uint64_t current_time, const float delta_time) override
        {
            const auto &io = ImGui::GetIO();

            if (liminal::input::input::key_down(liminal::input::keycode::TAB))
            {
                sdl->set_relative_mouse_mode(!sdl->get_relative_mouse_mode());
            }

            const auto camera_front = player_camera->calc_front(*player_transform);
            const auto camera_right = player_camera->calc_right(*player_transform);

            static glm::vec3 velocity(0, 0, 0);
            glm::vec3 acceleration(0, 0, 0);
            constexpr auto speed = 50.0f;
            constexpr auto drag = 10.0f;
            auto sprint = false;
            if (!io.WantCaptureKeyboard)
            {
                if (liminal::input::input::key(liminal::input::keycode::W))
                {
                    acceleration += camera_front;
                }
                if (liminal::input::input::key(liminal::input::keycode::A))
                {
                    acceleration -= camera_right;
                }
                if (liminal::input::input::key(liminal::input::keycode::S))
                {
                    acceleration -= camera_front;
                }
                if (liminal::input::input::key(liminal::input::keycode::D))
                {
                    acceleration += camera_right;
                }
                if (liminal::input::input::key(liminal::input::keycode::SPACE))
                {
                    acceleration.y = 1;
                }
                if (liminal::input::input::key(liminal::input::keycode::LCTRL))
                {
                    acceleration.y = -1;
                }
                if (liminal::input::input::key(liminal::input::keycode::LSHIFT))
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
            player_transform->position = 0.5f * acceleration * powf(delta_time, 2) + velocity * delta_time + player_transform->position;
            velocity = acceleration * delta_time + velocity;
            player_transform->rotation.z = glm::dot(camera_right, velocity);

            if (!io.WantCaptureMouse)
            {
                if (sdl->get_relative_mouse_mode())
                {
                    constexpr auto sensitivity = 0.5f;
                    player_transform->rotation.x -= liminal::input::input::mouse_dy * sensitivity;
                    player_transform->rotation.y += liminal::input::input::mouse_dx * sensitivity;
                    if (player_transform->rotation.x > 89)
                    {
                        player_transform->rotation.x = 89;
                    }
                    if (player_transform->rotation.x < -89)
                    {
                        player_transform->rotation.x = -89;
                    }

                    player_camera->fov -= liminal::input::input::mouse_wheel_y;
                    if (player_camera->fov <= 1)
                    {
                        player_camera->fov = 1;
                    }
                    if (player_camera->fov >= 120)
                    {
                        player_camera->fov = 120;
                    }
                }

                if (liminal::input::input::mouse_button(liminal::input::mouse_button::LEFT))
                {
                    for (std::size_t i = 0; i < 100; i++)
                    {
                        const auto x = static_cast<int>(glm::round(player_transform->position.x + (player_camera->calc_front(*player_transform).x * (i * 0.1f))));
                        const auto y = static_cast<int>(glm::round(player_transform->position.y + (player_camera->calc_front(*player_transform).y * (i * 0.1f))));
                        const auto z = static_cast<int>(glm::round(player_transform->position.z + (player_camera->calc_front(*player_transform).z * (i * 0.1f))));
                        world->set_block(x, y, z, minecraft::block_type::air);
                    }
                }
            }

            world->update();
            scene->update(current_time, delta_time);
            renderer->render(*scene, *player_camera, *player_transform, current_time, delta_time, false);
        }

    private:
        std::shared_ptr<liminal::entities::scene> scene;

        liminal::entities::entity player_entity;
        liminal::components::transform *player_transform;
        liminal::components::camera *player_camera;

        std::unique_ptr<minecraft::world> world;
    };
}

liminal::core::app *liminal::core::create_app(int argc, char *argv[])
{
    return new minecraft::app(argc, argv);
}
