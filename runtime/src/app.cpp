#include <glm/glm.hpp>
#include <imgui.h>
#include <liminal/liminal.hpp>
#include <liminal/main.hpp>
#include <memory>

namespace player
{
    class app : public liminal::app
    {
    public:
        app(int argc, char *argv[])
            : liminal::app(argc, argv)
        {
            sdl->set_relative_mouse_mode(true);

            scene = std::make_unique<liminal::scene>(assets);
            scene->load("editor/data/scenes/demo.json");
            scene->start();

            // ambient_sound = assets->load_sound("engine/data/audio/ambient.wav");
            // bounce_sound = assets->load_sound("engine/data/audio/bounce.wav");
            // shoot_sound = assets->load_sound("engine/data/audio/shoot.wav");
            // grass_texture = assets->load_texture("engine/data/images/grass_sprite.png");

            // TODO: these entities should come from the JSON file
            player_entity = scene->create_entity();
            player_entity.add_component<liminal::transform>("Player", nullptr, glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
            player_entity.add_component<liminal::camera>(45.0f);
            player_entity.add_component<liminal::audio_listener>();
            // player_entity.add_component<liminal::physical>();

            // ambience_entity = scene->create_entity();
            // ambience_entity.add_component<liminal::transform>("Ambience", nullptr, glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
            // auto &ambience_audio_source = ambience_entity.add_component<liminal::audio_source>();
            // ambience_audio_source.source->set_loop(true);
            // ambience_audio_source.source->set_gain(0.25f);
            // ambience_audio_source.source->play(*ambient_sound);

            // bounce_entity = scene->create_entity();
            // bounce_entity.add_component<liminal::transform>("Bounce sound", nullptr, glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
            // bounce_entity.add_component<liminal::audio_source>();

            // weapon_entity = scene->create_entity();
            // weapon_entity.add_component<liminal::transform>("Weapon", nullptr, glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
            // weapon_entity.add_component<liminal::audio_source>();

            // ui_entity = scene->create_entity();
            // ui_entity.add_component<liminal::sprite>(grass_texture.get(), glm::vec3(1, 1, 1), glm::vec2(0, 0), 0.0f, glm::vec2(1, 1));
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
            auto camera_front = camera.calc_front(transform);
            auto camera_right = camera.calc_right(transform);

            static glm::vec3 velocity(0, 0, 0);
            glm::vec3 acceleration(0, 0, 0);
            constexpr auto speed = 50.0f;
            constexpr auto drag = 10.0f;
            auto sprint = false;
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
                    if (transform.rotation.y < 0)
                    {
                        transform.rotation.y = 360;
                    }
                    if (transform.rotation.y > 360)
                    {
                        transform.rotation.y = 0;
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

            // ambience_entity.get_component<liminal::transform>().position = transform.position;
            // weapon_entity.get_component<liminal::transform>().position = transform.position;

            // if (!io.WantCaptureMouse)
            // {
            //     if (liminal::input::mouse_button(liminal::mouse_button::LEFT))
            //     {
            //         auto &weapon_audio_source = weapon_entity.get_component<liminal::audio_source>();
            //         if (!weapon_audio_source.source->is_playing())
            //         {
            //             weapon_audio_source.source->play(*shoot_sound);
            //         }
            //     }

            //     if (liminal::input::mouse_button(liminal::mouse_button::RIGHT))
            //     {
            //         auto &bounce_audio_source = bounce_entity.get_component<liminal::audio_source>();
            //         if (!bounce_audio_source.source->is_playing())
            //         {
            //             bounce_audio_source.source->play(*bounce_sound);
            //         }
            //     }
            // }

            // update scene
            scene->update(current_time, delta_time);

            // render scene
            renderer->render(*scene, current_time, delta_time);
        }

    private:
        std::unique_ptr<liminal::scene> scene;

        liminal::entity player_entity;

        // std::shared_ptr<liminal::sound> ambient_sound;
        // liminal::entity ambience_entity;

        // std::shared_ptr<liminal::sound> bounce_sound;
        // liminal::entity bounce_entity;

        // std::shared_ptr<liminal::sound> shoot_sound;
        // liminal::entity weapon_entity;

        // std::shared_ptr<liminal::texture> grass_texture;
        // liminal::entity ui_entity;
    };
}

liminal::app *liminal::create_app(int argc, char *argv[])
{
    return new player::app(argc, argv);
}
