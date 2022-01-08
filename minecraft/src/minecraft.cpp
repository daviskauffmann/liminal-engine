#include <glm/glm.hpp>
#include <imgui.h>
#include <iostream>
#include <liminal/liminal.hpp>
#include <SDL2/SDL.h>

struct mesh_data
{
    std::vector<liminal::vertex> vertices;
    std::vector<unsigned int> indices;

    void add_quad_indices();
};

enum direction
{
    DIRECTION_NORTH,
    DIRECTION_EAST,
    DIRECTION_SOUTH,
    DIRECTION_WEST,
    DIRECTION_UP,
    DIRECTION_DOWN,
};

class block;

class chunk
{
public:
    block *blocks[16][16][16];

    block *get_block(int x, int y, int z);
    liminal::mesh *render(liminal::texture *tiles_texture);
};

struct tile
{
    int x;
    int y;
};

class block
{
public:
    virtual bool is_solid(direction direction);
    virtual void add_to_mesh(chunk *chunk, int x, int y, int z, mesh_data *mesh_data);

private:
    virtual void add_face_up(chunk *chunk, int x, int y, int z, mesh_data *mesh_data);
    virtual void add_face_down(chunk *chunk, int x, int y, int z, mesh_data *mesh_data);
    virtual void add_face_north(chunk *chunk, int x, int y, int z, mesh_data *mesh_data);
    virtual void add_face_east(chunk *chunk, int x, int y, int z, mesh_data *mesh_data);
    virtual void add_face_south(chunk *chunk, int x, int y, int z, mesh_data *mesh_data);
    virtual void add_face_west(chunk *chunk, int x, int y, int z, mesh_data *mesh_data);
    virtual std::vector<glm::vec2> get_face_uvs(direction direction);
    virtual tile get_tile(direction direction);
};

class air_block : public block
{
    bool is_solid(direction direction) override;
    void add_to_mesh(chunk *chunk, int x, int y, int z, mesh_data *mesh_data) override;
};

class grass_block : public block
{
public:
    tile get_tile(direction direction) override;
};

void mesh_data::add_quad_indices()
{
    indices.push_back((unsigned int)(vertices.size() - 4));
    indices.push_back((unsigned int)(vertices.size() - 3));
    indices.push_back((unsigned int)(vertices.size() - 2));

    indices.push_back((unsigned int)(vertices.size() - 4));
    indices.push_back((unsigned int)(vertices.size() - 2));
    indices.push_back((unsigned int)(vertices.size() - 1));
}

block *chunk::get_block(int x, int y, int z)
{
    return blocks[x][y][z];
}

liminal::mesh *chunk::render(liminal::texture *tiles_texture)
{
    mesh_data mesh_data;

    for (int x = 0; x < 16; x++)
    {
        for (int y = 0; y < 16; y++)
        {
            for (int z = 0; z < 16; z++)
            {
                blocks[x][y][z]->add_to_mesh(this, x, y, z, &mesh_data);
            }
        }
    }

    std::vector<std::vector<liminal::texture *>> textures;
    for (aiTextureType type = aiTextureType_NONE; type <= AI_TEXTURE_TYPE_MAX; type = (aiTextureType)(type + 1))
    {
        std::vector<liminal::texture *> textures_for_type;
        textures.push_back(textures_for_type);
    }
    textures[aiTextureType_DIFFUSE].push_back(tiles_texture);

    return new liminal::mesh(mesh_data.vertices, mesh_data.indices, textures);
}

bool block::is_solid(direction direction)
{
    switch (direction)
    {
    case direction::DIRECTION_NORTH:
        return true;
    case direction::DIRECTION_EAST:
        return true;
    case direction::DIRECTION_SOUTH:
        return true;
    case direction::DIRECTION_WEST:
        return true;
    case direction::DIRECTION_UP:
        return true;
    case direction::DIRECTION_DOWN:
        return true;
    default:
        return false;
    }
}

