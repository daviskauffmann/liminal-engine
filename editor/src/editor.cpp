#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <iostream>
#include <liminal/liminal.hpp>
#include <liminal/main.hpp>

namespace editor
{
    class app : public liminal::app
    {
    public:
        app(int argc, char *argv[])
            : liminal::app(argc, argv)
        {
            load_scene();
        }

        void update(unsigned int current_time, float delta_time) override
        {
            ImGuiIO &io = ImGui::GetIO();

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
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
            window_flags |=
                ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoBringToFrontOnFocus |
                ImGuiWindowFlags_NoNavFocus;

            if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            {
                window_flags |= ImGuiWindowFlags_NoBackground;
            }

            if (ImGui::Begin("Editor", NULL, window_flags))
            {
                ImGui::PopStyleVar(3);

                if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
                {
                    ImGuiID dockspace_id = ImGui::GetID("Dockspace");
                    ImGui::DockSpace(dockspace_id, ImVec2(0, 0), dockspace_flags);
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
                            stop();
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
                    auto &camera = *liminal::renderer::instance->default_camera;

                    if (ImGui::IsWindowHovered())
                    {
                        auto &transform = *liminal::renderer::instance->default_camera_transform;
                        glm::vec3 camera_front = camera.calc_front(transform);
                        glm::vec3 camera_right = camera.calc_right(transform);

                        if (liminal::input::mouse_button_down(liminal::mouse_button::MOUSE_BUTTON_LEFT))
                        {
                            auto mouse_position = ImGui::GetMousePos();
                            auto mouse_x = mouse_position.x - scene_region_bounds[0].x;
                            auto mouse_y = scene_region_size.y - (mouse_position.y - scene_region_bounds[0].y);
                            if (mouse_x >= 0 && mouse_x < scene_region_size.x && mouse_y >= 0 && mouse_y < scene_region_size.y)
                            {
                                selected_entity = liminal::renderer::instance->pick((int)mouse_x, (int)mouse_y, scene);
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

                        if (liminal::input::mouse_button(liminal::mouse_button::MOUSE_BUTTON_MIDDLE))
                        {
                            transform.position -= camera_right * (liminal::input::mouse_dx * sensitivity);
                            transform.position += glm::vec3(0, 1, 0) * (liminal::input::mouse_dy * sensitivity);
                        }

                        transform.position += camera_front * (float)liminal::input::mouse_wheel_y;
                    }

                    scene_region_size = ImGui::GetContentRegionAvail();
                    static auto prev_scene_region_size = ImVec2(0, 0);
                    if (scene_region_size.x != prev_scene_region_size.x || scene_region_size.y != prev_scene_region_size.y)
                    {
                        liminal::renderer::instance->set_target_size((int)scene_region_size.x, (int)scene_region_size.y);
                        prev_scene_region_size = scene_region_size;
                    }

                    ImGui::Image((ImTextureID)(long long)camera.render_texture_id, scene_region_size, ImVec2{0, 1}, ImVec2{1, 0});

                    auto min_region = ImGui::GetWindowContentRegionMin();
                    auto max_region = ImGui::GetWindowContentRegionMax();
                    auto viewport_offset = ImGui::GetWindowPos();
                    scene_region_bounds[0] = ImVec2(min_region.x + viewport_offset.x, min_region.y + viewport_offset.y);
                    scene_region_bounds[1] = ImVec2(max_region.x + viewport_offset.x, max_region.y + viewport_offset.y);
                }
                ImGui::End();

                if (ImGui::Begin("Entities"))
                {
                    for (auto [id, transform] : scene->get_entities_with<liminal::transform>().each())
                    {
                        if (!transform.parent)
                        {
                            draw_entity_node(liminal::entity(id, scene), transform);
                        }
                    }

                    if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
                    {
                        selected_entity = liminal::entity();
                    }

                    if (ImGui::BeginPopupContextWindow(0, 1, false))
                    {
                        if (ImGui::MenuItem("Create Entity"))
                        {
                            auto entity = selected_entity = scene->create_entity();
                            entity.add_component<liminal::transform>();
                        }

                        ImGui::EndPopup();
                    }
                }
                ImGui::End();

                if (ImGui::Begin("Selected"))
                {
                    if (selected_entity)
                    {
                        const ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;

                        if (selected_entity.has_components<liminal::transform>())
                        {
                            auto opened = ImGui::TreeNodeEx((void *)typeid(liminal::transform).hash_code(), flags, "Transform");

                            if (opened)
                            {
                                auto &transform = selected_entity.get_component<liminal::transform>();

                                char buffer[256];
                                memset(buffer, 0, sizeof(buffer));
                                memcpy(buffer, transform.name.c_str(), sizeof(buffer));
                                if (ImGui::InputText("Name", buffer, sizeof(buffer)))
                                {
                                    transform.name = std::string(buffer);
                                }

                                ImGui::Text("Parent: %s", transform.parent ? transform.parent->name.c_str() : "null");

                                ImGui::DragFloat3("Position", glm::value_ptr(transform.position), .1f);
                                ImGui::DragFloat3("Rotation", glm::value_ptr(transform.rotation), .1f);
                                ImGui::DragFloat3("Scale", glm::value_ptr(transform.scale), .1f);

                                ImGui::TreePop();
                            }
                        }

                        if (selected_entity.has_components<liminal::mesh_renderer>())
                        {
                            auto opened = ImGui::TreeNodeEx((void *)typeid(liminal::mesh_renderer).hash_code(), flags, "Mesh Renderer");

                            auto deleted = false;
                            if (ImGui::BeginPopupContextItem())
                            {
                                if (ImGui::MenuItem("Remove"))
                                {
                                    deleted = true;
                                }

                                ImGui::EndPopup();
                            }

                            if (opened)
                            {
                                auto &mesh_renderer = selected_entity.get_component<liminal::mesh_renderer>();

                                if (ImGui::Button("Load Model"))
                                {
                                    // TODO: file dialog
                                    // TODO: drag and drop from asset browser
                                    mesh_renderer.model = liminal::assets::instance->load_model("assets/models/backpack/backpack.obj", true);
                                }

                                ImGui::TreePop();
                            }

                            if (deleted)
                            {
                                selected_entity.remove_component<liminal::mesh_renderer>();
                            }
                        }

                        if (selected_entity.has_components<liminal::point_light>())
                        {
                            auto opened = ImGui::TreeNodeEx((void *)typeid(liminal::point_light).hash_code(), flags, "Point Light");

                            auto deleted = false;
                            if (ImGui::BeginPopupContextItem())
                            {
                                if (ImGui::MenuItem("Remove"))
                                {
                                    deleted = true;
                                }

                                ImGui::EndPopup();
                            }

                            if (opened)
                            {
                                auto &point_light = selected_entity.get_component<liminal::point_light>();

                                ImGui::DragFloat3("Color", glm::value_ptr(point_light.color), .1f);

                                ImGui::TreePop();
                            }

                            if (deleted)
                            {
                                selected_entity.remove_component<liminal::point_light>();
                            }
                        }

                        if (ImGui::Button("Add Component"))
                        {
                            ImGui::OpenPopup("AddComponent");
                        }

                        if (ImGui::BeginPopup("AddComponent"))
                        {
                            if (!selected_entity.has_components<liminal::mesh_renderer>() && ImGui::MenuItem("Mesh Renderer"))
                            {
                                selected_entity.add_component<liminal::mesh_renderer>();

                                ImGui::CloseCurrentPopup();
                            }

                            if (!selected_entity.has_components<liminal::point_light>() && ImGui::MenuItem("Point Light"))
                            {
                                selected_entity.add_component<liminal::point_light>();

                                ImGui::CloseCurrentPopup();
                            }

                            ImGui::EndPopup();
                        }
                    }
                }
                ImGui::End();

                if (ImGui::Begin("Assets"))
                {
                    if (current_asset_directory.has_parent_path())
                    {
                        if (ImGui::Button("../"))
                        {
                            current_asset_directory = current_asset_directory.parent_path();
                        }
                    }

                    for (auto &directory_entry : std::filesystem::directory_iterator(current_asset_directory))
                    {
                        auto path = directory_entry.path();
                        auto filename = path.filename();
                        auto filename_string = filename.string();
                        if (ImGui::Button(filename_string.c_str()))
                        {
                            if (directory_entry.is_directory())
                            {
                                current_asset_directory /= filename;
                            }
                            else
                            {
                            }
                        }
                    }
                }
                ImGui::End();
            }
            ImGui::End();
        }

        void resize(int width, int height) override
        {
        }

    private:
        bool playing = false;
        liminal::scene *editor_scene;

        ImVec2 scene_region_bounds[2];
        ImVec2 scene_region_size;

        liminal::entity selected_entity;

        std::filesystem::path current_asset_directory = "assets";

        void new_scene()
        {
            if (scene)
            {
                delete scene;
            }
            scene = new liminal::scene();

            liminal::renderer::instance->default_camera = new liminal::camera(45.f, true);
            liminal::renderer::instance->default_camera_transform = new liminal::transform();

            selected_entity = liminal::entity();
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

            delete liminal::renderer::instance->default_camera;
            delete liminal::renderer::instance->default_camera_transform;

            // TODO: remove this, should just come from what is placed in the scene
            // the only caveat is that the camera.render_to_texture needs to be overridden to true when running in the editor, so that the scene can render to the ImGui window
            auto player = scene->create_entity();
            player.add_component<liminal::transform>("Player");
            player.add_component<liminal::camera>(45.f, true);
            player.add_component<liminal::audio_listener>();

            for (auto [id, script] : scene->get_entities_with<liminal::script>().each())
            {
                script.init();
            }
        }

        void stop()
        {
            playing = false;

            load_scene();
        }

        void delete_entity(liminal::entity entity, liminal::transform &transform)
        {
            for (auto [child_id, child_transform] : scene->get_entities_with<liminal::transform>().each())
            {
                if (child_transform.parent == &transform)
                {
                    delete_entity(liminal::entity(child_id, scene), child_transform);
                }
            }

            scene->delete_entity(entity);

            if (entity == selected_entity)
            {
                selected_entity = liminal::entity();
            }
        }

        void draw_entity_node(liminal::entity entity, liminal::transform &transform)
        {
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
            if (entity == selected_entity)
            {
                flags |= ImGuiTreeNodeFlags_Selected;
            }
            auto opened = ImGui::TreeNodeEx((void *)(uint64_t)entity, flags, transform.name.c_str());

            if (ImGui::IsItemClicked())
            {
                selected_entity = entity;
            }

            auto deleted = false;
            if (ImGui::BeginPopupContextItem())
            {
                if (ImGui::MenuItem("Create Child"))
                {
                    auto child = selected_entity = scene->create_entity();
                    auto &child_transform = child.add_component<liminal::transform>();
                    child_transform.parent = &transform;
                }

                if (ImGui::MenuItem("Delete"))
                {
                    deleted = true;
                }

                ImGui::EndPopup();
            }

            if (opened)
            {
                for (auto [child_id, child_transform] : scene->get_entities_with<liminal::transform>().each())
                {
                    if (child_transform.parent == &transform)
                    {
                        draw_entity_node(liminal::entity(child_id, scene), child_transform);
                    }
                }

                ImGui::TreePop();
            }

            if (deleted)
            {
                delete_entity(entity, transform);
            }
        }
    };
}

liminal::app *liminal::create_app(int argc, char *argv[])
{
    return new editor::app(argc, argv);
}
