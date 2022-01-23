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
        scene = new liminal::scene();
        scene->load("assets/scenes/demo.json");

        liminal::engine::get_instance().renderer->draw_to_imgui_texture = true;
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

        ImGui::Begin("Main", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_MenuBar);
        {
            ImGui::DockSpace(ImGui::GetID("Dockspace"), ImVec2(0, 0), ImGuiDockNodeFlags_PassthruCentralNode);

            // if (ImGui::BeginMenuBar())
            // {
            //     if (ImGui::BeginMenu("File"))
            //     {
            //         if (ImGui::MenuItem("Exit"))
            //         {
            //             // TODO: exit
            //         }
            //     }
            //     ImGui::EndMenu();
            // }
            // ImGui::EndMenuBar();

            if (ImGui::Begin("Scene", nullptr))
            {
                static ImVec2 region_size = ImGui::GetContentRegionAvail();
                ImVec2 new_region_size = ImGui::GetContentRegionAvail();
                if (new_region_size.x != region_size.x || new_region_size.y != region_size.y)
                {
                    liminal::engine::get_instance().renderer->set_target_size((int)new_region_size.x, (int)new_region_size.y);
                    region_size = new_region_size;
                }
                ImGui::Image(liminal::engine::get_instance().renderer->imgui_texture_id, region_size, ImVec2{0, 1}, ImVec2{1, 0});
            }
            ImGui::End();

            if (ImGui::Begin("Scene Hierarchy"))
            {
                for (auto [entity, transform] : scene->registry.view<liminal::transform>().each())
                {
                    ImGui::Text("%s", transform.name.c_str());
                }
            }
            ImGui::End();
        }
        ImGui::End();
    }
};

liminal::app *liminal::create_app(int argc, char *argv[])
{
    return new editor();
}
