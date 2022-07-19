#include <glm/glm.hpp>
#include <imgui.h>
#include <iostream>
#include <liminal/liminal.hpp>
#include <liminal/main.hpp>
#include <SDL2/SDL.h>

class editor : public liminal::app
{
public:
    editor()
    {
        load_scene();
        liminal::engine::instance->renderer->draw_to_imgui_texture = true;
    }

    void update(unsigned int current_time, float delta_time) override
    {
        ImGuiIO &io = ImGui::GetIO();

        auto camera = scene->camera;
        glm::vec3 camera_front = camera->calc_front();
        glm::vec3 camera_right = camera->calc_right();

        const float sensitivity = 0.1f;
        if (liminal::input::mouse_button(liminal::mouse_button::MOUSE_BUTTON_LEFT))
        {
        }
        else if (liminal::input::mouse_button(liminal::mouse_button::MOUSE_BUTTON_RIGHT))
        {
            camera->yaw -= liminal::input::mouse_dx * sensitivity;
            camera->pitch += liminal::input::mouse_dy * sensitivity;
            if (camera->pitch > 89)
            {
                camera->pitch = 89;
            }
            if (camera->pitch < -89)
            {
                camera->pitch = -89;
            }
        }
        else if (liminal::input::mouse_button(liminal::mouse_button::MOUSE_BUTTON_MIDDLE))
        {
            camera->position -= camera_right * (liminal::input::mouse_dx * sensitivity);
            camera->position += glm::vec3(0, 1, 0) * (liminal::input::mouse_dy * sensitivity);
        }

        camera->position += camera_front * (float)liminal::input::mouse_wheel_y;

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
                static ImVec2 region_size = ImGui::GetContentRegionAvail();
                ImVec2 new_region_size = ImGui::GetContentRegionAvail();
                if (new_region_size.x != region_size.x || new_region_size.y != region_size.y)
                {
                    liminal::engine::instance->renderer->set_target_size((int)new_region_size.x, (int)new_region_size.y);
                    region_size = new_region_size;
                }
                ImGui::Image(liminal::engine::instance->renderer->imgui_texture_id, region_size, ImVec2{0, 1}, ImVec2{1, 0});
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
        }
        ImGui::End();
    }

private:
    bool playing = false;

    void new_scene()
    {
        if (scene)
        {
            delete scene;
        }
        scene = new liminal::scene();
        scene->camera = new liminal::camera(
            glm::vec3(0, 0, 3),
            0,
            0,
            0,
            45);
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