void block::add_to_mesh(chunk *chunk, int x, int y, int z, mesh_data *mesh_data)
{
    if (!chunk->get_block(x, y + 1, z)->is_solid(direction::DIRECTION_DOWN))
    {
        add_face_up(chunk, x, y, z, mesh_data);
    }

    if (!chunk->get_block(x, y - 1, z)->is_solid(direction::DIRECTION_UP))
    {
        add_face_down(chunk, x, y, z, mesh_data);
    }

    if (!chunk->get_block(x, y, z + 1)->is_solid(direction::DIRECTION_SOUTH))
    {
        add_face_north(chunk, x, y, z, mesh_data);
    }

    if (!chunk->get_block(x, y, z - 1)->is_solid(direction::DIRECTION_NORTH))
    {
        add_face_south(chunk, x, y, z, mesh_data);
    }

    if (!chunk->get_block(x + 1, y, z)->is_solid(direction::DIRECTION_WEST))
    {
        add_face_east(chunk, x, y, z, mesh_data);
    }

    if (!chunk->get_block(x - 1, y, z)->is_solid(direction::DIRECTION_EAST))
    {
        add_face_west(chunk, x, y, z, mesh_data);
    }
}

void block::add_face_up(chunk *chunk, int x, int y, int z, mesh_data *mesh_data)
{
    std::vector<glm::vec2> uvs = get_face_uvs(direction::DIRECTION_UP);

    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x - 0.5f, y + 0.5f, z + 0.5f), glm::vec3(0, 1, 0), uvs[0]});
    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x + 0.5f, y + 0.5f, z + 0.5f), glm::vec3(0, 1, 0), uvs[1]});
    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x + 0.5f, y + 0.5f, z - 0.5f), glm::vec3(0, 1, 0), uvs[2]});
    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x - 0.5f, y + 0.5f, z - 0.5f), glm::vec3(0, 1, 0), uvs[3]});

    mesh_data->add_quad_indices();
}

void block::add_face_down(chunk *chunk, int x, int y, int z, mesh_data *mesh_data)
{
    std::vector<glm::vec2> uvs = get_face_uvs(direction::DIRECTION_DOWN);

    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x - 0.5f, y - 0.5f, z - 0.5f), glm::vec3(0, -1, 0), uvs[0]});
    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x + 0.5f, y - 0.5f, z - 0.5f), glm::vec3(0, -1, 0), uvs[1]});
    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x + 0.5f, y - 0.5f, z + 0.5f), glm::vec3(0, -1, 0), uvs[2]});
    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x - 0.5f, y - 0.5f, z + 0.5f), glm::vec3(0, -1, 0), uvs[3]});

    mesh_data->add_quad_indices();
}

void block::add_face_north(chunk *chunk, int x, int y, int z, mesh_data *mesh_data)
{
    std::vector<glm::vec2> uvs = get_face_uvs(direction::DIRECTION_NORTH);

    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x + 0.5f, y - 0.5f, z + 0.5f), glm::vec3(0, 0, 1), uvs[0]});
    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x + 0.5f, y + 0.5f, z + 0.5f), glm::vec3(0, 0, 1), uvs[1]});
    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x - 0.5f, y + 0.5f, z + 0.5f), glm::vec3(0, 0, 1), uvs[2]});
    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x - 0.5f, y - 0.5f, z + 0.5f), glm::vec3(0, 0, 1), uvs[3]});

    mesh_data->add_quad_indices();
}

void block::add_face_east(chunk *chunk, int x, int y, int z, mesh_data *mesh_data)
{
    std::vector<glm::vec2> uvs = get_face_uvs(direction::DIRECTION_EAST);

    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x + 0.5f, y - 0.5f, z - 0.5f), glm::vec3(1, 0, 0), uvs[0]});
    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x + 0.5f, y + 0.5f, z - 0.5f), glm::vec3(1, 0, 0), uvs[1]});
    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x + 0.5f, y + 0.5f, z + 0.5f), glm::vec3(1, 0, 0), uvs[2]});
    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x + 0.5f, y - 0.5f, z + 0.5f), glm::vec3(1, 0, 0), uvs[3]});

    mesh_data->add_quad_indices();
}

