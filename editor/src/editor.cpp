#include <entt/entt.hpp>
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
#include <liminal/graphics/texture.hpp>
#include <liminal/input/input.hpp>

// TODO: this will be an editor eventually, currently its more like a demo level

class editor : public liminal::app
{
public:
    entt::entity camera_entity;

    const float flashlight_intensity = 20.0f;
    entt::entity flashlight_entity;
    bool flashlight_follow = true;
    bool flashlight_on = true;

    liminal::sound *ambient_sound;
    entt::entity ambience_entity;

    liminal::sound *bounce_sound;
    entt::entity bounce_entity;

    liminal::sound *shoot_sound;
    entt::entity weapon_entity;

    liminal::texture *grass_texture;
    entt::entity ui_entity;

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
        camera_entity = scene->registry.create();
        scene->registry.emplace<liminal::transform>(camera_entity, "Camera", nullptr, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        scene->registry.emplace<liminal::audio_listener>(camera_entity, glm::vec3(0.0f, 0.0f, 0.0f));

        // TODO: these entities should come from the JSON file
        flashlight_entity = scene->registry.create();
        scene->registry.emplace<liminal::transform>(flashlight_entity, "Flashlight", nullptr, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        scene->registry.emplace<liminal::spot_light>(flashlight_entity, glm::vec3(1.0f, 1.0f, 1.0f) * flashlight_intensity, cosf(glm::radians(12.5f)), cosf(glm::radians(15.0f)));

        ambience_entity = scene->registry.create();
        scene->registry.emplace<liminal::transform>(ambience_entity, "Ambience", nullptr, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        scene->registry.emplace<liminal::audio_source>(ambience_entity, new liminal::source());
        scene->registry.get<liminal::audio_source>(ambience_entity).source->set_loop(true);
        scene->registry.get<liminal::audio_source>(ambience_entity).source->set_gain(0.25f);
        // scene->registry.get<liminal::audio_source>(ambience_entity).play(ambient_sound);

        bounce_entity = scene->registry.create();
        scene->registry.emplace<liminal::transform>(bounce_entity, "Bounce sound", nullptr, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        scene->registry.emplace<liminal::audio_source>(bounce_entity, new liminal::source());

        weapon_entity = scene->registry.create();
        scene->registry.emplace<liminal::transform>(weapon_entity, "Weapon", nullptr, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        scene->registry.emplace<liminal::audio_source>(weapon_entity, new liminal::source());

        ui_entity = scene->registry.create();
        // scene->registry.emplace<liminal::sprite>(ui_entity, &grass_texture, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f), 0.0f, glm::vec2(1.0f, 1.0f));}
    }

    ~editor()
    {
    }

    void update(unsigned int current_time, float delta_time) override
    {
        ImGuiIO &io = ImGui::GetIO();

        auto camera = liminal::engine::get_instance().renderer->camera;
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
        scene->registry.get<liminal::transform>(camera_entity).position = camera->position;
        scene->registry.get<liminal::transform>(camera_entity).rotation = camera->calc_front();

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
            scene->registry.get<liminal::spot_light>(flashlight_entity).color = glm::vec3(1.0f, 1.0f, 1.0f) * flashlight_intensity;
        }
        else
        {
            scene->registry.get<liminal::spot_light>(flashlight_entity).color = glm::vec3(0.0f, 0.0f, 0.0f);
        }
        if (flashlight_follow)
        {
            scene->registry.get<liminal::transform>(flashlight_entity).position = camera->position;
            scene->registry.get<liminal::transform>(flashlight_entity).rotation = glm::mix(
                scene->registry.get<liminal::transform>(flashlight_entity).rotation,
                camera_front,
                30.0f * delta_time);
        }

        scene->registry.get<liminal::transform>(ambience_entity).position = camera->position;
        scene->registry.get<liminal::transform>(weapon_entity).position = camera->position;

        if (!io.WantCaptureMouse)
        {
            if (liminal::input::mouse_button(liminal::MOUSE_BUTTON_LEFT))
            {
                if (!scene->registry.get<liminal::audio_source>(weapon_entity).source->is_playing())
                {
                    scene->registry.get<liminal::audio_source>(weapon_entity).source->play(*shoot_sound);
                }
            }

            if (liminal::input::mouse_button(liminal::MOUSE_BUTTON_RIGHT))
            {
                if (!scene->registry.get<liminal::audio_source>(bounce_entity).source->is_playing())
                {
                    scene->registry.get<liminal::audio_source>(bounce_entity).source->play(*bounce_sound);
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
