#include "block.hpp"

#include "chunk.hpp"
#include "mesh_data.hpp"

bool minecraft::block::is_solid(minecraft::direction direction)
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

void minecraft::block::add_to_mesh(minecraft::chunk *chunk, int x, int y, int z, minecraft::mesh_data *mesh_data)
{
    if (chunk->get_block(x, y + 1, z)->is_solid(direction::DIRECTION_DOWN))
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

void minecraft::block::add_face_up(minecraft::chunk *, int x, int y, int z, minecraft::mesh_data *mesh_data)
{
    const auto uvs = get_face_uvs(direction::DIRECTION_UP);

    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x - 0.5f, y + 0.5f, z + 0.5f), glm::vec3(0, 1, 0), uvs[0]});
    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x + 0.5f, y + 0.5f, z + 0.5f), glm::vec3(0, 1, 0), uvs[1]});
    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x + 0.5f, y + 0.5f, z - 0.5f), glm::vec3(0, 1, 0), uvs[2]});
    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x - 0.5f, y + 0.5f, z - 0.5f), glm::vec3(0, 1, 0), uvs[3]});

    mesh_data->add_quad_indices();
}

void minecraft::block::add_face_down(minecraft::chunk *, int x, int y, int z, minecraft::mesh_data *mesh_data)
{
    const auto uvs = get_face_uvs(direction::DIRECTION_DOWN);

    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x - 0.5f, y - 0.5f, z - 0.5f), glm::vec3(0, -1, 0), uvs[0]});
    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x + 0.5f, y - 0.5f, z - 0.5f), glm::vec3(0, -1, 0), uvs[1]});
    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x + 0.5f, y - 0.5f, z + 0.5f), glm::vec3(0, -1, 0), uvs[2]});
    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x - 0.5f, y - 0.5f, z + 0.5f), glm::vec3(0, -1, 0), uvs[3]});

    mesh_data->add_quad_indices();
}

void minecraft::block::add_face_north(minecraft::chunk *, int x, int y, int z, minecraft::mesh_data *mesh_data)
{
    const auto uvs = get_face_uvs(direction::DIRECTION_NORTH);

    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x + 0.5f, y - 0.5f, z + 0.5f), glm::vec3(0, 0, 1), uvs[0]});
    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x + 0.5f, y + 0.5f, z + 0.5f), glm::vec3(0, 0, 1), uvs[1]});
    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x - 0.5f, y + 0.5f, z + 0.5f), glm::vec3(0, 0, 1), uvs[2]});
    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x - 0.5f, y - 0.5f, z + 0.5f), glm::vec3(0, 0, 1), uvs[3]});

    mesh_data->add_quad_indices();
}

void minecraft::block::add_face_east(minecraft::chunk *, int x, int y, int z, minecraft::mesh_data *mesh_data)
{
    const auto uvs = get_face_uvs(direction::DIRECTION_EAST);

    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x + 0.5f, y - 0.5f, z - 0.5f), glm::vec3(1, 0, 0), uvs[0]});
    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x + 0.5f, y + 0.5f, z - 0.5f), glm::vec3(1, 0, 0), uvs[1]});
    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x + 0.5f, y + 0.5f, z + 0.5f), glm::vec3(1, 0, 0), uvs[2]});
    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x + 0.5f, y - 0.5f, z + 0.5f), glm::vec3(1, 0, 0), uvs[3]});

    mesh_data->add_quad_indices();
}

void minecraft::block::add_face_south(minecraft::chunk *, int x, int y, int z, minecraft::mesh_data *mesh_data)
{
    const auto uvs = get_face_uvs(direction::DIRECTION_SOUTH);

    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x - 0.5f, y - 0.5f, z - 0.5f), glm::vec3(0, 0, -1), uvs[0]});
    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x - 0.5f, y + 0.5f, z - 0.5f), glm::vec3(0, 0, -1), uvs[1]});
    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x + 0.5f, y + 0.5f, z - 0.5f), glm::vec3(0, 0, -1), uvs[2]});
    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x + 0.5f, y - 0.5f, z - 0.5f), glm::vec3(0, 0, -1), uvs[3]});

    mesh_data->add_quad_indices();
}

void minecraft::block::add_face_west(minecraft::chunk *, int x, int y, int z, minecraft::mesh_data *mesh_data)
{
    const auto uvs = get_face_uvs(direction::DIRECTION_WEST);

    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x - 0.5f, y - 0.5f, z + 0.5f), glm::vec3(-1, 0, 0), uvs[0]});
    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x - 0.5f, y + 0.5f, z + 0.5f), glm::vec3(-1, 0, 0), uvs[1]});
    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x - 0.5f, y + 0.5f, z - 0.5f), glm::vec3(-1, 0, 0), uvs[2]});
    mesh_data->vertices.push_back(liminal::vertex{glm::vec3(x - 0.5f, y - 0.5f, z - 0.5f), glm::vec3(-1, 0, 0), uvs[3]});

    mesh_data->add_quad_indices();
}

std::vector<glm::vec2> minecraft::block::get_face_uvs(minecraft::direction direction)
{
    std::vector<glm::vec2> uvs;
    auto tile = get_tile(direction);
    const auto tile_size = 0.25f;
    uvs.push_back(glm::vec2(tile_size * tile.x + tile_size, tile_size * tile.y));
    uvs.push_back(glm::vec2(tile_size * tile.x + tile_size, tile_size * tile.y + tile_size));
    uvs.push_back(glm::vec2(tile_size * tile.x, tile_size * tile.y + tile_size));
    uvs.push_back(glm::vec2(tile_size * tile.x, tile_size * tile.y));
    return uvs;
}

minecraft::tile minecraft::block::get_tile(minecraft::direction)
{
    tile tile;
    tile.x = 0;
    tile.y = 0;
    return tile;
}