void block::add_face_south(chunk *chunk, int x, int y, int z, mesh_data *mesh_data)
{
    std::vector<glm::vec2> uvs = get_face_uvs(direction::DIRECTION_SOUTH);

    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x - 0.5f, y - 0.5f, z - 0.5f), glm::vec3(0, 0, -1), uvs[0]});
    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x - 0.5f, y + 0.5f, z - 0.5f), glm::vec3(0, 0, -1), uvs[1]});
    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x + 0.5f, y + 0.5f, z - 0.5f), glm::vec3(0, 0, -1), uvs[2]});
    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x + 0.5f, y - 0.5f, z - 0.5f), glm::vec3(0, 0, -1), uvs[3]});

    mesh_data->add_quad_indices();
}

void block::add_face_west(chunk *chunk, int x, int y, int z, mesh_data *mesh_data)
{
    std::vector<glm::vec2> uvs = get_face_uvs(direction::DIRECTION_WEST);

    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x - 0.5f, y - 0.5f, z + 0.5f), glm::vec3(-1, 0, 0), uvs[0]});
    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x - 0.5f, y + 0.5f, z + 0.5f), glm::vec3(-1, 0, 0), uvs[1]});
    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x - 0.5f, y + 0.5f, z - 0.5f), glm::vec3(-1, 0, 0), uvs[2]});
    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x - 0.5f, y - 0.5f, z - 0.5f), glm::vec3(-1, 0, 0), uvs[3]});

    mesh_data->add_quad_indices();
}

std::vector<glm::vec2> block::get_face_uvs(direction direction)
{
    std::vector<glm::vec2> uvs;
    tile tile = get_tile(direction);
    const float tile_size = 0.25f;
    uvs.push_back(glm::vec2(tile_size * tile.x + tile_size, tile_size * tile.y));
    uvs.push_back(glm::vec2(tile_size * tile.x + tile_size, tile_size * tile.y + tile_size));
    uvs.push_back(glm::vec2(tile_size * tile.x, tile_size * tile.y + tile_size));
    uvs.push_back(glm::vec2(tile_size * tile.x, tile_size * tile.y));
    return uvs;
}

tile block::get_tile(direction direction)
{
    tile tile;
    tile.x = 0;
    tile.y = 0;
    return tile;
}

void air_block::add_to_mesh(chunk *chunk, int x, int y, int z, mesh_data *mesh_data)
{
}

bool air_block::is_solid(direction direction)
{
    return false;
}

tile grass_block::get_tile(direction direction)
{
    tile tile;
    switch (direction)
    {
    case direction::DIRECTION_UP:
        tile.x = 2;
        tile.y = 0;
        return tile;
    case direction::DIRECTION_DOWN:
        tile.x = 1;
        tile.y = 0;
        return tile;
    default:
        tile.x = 3;
        tile.y = 0;
        return tile;
    }
}

class minecraft : public liminal::app
{
public:
    minecraft()
    {
        SDL_SetRelativeMouseMode(SDL_TRUE);

        scene = new liminal::scene();
        scene->load("assets/scenes/minecraft.json");

        liminal::texture *tiles_texture = new liminal::texture("assets/images/tiles.png", false, false);

        chunk chunk;
        for (int x = 0; x < 16; x++)
        {
            for (int y = 0; y < 16; y++)
            {
                for (int z = 0; z < 16; z++)
                {
                    chunk.blocks[x][y][z] = new air_block();
                }
            }
        }
        chunk.blocks[3][5][2] = new block();
        chunk.blocks[4][5][2] = new grass_block();

        liminal::mesh *chunk_mesh = chunk.render(tiles_texture);
        liminal::model *chunk_model = new liminal::model(chunk_mesh);
        liminal::entity chunk_entity = scene->create_entity();
        chunk_entity.add_component<liminal::transform>("Chunk", nullptr, glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
        chunk_entity.add_component<liminal::mesh_renderer>(chunk_model);
    }

    ~minecraft()
    {
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
};

liminal::app *liminal::create_app(int argc, char *argv[])
{
    return new minecraft();
}
