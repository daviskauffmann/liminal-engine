#include "block.hpp"

#include "chunk.hpp"
#include "mesh_data.hpp"

bool minecraft::block::is_solid(const minecraft::direction direction) const
{
    switch (direction)
    {
    case minecraft::direction::NORTH:
        return true;
    case minecraft::direction::EAST:
        return true;
    case minecraft::direction::SOUTH:
        return true;
    case minecraft::direction::WEST:
        return true;
    case minecraft::direction::UP:
        return true;
    case minecraft::direction::DOWN:
        return true;
    default:
        return false;
    }
}

void minecraft::block::add_to_mesh(const minecraft::chunk *const chunk, const int x, const int y, const int z, minecraft::mesh_data *const mesh_data) const
{
    const auto color = glm::vec3(static_cast<float>(std::clamp(light_level, min_light_level, max_light_level) / max_light_level));

    if (!chunk->get_block(x, y + 1, z)->is_solid(minecraft::direction::DOWN))
    {
        add_face_up(x, y, z, color, mesh_data);
    }

    if (!chunk->get_block(x, y - 1, z)->is_solid(minecraft::direction::UP))
    {
        add_face_down(x, y, z, color, mesh_data);
    }

    if (!chunk->get_block(x, y, z + 1)->is_solid(minecraft::direction::SOUTH))
    {
        add_face_north(x, y, z, color, mesh_data);
    }

    if (!chunk->get_block(x, y, z - 1)->is_solid(minecraft::direction::NORTH))
    {
        add_face_south(x, y, z, color, mesh_data);
    }

    if (!chunk->get_block(x + 1, y, z)->is_solid(minecraft::direction::WEST))
    {
        add_face_east(x, y, z, color, mesh_data);
    }

    if (!chunk->get_block(x - 1, y, z)->is_solid(minecraft::direction::EAST))
    {
        add_face_west(x, y, z, color, mesh_data);
    }
}

void minecraft::block::add_face_up(const int x, const int y, const int z, const glm::vec3 &color, minecraft::mesh_data *const mesh_data) const
{
    const auto uvs = get_face_uvs(minecraft::direction::UP);

    mesh_data->vertices.push_back(liminal::vertex{
        .position = glm::vec3(x - 0.5f, y + 0.5f, z + 0.5f),
        .normal = glm::vec3(0, 1, 0),
        .uv = uvs.at(0),
        .color = color});
    mesh_data->vertices.push_back(liminal::vertex{
        .position = glm::vec3(x + 0.5f, y + 0.5f, z + 0.5f),
        .normal = glm::vec3(0, 1, 0),
        .uv = uvs.at(1),
        .color = color});
    mesh_data->vertices.push_back(liminal::vertex{
        .position = glm::vec3(x + 0.5f, y + 0.5f, z - 0.5f),
        .normal = glm::vec3(0, 1, 0),
        .uv = uvs.at(2),
        .color = color});
    mesh_data->vertices.push_back(liminal::vertex{
        .position = glm::vec3(x - 0.5f, y + 0.5f, z - 0.5f),
        .normal = glm::vec3(0, 1, 0),
        .uv = uvs.at(3),
        .color = color});

    mesh_data->add_quad_indices();
}

void minecraft::block::add_face_down(const int x, const int y, const int z, const glm::vec3 &color, minecraft::mesh_data *const mesh_data) const
{
    const auto uvs = get_face_uvs(minecraft::direction::DOWN);

    mesh_data->vertices.push_back(liminal::vertex{
        .position = glm::vec3(x - 0.5f, y - 0.5f, z - 0.5f),
        .normal = glm::vec3(0, -1, 0),
        .uv = uvs.at(0),
        .color = color});
    mesh_data->vertices.push_back(liminal::vertex{
        .position = glm::vec3(x + 0.5f, y - 0.5f, z - 0.5f),
        .normal = glm::vec3(0, -1, 0),
        .uv = uvs.at(1),
        .color = color});
    mesh_data->vertices.push_back(liminal::vertex{
        .position = glm::vec3(x + 0.5f, y - 0.5f, z + 0.5f),
        .normal = glm::vec3(0, -1, 0),
        .uv = uvs.at(2),
        .color = color});
    mesh_data->vertices.push_back(liminal::vertex{
        .position = glm::vec3(x - 0.5f, y - 0.5f, z + 0.5f),
        .normal = glm::vec3(0, -1, 0),
        .uv = uvs.at(3),
        .color = color});

    mesh_data->add_quad_indices();
}

void minecraft::block::add_face_north(const int x, const int y, const int z, const glm::vec3 &color, minecraft::mesh_data *const mesh_data) const
{
    const auto uvs = get_face_uvs(minecraft::direction::NORTH);

    mesh_data->vertices.push_back(liminal::vertex{
        .position = glm::vec3(x + 0.5f, y - 0.5f, z + 0.5f),
        .normal = glm::vec3(0, 0, 1),
        .uv = uvs.at(0),
        .color = color});
    mesh_data->vertices.push_back(liminal::vertex{
        .position = glm::vec3(x + 0.5f, y + 0.5f, z + 0.5f),
        .normal = glm::vec3(0, 0, 1),
        .uv = uvs.at(1),
        .color = color});
    mesh_data->vertices.push_back(liminal::vertex{
        .position = glm::vec3(x - 0.5f, y + 0.5f, z + 0.5f),
        .normal = glm::vec3(0, 0, 1),
        .uv = uvs.at(2),
        .color = color});
    mesh_data->vertices.push_back(liminal::vertex{
        .position = glm::vec3(x - 0.5f, y - 0.5f, z + 0.5f),
        .normal = glm::vec3(0, 0, 1),
        .uv = uvs.at(3),
        .color = color});

    mesh_data->add_quad_indices();
}

