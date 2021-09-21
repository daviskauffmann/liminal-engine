#include <glm/glm.hpp>
#include <imgui.h>
#include <iostream>
#include <liminal/main.hpp>
#include <liminal/audio/sound.hpp>
#include <liminal/audio/source.hpp>
#include <liminal/components/audio_listener.hpp>
#include <liminal/components/audio_source.hpp>
#include <liminal/components/mesh_renderer.hpp>
#include <liminal/components/transform.hpp>
#include <liminal/components/script.hpp>
#include <liminal/components/spot_light.hpp>
#include <liminal/core/app.hpp>
#include <liminal/core/entity.hpp>
#include <liminal/graphics/camera.hpp>
#include <liminal/graphics/texture.hpp>
#include <liminal/input/input.hpp>

// TODO: this will be an editor eventually, currently its more like a demo level

class editor : public liminal::app
{
public:
    liminal::entity camera_entity;

    const float flashlight_intensity = 20.0f;
    liminal::entity flashlight_entity;
    bool flashlight_follow = true;
    bool flashlight_on = true;

    liminal::sound *ambient_sound;
    liminal::entity ambience_entity;

    liminal::sound *bounce_sound;
    liminal::entity bounce_entity;

    liminal::sound *shoot_sound;
    liminal::entity weapon_entity;

    liminal::texture *grass_texture;
    liminal::entity ui_entity;

