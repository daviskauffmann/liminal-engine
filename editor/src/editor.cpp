#include <glm/glm.hpp>
#include <imgui.h>
#include <iostream>
#include <liminal/liminal.hpp>
#include <liminal/main.hpp>
#include <SDL2/SDL.h>

class editor : public liminal::app
{
public:
    liminal::entity editor_entity;

    editor()
    {
        load_scene();
    }

    void update(unsigned int current_time, float delta_time) override
    {
        ImGuiIO &io = ImGui::GetIO();

        auto &transform = editor_entity.get_component<liminal::transform>();
        auto &camera = editor_entity.get_component<liminal::camera>();
        glm::vec3 camera_front = camera.calc_front(transform);
        glm::vec3 camera_right = camera.calc_right(transform);

        if (playing)
        {
            // TODO: remove all this
            // camera movement while in play mode should be controlled by scripts
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
                if (liminal::input::key_down(liminal::keycode::KEYCODE_SPACE) && !jumping)
                {
                    velocity.y = 10;
                    jumping = true;
                }
            }
            float acceleration_length = glm::length(acceleration);
            if (acceleration_length > 1)
            {
                acceleration /= acceleration_length;
            }
            acceleration *= speed * (sprint ? 2 : 1);
            acceleration -= velocity * drag;
            acceleration.y = -9.8f;
            transform.position = 0.5f * acceleration * powf(delta_time, 2) + velocity * delta_time + transform.position;
            if (transform.position.y < 0)
            {
                transform.position.y = 0;
                jumping = false;
            }
            velocity = acceleration * delta_time + velocity;
            transform.rotation.z = glm::dot(camera_right, velocity);

            if (!io.WantCaptureMouse)
            {
                if (SDL_GetRelativeMouseMode())
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
        }
        else
        {
            if (liminal::input::mouse_button_down(liminal::mouse_button::MOUSE_BUTTON_LEFT))
            {
                auto mouse_position = ImGui::GetMousePos();
                // auto viewport_size = ImVec2(scene_region_bounds[1].x - scene_region_bounds[0].x, scene_region_bounds[1].y - scene_region_bounds[0].y);
                auto mouse_x = mouse_position.x - scene_region_bounds[0].x;
                auto mouse_y = scene_region_size.y - (mouse_position.y - scene_region_bounds[0].y);
                if (mouse_x >= 0 && mouse_x < scene_region_size.x && mouse_y >= 0 && mouse_y < scene_region_size.y)
                {
                    selected_entity_id = liminal::engine::instance->renderer->pick((int)mouse_x, (int)mouse_y);
                }
            }

            const float sensitivity = 0.1f;
            if (liminal::input::mouse_button(liminal::mouse_button::MOUSE_BUTTON_RIGHT))
            {
                transform.rotation.y -= liminal::input::mouse_dx * sensitivity;
                transform.rotation.x += liminal::input::mouse_dy * sensitivity;
                if (transform.rotation.x > 89)
                {
                    transform.rotation.x = 89;
                }
                if (transform.rotation.x < -89)
                {
                    transform.rotation.x = -89;
                }
            }
            else if (liminal::input::mouse_button(liminal::mouse_button::MOUSE_BUTTON_MIDDLE))
            {
                transform.position -= camera_right * (liminal::input::mouse_dx * sensitivity);
                transform.position += glm::vec3(0, 1, 0) * (liminal::input::mouse_dy * sensitivity);
            }

            transform.position += camera_front * (float)liminal::input::mouse_wheel_y;
        }

        if (liminal::input::key_down(liminal::keycode::KEYCODE_N))
        {
            if (liminal::input::key(liminal::keycode::KEYCODE_LCTRL))
            {
                new_scene();
            }
        }

        if (liminal::input::key_down(liminal::keycode::KEYCODE_O))
        {
            if (liminal::input::key(liminal::keycode::KEYCODE_LCTRL))
            {
                load_scene();
            }
        }

        if (liminal::input::key_down(liminal::keycode::KEYCODE_S))
        {
            if (liminal::input::key(liminal::keycode::KEYCODE_LCTRL))
            {
                if (liminal::input::key(liminal::keycode::KEYCODE_LSHIFT))
                {
                    save_scene();
                }
                else
                {
                    save_scene();
                }
            }
        }

        if (liminal::input::key_down(liminal::keycode::KEYCODE_F5))
        {
            if (liminal::input::key(liminal::keycode::KEYCODE_LSHIFT))
            {
                stop();
            }
            else
            {
                play();
            }
        }

        if (playing)
        {
            scene->update(current_time, delta_time);
        }

        ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

        ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        {
            window_flags |= ImGuiWindowFlags_NoBackground;
        }

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        if (ImGui::Begin("DockSpace Demo", NULL, window_flags))
        {
            ImGui::PopStyleVar(3);

            if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
            {
                ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
                ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
            }

            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("New", "Ctrl+N"))
                    {
                        new_scene();
                    }

                    if (ImGui::MenuItem("Open...", "Ctrl+O"))
                    {
                        load_scene();
                    }

                    if (ImGui::MenuItem("Save", "Ctrl+S"))
                    {
                        save_scene();
                    }

                    if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
                    {
                        save_scene();
                    }

                    if (ImGui::MenuItem("Exit", "Alt+F4"))
                    {
                        liminal::engine::instance->stop();
                    }

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Run"))
                {
                    if (ImGui::MenuItem("Play", "F5"))
                    {
                        play();
                    }

                    if (ImGui::MenuItem("Stop", "Shift+F5"))
                    {
                        stop();
                    }

                    ImGui::EndMenu();
                }

                ImGui::EndMenuBar();
            }

