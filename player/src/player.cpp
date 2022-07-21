#include <glm/glm.hpp>
#include <imgui.h>
#include <iostream>
#include <liminal/liminal.hpp>
#include <liminal/main.hpp>

class player : public liminal::app
{
public:
    liminal::entity player_entity;

    liminal::sound *ambient_sound;
    liminal::entity ambience_entity;

    liminal::sound *bounce_sound;
    liminal::entity bounce_entity;

    liminal::sound *shoot_sound;
    liminal::entity weapon_entity;

    liminal::texture *grass_texture;
    liminal::entity ui_entity;

    bool noclip = false;

    player()
    {
        liminal::platform::instance->set_relative_mouse_mode(true);

        // load assets
        // TODO: asset management
        ambient_sound = new liminal::sound("assets/audio/ambient.wav");
        bounce_sound = new liminal::sound("assets/audio/bounce.wav");
        shoot_sound = new liminal::sound("assets/audio/shoot.wav");
        grass_texture = new liminal::texture("assets/images/grass_sprite.png");

        scene = new liminal::scene();
        scene->load("assets/scenes/demo.json");

        // run init scripts
        for (auto [id, script] : scene->get_entities_with<liminal::script>().each())
        {
            script.init();
        }

        player_entity = scene->create_entity();
        player_entity.add_component<liminal::transform>("Player", nullptr, glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
        player_entity.add_component<liminal::camera>(45.f);
        player_entity.add_component<liminal::audio_listener>();

        // TODO: these entities should come from the JSON file
        ambience_entity = scene->create_entity();
        ambience_entity.add_component<liminal::transform>("Ambience", nullptr, glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
        auto &ambience_audio_source = ambience_entity.add_component<liminal::audio_source>();
        ambience_audio_source.set_loop(true);
        ambience_audio_source.set_gain(0.25f);
        ambience_audio_source.play(*ambient_sound);

        bounce_entity = scene->create_entity();
        bounce_entity.add_component<liminal::transform>("Bounce sound", nullptr, glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
        bounce_entity.add_component<liminal::audio_source>();

        weapon_entity = scene->create_entity();
        weapon_entity.add_component<liminal::transform>("Weapon", nullptr, glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
        weapon_entity.add_component<liminal::audio_source>();

        ui_entity = scene->create_entity();
        // ui_entity.add_component<liminal::sprite>(grass_texture, glm::vec3(1, 1, 1), glm::vec2(0, 0), 0.f, glm::vec2(1, 1));
    }

    ~player()
    {
        // TODO: asset management, this is currently generating OpenAL errors for deleting in-use buffers
        delete ambient_sound;
        delete bounce_sound;
        delete shoot_sound;
        delete grass_texture;
    }

    void update(unsigned int current_time, float delta_time) override
    {
        ImGuiIO &io = ImGui::GetIO();

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
        bool jumping = false;
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
            if (liminal::input::key(liminal::keycode::KEYCODE_SPACE) && noclip)
            {
                acceleration.y = 1;
            }
            if (liminal::input::key(liminal::keycode::KEYCODE_LCTRL) && noclip)
            {
                acceleration.y = -1;
            }
            if (liminal::input::key(liminal::keycode::KEYCODE_LSHIFT))
            {
                sprint = true;
            }
            if (liminal::input::key_down(liminal::keycode::KEYCODE_SPACE) && !jumping && !noclip)
            {
                velocity.y = 10;
                jumping = true;
            }
            if (liminal::input::key_down(liminal::keycode::KEYCODE_V))
            {
                noclip = !noclip;
            }
        }
        float acceleration_length = glm::length(acceleration);
        if (acceleration_length > 1)
        {
            acceleration /= acceleration_length;
        }
        acceleration *= speed * (sprint ? 2 : 1);
        acceleration -= velocity * drag;
        if (!noclip)
        {
            acceleration.y = -9.8f;
        }
        transform.position = 0.5f * acceleration * powf(delta_time, 2) + velocity * delta_time + transform.position;
        if (transform.position.y < 0 && !noclip)
        {
            transform.position.y = 0;
            jumping = false;
        }
        velocity = acceleration * delta_time + velocity;
        transform.rotation.z = glm::dot(camera_right, velocity);

        if (!io.WantCaptureMouse)
        {
            if (liminal::platform::instance->get_relative_mouse_mode())
            {
                const float sensitivity = 0.1f;
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

        ambience_entity.get_component<liminal::transform>().position = transform.position;
        weapon_entity.get_component<liminal::transform>().position = transform.position;

        if (!io.WantCaptureMouse)
        {
            if (liminal::input::mouse_button(liminal::MOUSE_BUTTON_LEFT))
            {
                auto &weapon_audio_source = weapon_entity.get_component<liminal::audio_source>();
                if (!weapon_audio_source.is_playing())
                {
                    weapon_audio_source.play(*shoot_sound);
                }
            }

            if (liminal::input::mouse_button(liminal::MOUSE_BUTTON_RIGHT))
            {
                auto &bounce_audio_source = bounce_entity.get_component<liminal::audio_source>();
                if (!bounce_audio_source.is_playing())
                {
                    bounce_audio_source.play(*bounce_sound);
                }
            }
        }

        // update scene
        scene->update(current_time, delta_time);
    }
};

liminal::app *liminal::create_app(int argc, char *argv[])
{
    return new player();
}