void minecraft::block::add_face_east(const int x, const int y, const int z, const glm::vec3 &color, minecraft::mesh_data *const mesh_data) const
{
    const auto uvs = get_face_uvs(minecraft::direction::EAST);

    mesh_data->vertices.push_back(liminal::vertex{
        .position = glm::vec3(x + 0.5f, y - 0.5f, z - 0.5f),
        .normal = glm::vec3(1, 0, 0),
        .uv = uvs.at(0),
        .color = color});
    mesh_data->vertices.push_back(liminal::vertex{
        .position = glm::vec3(x + 0.5f, y + 0.5f, z - 0.5f),
        .normal = glm::vec3(1, 0, 0),
        .uv = uvs.at(1),
        .color = color});
    mesh_data->vertices.push_back(liminal::vertex{
        .position = glm::vec3(x + 0.5f, y + 0.5f, z + 0.5f),
        .normal = glm::vec3(1, 0, 0),
        .uv = uvs.at(2),
        .color = color});
    mesh_data->vertices.push_back(liminal::vertex{
        .position = glm::vec3(x + 0.5f, y - 0.5f, z + 0.5f),
        .normal = glm::vec3(1, 0, 0),
        .uv = uvs.at(3),
        .color = color});

    mesh_data->add_quad_indices();
}

void minecraft::block::add_face_south(const int x, const int y, const int z, const glm::vec3 &color, minecraft::mesh_data *const mesh_data) const
{
    const auto uvs = get_face_uvs(minecraft::direction::SOUTH);

    mesh_data->vertices.push_back(liminal::vertex{
        .position = glm::vec3(x - 0.5f, y - 0.5f, z - 0.5f),
        .normal = glm::vec3(0, 0, -1),
        .uv = uvs.at(0),
        .color = color});
    mesh_data->vertices.push_back(liminal::vertex{
        .position = glm::vec3(x - 0.5f, y + 0.5f, z - 0.5f),
        .normal = glm::vec3(0, 0, -1),
        .uv = uvs.at(1),
        .color = color});
    mesh_data->vertices.push_back(liminal::vertex{
        .position = glm::vec3(x + 0.5f, y + 0.5f, z - 0.5f),
        .normal = glm::vec3(0, 0, -1),
        .uv = uvs.at(2),
        .color = color});
    mesh_data->vertices.push_back(liminal::vertex{
        .position = glm::vec3(x + 0.5f, y - 0.5f, z - 0.5f),
        .normal = glm::vec3(0, 0, -1),
        .uv = uvs.at(3),
        .color = color});

    mesh_data->add_quad_indices();
}

void minecraft::block::add_face_west(const int x, const int y, const int z, const glm::vec3 &color, minecraft::mesh_data *const mesh_data) const
{
    const auto uvs = get_face_uvs(minecraft::direction::WEST);

    mesh_data->vertices.push_back(liminal::vertex{
        .position = glm::vec3(x - 0.5f, y - 0.5f, z + 0.5f),
        .normal = glm::vec3(-1, 0, 0),
        .uv = uvs.at(0),
        .color = color});
    mesh_data->vertices.push_back(liminal::vertex{
        .position = glm::vec3(x - 0.5f, y + 0.5f, z + 0.5f),
        .normal = glm::vec3(-1, 0, 0),
        .uv = uvs.at(1),
        .color = color});
    mesh_data->vertices.push_back(liminal::vertex{
        .position = glm::vec3(x - 0.5f, y + 0.5f, z - 0.5f),
        .normal = glm::vec3(-1, 0, 0),
        .uv = uvs.at(2),
        .color = color});
    mesh_data->vertices.push_back(liminal::vertex{
        .position = glm::vec3(x - 0.5f, y - 0.5f, z - 0.5f),
        .normal = glm::vec3(-1, 0, 0),
        .uv = uvs.at(3),
        .color = color});

    mesh_data->add_quad_indices();
}

std::array<glm::vec2, 4> minecraft::block::get_face_uvs(minecraft::direction direction) const
{
    const auto tile = get_tile(direction);
    const auto tile_size = 0.25f;
    return {
        glm::vec2(tile_size * tile.x + tile_size, tile_size * tile.y),
        glm::vec2(tile_size * tile.x + tile_size, tile_size * tile.y + tile_size),
        glm::vec2(tile_size * tile.x, tile_size * tile.y + tile_size),
        glm::vec2(tile_size * tile.x, tile_size * tile.y)};
}

glm::ivec2 minecraft::block::get_tile(minecraft::direction) const
{
    return glm::ivec2(0, 0);
}