            if (ImGui::Begin("Scene", nullptr))
            {
                auto viewport_offset = ImGui::GetCursorPos();

                scene_region_size = ImGui::GetContentRegionAvail();
                static auto prev_scene_region_size = ImVec2(0, 0);
                if (scene_region_size.x != prev_scene_region_size.x || scene_region_size.y != prev_scene_region_size.y)
                {
                    liminal::engine::instance->renderer->set_target_size((int)scene_region_size.x, (int)scene_region_size.y);
                    prev_scene_region_size = scene_region_size;
                }

                ImGui::Image((ImTextureID)(long long)camera.render_texture_id, scene_region_size, ImVec2{0, 1}, ImVec2{1, 0});

                auto window_size = ImGui::GetWindowSize();
                auto min_bound = ImGui::GetWindowPos();
                min_bound.x += viewport_offset.x;
                min_bound.y += viewport_offset.y;
                auto max_bound = ImVec2(min_bound.x + window_size.x, min_bound.y + window_size.y);
                scene_region_bounds[0] = ImVec2(min_bound.x, min_bound.y);
                scene_region_bounds[1] = ImVec2(max_bound.x, max_bound.y);
            }
            ImGui::End();

            if (ImGui::Begin("Scene Hierarchy"))
            {
                for (auto [entity, transform] : scene->registry.view<liminal::transform>().each())
                {
                    ImGui::Text("%s (%d)", transform.name.c_str(), entity);
                }
            }
            ImGui::End();

            if (ImGui::Begin("Inspector"))
            {
                if (selected_entity_id != entt::null)
                {
                    auto entity = scene->get_entity(selected_entity_id);
                    auto transform = entity.get_component<liminal::transform>();
                    ImGui::Text("ID: %d", (int)selected_entity_id);
                    ImGui::Text("Name: %s", transform.name.c_str());
                }
            }
            ImGui::End();
        }
        ImGui::End();
    }

    void resize(int width, int height) override {}

private:
    bool playing = false;

    ImVec2 scene_region_bounds[2];
    ImVec2 scene_region_size;

    entt::entity selected_entity_id = entt::null;

    void new_scene()
    {
        if (scene)
        {
            delete scene;
        }
        scene = new liminal::scene();

        editor_entity = scene->create_entity();
        editor_entity.add_component<liminal::transform>("Editor", nullptr, glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
        editor_entity.add_component<liminal::camera>(45.f, true);
    }

    void load_scene()
    {
        new_scene();
        scene->load("assets/scenes/demo.json");
    }

    void save_scene()
    {
        scene->save("assets/scenes/demo.json");
    }

    void play()
    {
        playing = true;

        scene->delete_entity(editor_entity);

        // TODO: remove this, should just come from what is placed in the scene
        // the only caveat is that the camera.render_to_texture needs to be overridden to true when running in the editor, so that the scene can render to the ImGui window
        editor_entity = scene->create_entity();
        editor_entity.add_component<liminal::transform>("Player", nullptr, glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
        editor_entity.add_component<liminal::camera>(45.f, true);
        editor_entity.add_component<liminal::audio_listener>();

        for (auto [entity, script] : scene->registry.view<liminal::script>().each())
        {
            script.init();
        }
    }

    void stop()
    {
        playing = false;

        load_scene();
    }
};

liminal::app *liminal::create_app(int argc, char *argv[])
{
    return new editor();
}
