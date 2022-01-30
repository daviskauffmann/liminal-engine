#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <imgui.h>
#include <iostream>
#include <liminal/liminal.hpp>
#include <liminal/main.hpp>
#include <SDL2/SDL.h>
#include <unordered_map>

#include "blocks/air_block.hpp"
#include "blocks/grass_block.hpp"
#include "block.hpp"
#include "chunk.hpp"
#include "mesh_data.hpp"

namespace minecraft
{
    class app : public liminal::app
    {
    public:
        app()
        {
            SDL_SetRelativeMouseMode(SDL_TRUE);

            scene = new liminal::scene();
            scene->load("assets/scenes/minecraft.json");

            tiles_texture = new liminal::texture("assets/images/tiles.png", false, false);

            for (int x = -2; x < 2; x++)
            {
                for (int y = -1; y < 1; y++)
                {
                    for (int z = -1; z < 1; z++)
                    {
                        create_chunk(x * minecraft::chunk::size, y * minecraft::chunk::size, z * minecraft::chunk::size);
                    }
                }
            }
        }

        ~app()
        {
            delete tiles_texture;
        }

        void update(unsigned int current_time, float delta_time) override
        {
            ImGuiIO &io = ImGui::GetIO();

            if (liminal::input::key_down(liminal::KEYCODE_TAB))
            {
                SDL_SetRelativeMouseMode((SDL_bool)!SDL_GetRelativeMouseMode());
            }

            auto camera = scene->camera;
            glm::vec3 camera_front = camera->calc_front();
            glm::vec3 camera_right = camera->calc_right();

            static glm::vec3 velocity(0, 0, 0);
            glm::vec3 acceleration(0, 0, 0);
            const float speed = 50;
            const float drag = 10;
            bool sprint = false;
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
            }
            float acceleration_length = glm::length(acceleration);
            if (acceleration_length > 1)
            {
                acceleration /= acceleration_length;
            }
            acceleration *= speed * (sprint ? 2 : 1);
            acceleration -= velocity * drag;
            camera->position = 0.5f * acceleration * powf(delta_time, 2) + velocity * delta_time + camera->position;
            velocity = acceleration * delta_time + velocity;
            camera->roll = glm::dot(camera_right, velocity);

            if (!io.WantCaptureMouse)
            {
                if (SDL_GetRelativeMouseMode())
                {
                    const float sensitivity = 0.1f;
                    camera->pitch -= liminal::input::mouse_dy * sensitivity;
                    camera->yaw += liminal::input::mouse_dx * sensitivity;
                    if (camera->pitch > 89)
                    {
                        camera->pitch = 89;
                    }
                    if (camera->pitch < -89)
                    {
                        camera->pitch = -89;
                    }

                    camera->fov -= liminal::input::mouse_wheel_y;
                    if (camera->fov <= 1)
                    {
                        camera->fov = 1;
                    }
                    if (camera->fov >= 120)
                    {
                        camera->fov = 120;
                    }
                }
            }

            scene->update(current_time, delta_time);
        }

    private:
        liminal::texture *tiles_texture;
        std::unordered_map<glm::ivec3, minecraft::chunk> chunks;

        void create_chunk(int x, int y, int z)
        {
            liminal::entity chunk_entity = scene->create_entity();
            chunk_entity.add_component<liminal::transform>("Chunk", nullptr, glm::vec3(x, y, z), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
            auto &chunk = chunk_entity.add_component<minecraft::chunk>();
            chunk_entity.add_component<liminal::mesh_renderer>(new liminal::model(chunk.render(tiles_texture)));

            chunk.position = glm::ivec3(x, y, z);

            chunks.emplace(chunk.position, chunk);
        }

        minecraft::chunk &get_chunk(int x, int y, int z)
        {
            glm::ivec3 world_position;
            world_position.x = (int)std::floor(x / minecraft::chunk::size) * minecraft::chunk::size;
            world_position.y = (int)std::floor(y / minecraft::chunk::size) * minecraft::chunk::size;
            world_position.z = (int)std::floor(z / minecraft::chunk::size) * minecraft::chunk::size;

            if (chunks.find(world_position) == chunks.end())
            {
                create_chunk(x, y, z);
            }

            return chunks[world_position];
        }

        minecraft::block *get_block(int x, int y, int z)
        {
            auto &chunk = get_chunk(x, y, z);
            return chunk.get_block(x - chunk.position.x, y - chunk.position.y, z - chunk.position.z);
        }
    };
}

liminal::app *liminal::create_app(int argc, char *argv[])
{
    return new minecraft::app();
}
