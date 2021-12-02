#include <glm/glm.hpp>
#include <imgui.h>
#include <iostream>
#include <liminal/liminal.hpp>
#include <SDL2/SDL.h>

class editor : public liminal::app
{
public:
    liminal::entity camera_entity;

    editor()
    {
        scene = new liminal::scene();
        scene->draw_to_texture = true;
        scene->load("assets/scenes/demo.json");

        // TODO: camera should be a component maybe
        camera_entity = scene->create_entity();
        camera_entity.add_component<liminal::transform>("Camera", nullptr, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        camera_entity.add_component<liminal::audio_listener>();
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
            if (camera->pitch > 89.0f)
            {
                camera->pitch = 89.0f;
            }
            if (camera->pitch < -89.0f)
            {
                camera->pitch = -89.0f;
            }
        }
        else if (liminal::input::mouse_button(liminal::mouse_button::MOUSE_BUTTON_MIDDLE))
        {
            camera->position -= camera_right * (liminal::input::mouse_dx * sensitivity);
            camera->position += glm::vec3(0.0f, 1.0f, 0.0f) * (liminal::input::mouse_dy * sensitivity);
        }

        camera->position += camera_front * (float)liminal::input::mouse_wheel_y;

        // TODO: camera itself should be a component attached to the camera_entity
        // and its transform would be changed directly, making this step unnecessary
        camera_entity.get_component<liminal::transform>().position = camera->position;
        camera_entity.get_component<liminal::transform>().rotation = camera->calc_front();

        static bool dockspace_open = true;
        ImGui::Begin("Dockspace Demo", &dockspace_open, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_MenuBar);
        {
            ImGui::DockSpace(ImGui::GetID("MyDockSpace"), ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

            if (ImGui::BeginMenuBar())
            {
                if (ImGui::MenuItem("Exit"))
                {
                    // TODO: exit
                }
            }
            ImGui::EndMenuBar();

            if (ImGui::Begin("Scene"))
            {
                ImGui::Image(scene->texture_id, ImGui::GetWindowSize(), ImVec2{0, 1}, ImVec2{1, 0});
            }
            ImGui::End();

            if (ImGui::Begin("Scene Hierarchy"))
            {
                for (auto [entity, transform] : scene->registry.view<liminal::transform>().each())
                {
                    ImGui::Text("%s", transform.name);
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