    editor()
    {
        // load assets
        // TODO: asset management
        ambient_sound = new liminal::sound("assets/audio/ambient.wav");
        bounce_sound = new liminal::sound("assets/audio/bounce.wav");
        shoot_sound = new liminal::sound("assets/audio/shoot.wav");
        grass_texture = new liminal::texture("assets/images/grass_sprite.png");

        scene = new liminal::scene("assets/scenes/demo.json");

        // TODO: camera should be a component maybe
        camera_entity = scene->create_entity();
        camera_entity.add_component<liminal::transform>("Camera", nullptr, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        camera_entity.add_component<liminal::audio_listener>();

        // TODO: these entities should come from the JSON file
        flashlight_entity = scene->create_entity();
        flashlight_entity.add_component<liminal::transform>("Flashlight", nullptr, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        flashlight_entity.add_component<liminal::spot_light>(glm::vec3(1.0f, 1.0f, 1.0f) * flashlight_intensity, cosf(glm::radians(12.5f)), cosf(glm::radians(15.0f)));

        ambience_entity = scene->create_entity();
        ambience_entity.add_component<liminal::transform>("Ambience", nullptr, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        ambience_entity.add_component<liminal::audio_source>(new liminal::source());
        ambience_entity.get_component<liminal::audio_source>().source->set_loop(true);
        ambience_entity.get_component<liminal::audio_source>().source->set_gain(0.25f);
        // ambience_entity.get_component<liminal::audio_source>().play(ambient_sound);

        bounce_entity = scene->create_entity();
        bounce_entity.add_component<liminal::transform>("Bounce sound", nullptr, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        bounce_entity.add_component<liminal::audio_source>(new liminal::source());

        weapon_entity = scene->create_entity();
        weapon_entity.add_component<liminal::transform>("Weapon", nullptr, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        weapon_entity.add_component<liminal::audio_source>(new liminal::source());

        ui_entity = scene->create_entity();
        // ui_entity.add_component<liminal::sprite>(&grass_texture, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f), 0.0f, glm::vec2(1.0f, 1.0f));}
    }

    ~editor()
    {
    }

    void update(unsigned int current_time, float delta_time) override
    {
        ImGuiIO &io = ImGui::GetIO();

        auto camera = scene->camera;
        glm::vec3 camera_front = camera->calc_front();
        glm::vec3 camera_right = camera->calc_right();

        static glm::vec3 velocity(0.0f, 0.0f, 0.0f);
        glm::vec3 acceleration(0.0f, 0.0f, 0.0f);
        const float speed = 50.0f;
        const float drag = 10.0f;
        bool sprint = false;
        if (!io.WantCaptureKeyboard)
        {
            if (liminal::input::key(liminal::KEYCODE_W))
            {
                acceleration += camera_front;
            }
            if (liminal::input::key(liminal::KEYCODE_A))
            {
                acceleration -= camera_right;
            }
            if (liminal::input::key(liminal::KEYCODE_S))
            {
                acceleration -= camera_front;
            }
            if (liminal::input::key(liminal::KEYCODE_D))
            {
                acceleration += camera_right;
            }
            if (liminal::input::key(liminal::KEYCODE_SPACE))
            {
                acceleration.y = 1.0f;
            }
            if (liminal::input::key(liminal::KEYCODE_LCTRL))
            {
                acceleration.y = -1.0f;
            }
            if (liminal::input::key(liminal::KEYCODE_LSHIFT))
            {
                sprint = true;
            }
        }
        float acceleration_length = glm::length(acceleration);
        if (acceleration_length > 1.0f)
        {
            acceleration /= acceleration_length;
        }
        acceleration *= speed * (sprint ? 2.0f : 1.0f);
        acceleration -= velocity * drag;
        camera->position = 0.5f * acceleration * powf(delta_time, 2.0f) + velocity * delta_time + camera->position;
        velocity = acceleration * delta_time + velocity;
        // camera->pitch = -glm::dot(camera_front, velocity);
        camera->roll = glm::dot(camera_right, velocity);

        if (!io.WantCaptureMouse)
        {
            if (SDL_GetRelativeMouseMode())
            {
                const float sensitivity = 0.1f;
                camera->pitch -= liminal::input::mouse_dy * sensitivity;
                camera->yaw += liminal::input::mouse_dx * sensitivity;
                if (camera->pitch > 89.0f)
                {
                    camera->pitch = 89.0f;
                }
                if (camera->pitch < -89.0f)
                {
                    camera->pitch = -89.0f;
                }

                camera->fov -= liminal::input::mouse_wheel_y;
                if (camera->fov <= 1.0f)
                {
                    camera->fov = 1.0f;
                }
                if (camera->fov >= 120.0f)
                {
                    camera->fov = 120.0f;
                }
            }
        }

        // TODO: camera itself should be a component attached to the camera_entity
        // and its transform would be changed directly, making this step unnecessary
        camera_entity.get_component<liminal::transform>().position = camera->position;
        camera_entity.get_component<liminal::transform>().rotation = camera->calc_front();

        if (liminal::input::key_down(liminal::KEYCODE_F))
        {
            flashlight_on = !flashlight_on;
        }
        if (liminal::input::key_down(liminal::KEYCODE_G))
        {
            flashlight_follow = !flashlight_follow;
        }

        if (flashlight_on)
        {
            flashlight_entity.get_component<liminal::spot_light>().color = glm::vec3(1.0f, 1.0f, 1.0f) * flashlight_intensity;
        }
        else
        {
            flashlight_entity.get_component<liminal::spot_light>().color = glm::vec3(0.0f, 0.0f, 0.0f);
        }
        if (flashlight_follow)
        {
            flashlight_entity.get_component<liminal::transform>().position = camera->position;
            flashlight_entity.get_component<liminal::transform>().rotation = glm::mix(
                flashlight_entity.get_component<liminal::transform>().rotation,
                camera_front,
                30.0f * delta_time);
        }

        ambience_entity.get_component<liminal::transform>().position = camera->position;
        weapon_entity.get_component<liminal::transform>().position = camera->position;

        if (!io.WantCaptureMouse)
        {
            if (liminal::input::mouse_button(liminal::MOUSE_BUTTON_LEFT))
            {
                if (!weapon_entity.get_component<liminal::audio_source>().source->is_playing())
                {
                    weapon_entity.get_component<liminal::audio_source>().source->play(*shoot_sound);
                }
            }

            if (liminal::input::mouse_button(liminal::MOUSE_BUTTON_RIGHT))
            {
                if (!bounce_entity.get_component<liminal::audio_source>().source->is_playing())
                {
                    bounce_entity.get_component<liminal::audio_source>().source->play(*bounce_sound);
                }
            }
        }

        // ImGui::Begin("Scene Hierarchy");

        // for (auto [entity, transform] : scene->registry.view<liminal::transform>().each())
        // {
        //     ImGui::Text("%s", transform.name);
        // }

        // ImGui::End();
    }
};

liminal::app *liminal::create_app(int argc, char *argv[])
{
    return new editor();
}
