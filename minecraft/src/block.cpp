#include "block.hpp"

#include "chunk.hpp"
#include "mesh_data.hpp"

bool minecraft::block::is_solid(const minecraft::direction direction) const
{
    switch (type)
    {
    case minecraft::block_type::air:
        return false;
    default:
        switch (direction)
        {
        case minecraft::direction::north:
        case minecraft::direction::east:
        case minecraft::direction::south:
        case minecraft::direction::west:
        case minecraft::direction::up:
        case minecraft::direction::down:
            return true;
        default:
            return false;
        }
    }
}

void minecraft::block::add_to_mesh(const minecraft::chunk *const chunk, const int x, const int y, const int z, minecraft::mesh_data &mesh_data) const
{
    switch (type)
    {
    case minecraft::block_type::air:
        return;
    default:
    {
        const auto color = glm::vec3(static_cast<float>(std::clamp(light_level, min_light_level, max_light_level) / max_light_level));

        if (!chunk->get_block(x, y + 1, z).is_solid(minecraft::direction::down))
        {
            add_face_up(x, y, z, color, mesh_data);
        }

        if (!chunk->get_block(x, y - 1, z).is_solid(minecraft::direction::up))
        {
            add_face_down(x, y, z, color, mesh_data);
        }

        if (!chunk->get_block(x, y, z + 1).is_solid(minecraft::direction::south))
        {
            add_face_north(x, y, z, color, mesh_data);
        }

        if (!chunk->get_block(x, y, z - 1).is_solid(minecraft::direction::north))
        {
            add_face_south(x, y, z, color, mesh_data);
        }

        if (!chunk->get_block(x + 1, y, z).is_solid(minecraft::direction::west))
        {
            add_face_east(x, y, z, color, mesh_data);
        }

        if (!chunk->get_block(x - 1, y, z).is_solid(minecraft::direction::east))
        {
            add_face_west(x, y, z, color, mesh_data);
        }
    }
    break;
    }
}

void minecraft::block::add_face_up(const int x, const int y, const int z, const glm::vec3 &color, minecraft::mesh_data &mesh_data) const
{
    const auto uvs = get_face_uvs(minecraft::direction::up);

    mesh_data.vertices.push_back(liminal::mesh::vertex{
        .position = glm::vec3(x - 0.5f, y + 0.5f, z + 0.5f),
        .normal = glm::vec3(0, 1, 0),
        .uv = uvs.at(0),
        .color = color,
    });
    mesh_data.vertices.push_back(liminal::mesh::vertex{
        .position = glm::vec3(x + 0.5f, y + 0.5f, z + 0.5f),
        .normal = glm::vec3(0, 1, 0),
        .uv = uvs.at(1),
        .color = color,
    });
    mesh_data.vertices.push_back(liminal::mesh::vertex{
        .position = glm::vec3(x + 0.5f, y + 0.5f, z - 0.5f),
        .normal = glm::vec3(0, 1, 0),
        .uv = uvs.at(2),
        .color = color,
    });
    mesh_data.vertices.push_back(liminal::mesh::vertex{
        .position = glm::vec3(x - 0.5f, y + 0.5f, z - 0.5f),
        .normal = glm::vec3(0, 1, 0),
        .uv = uvs.at(3),
        .color = color,
    });

    mesh_data.add_quad_indices();
}

void minecraft::block::add_face_down(const int x, const int y, const int z, const glm::vec3 &color, minecraft::mesh_data &mesh_data) const
{
    const auto uvs = get_face_uvs(minecraft::direction::down);

    mesh_data.vertices.push_back(liminal::mesh::vertex{
        .position = glm::vec3(x - 0.5f, y - 0.5f, z - 0.5f),
        .normal = glm::vec3(0, -1, 0),
        .uv = uvs.at(0),
        .color = color,
    });
    mesh_data.vertices.push_back(liminal::mesh::vertex{
        .position = glm::vec3(x + 0.5f, y - 0.5f, z - 0.5f),
        .normal = glm::vec3(0, -1, 0),
        .uv = uvs.at(1),
        .color = color,
    });
    mesh_data.vertices.push_back(liminal::mesh::vertex{
        .position = glm::vec3(x + 0.5f, y - 0.5f, z + 0.5f),
        .normal = glm::vec3(0, -1, 0),
        .uv = uvs.at(2),
        .color = color,
    });
    mesh_data.vertices.push_back(liminal::mesh::vertex{
        .position = glm::vec3(x - 0.5f, y - 0.5f, z + 0.5f),
        .normal = glm::vec3(0, -1, 0),
        .uv = uvs.at(3),
        .color = color,
    });

    mesh_data.add_quad_indices();
}

void minecraft::block::add_face_north(const int x, const int y, const int z, const glm::vec3 &color, minecraft::mesh_data &mesh_data) const
{
    const auto uvs = get_face_uvs(minecraft::direction::north);

    mesh_data.vertices.push_back(liminal::mesh::vertex{
        .position = glm::vec3(x + 0.5f, y - 0.5f, z + 0.5f),
        .normal = glm::vec3(0, 0, 1),
        .uv = uvs.at(0),
        .color = color,
    });
    mesh_data.vertices.push_back(liminal::mesh::vertex{
        .position = glm::vec3(x + 0.5f, y + 0.5f, z + 0.5f),
        .normal = glm::vec3(0, 0, 1),
        .uv = uvs.at(1),
        .color = color,
    });
    mesh_data.vertices.push_back(liminal::mesh::vertex{
        .position = glm::vec3(x - 0.5f, y + 0.5f, z + 0.5f),
        .normal = glm::vec3(0, 0, 1),
        .uv = uvs.at(2),
        .color = color,
    });
    mesh_data.vertices.push_back(liminal::mesh::vertex{
        .position = glm::vec3(x - 0.5f, y - 0.5f, z + 0.5f),
        .normal = glm::vec3(0, 0, 1),
        .uv = uvs.at(3),
        .color = color,
    });

    mesh_data.add_quad_indices();
}

void minecraft::block::add_face_east(const int x, const int y, const int z, const glm::vec3 &color, minecraft::mesh_data &mesh_data) const
{
    const auto uvs = get_face_uvs(minecraft::direction::east);

    mesh_data.vertices.push_back(liminal::mesh::vertex{
        .position = glm::vec3(x + 0.5f, y - 0.5f, z - 0.5f),
        .normal = glm::vec3(1, 0, 0),
        .uv = uvs.at(0),
        .color = color,
    });
    mesh_data.vertices.push_back(liminal::mesh::vertex{
        .position = glm::vec3(x + 0.5f, y + 0.5f, z - 0.5f),
        .normal = glm::vec3(1, 0, 0),
        .uv = uvs.at(1),
        .color = color,
    });
    mesh_data.vertices.push_back(liminal::mesh::vertex{
        .position = glm::vec3(x + 0.5f, y + 0.5f, z + 0.5f),
        .normal = glm::vec3(1, 0, 0),
        .uv = uvs.at(2),
        .color = color,
    });
    mesh_data.vertices.push_back(liminal::mesh::vertex{
        .position = glm::vec3(x + 0.5f, y - 0.5f, z + 0.5f),
        .normal = glm::vec3(1, 0, 0),
        .uv = uvs.at(3),
        .color = color,
    });

    mesh_data.add_quad_indices();
}

void minecraft::block::add_face_south(const int x, const int y, const int z, const glm::vec3 &color, minecraft::mesh_data &mesh_data) const
{
    const auto uvs = get_face_uvs(minecraft::direction::south);

    mesh_data.vertices.push_back(liminal::mesh::vertex{
        .position = glm::vec3(x - 0.5f, y - 0.5f, z - 0.5f),
        .normal = glm::vec3(0, 0, -1),
        .uv = uvs.at(0),
        .color = color,
    });
    mesh_data.vertices.push_back(liminal::mesh::vertex{
        .position = glm::vec3(x - 0.5f, y + 0.5f, z - 0.5f),
        .normal = glm::vec3(0, 0, -1),
        .uv = uvs.at(1),
        .color = color,
    });
    mesh_data.vertices.push_back(liminal::mesh::vertex{
        .position = glm::vec3(x + 0.5f, y + 0.5f, z - 0.5f),
        .normal = glm::vec3(0, 0, -1),
        .uv = uvs.at(2),
        .color = color,
    });
    mesh_data.vertices.push_back(liminal::mesh::vertex{
        .position = glm::vec3(x + 0.5f, y - 0.5f, z - 0.5f),
        .normal = glm::vec3(0, 0, -1),
        .uv = uvs.at(3),
        .color = color,
    });

    mesh_data.add_quad_indices();
}

void minecraft::block::add_face_west(const int x, const int y, const int z, const glm::vec3 &color, minecraft::mesh_data &mesh_data) const
{
    const auto uvs = get_face_uvs(minecraft::direction::west);

    mesh_data.vertices.push_back(liminal::mesh::vertex{
        .position = glm::vec3(x - 0.5f, y - 0.5f, z + 0.5f),
        .normal = glm::vec3(-1, 0, 0),
        .uv = uvs.at(0),
        .color = color,
    });
    mesh_data.vertices.push_back(liminal::mesh::vertex{
        .position = glm::vec3(x - 0.5f, y + 0.5f, z + 0.5f),
        .normal = glm::vec3(-1, 0, 0),
        .uv = uvs.at(1),
        .color = color,
    });
    mesh_data.vertices.push_back(liminal::mesh::vertex{
        .position = glm::vec3(x - 0.5f, y + 0.5f, z - 0.5f),
        .normal = glm::vec3(-1, 0, 0),
        .uv = uvs.at(2),
        .color = color,
    });
    mesh_data.vertices.push_back(liminal::mesh::vertex{
        .position = glm::vec3(x - 0.5f, y - 0.5f, z - 0.5f),
        .normal = glm::vec3(-1, 0, 0),
        .uv = uvs.at(3),
        .color = color,
    });

    mesh_data.add_quad_indices();
}

std::array<glm::vec2, 4> minecraft::block::get_face_uvs(const minecraft::direction direction) const
{
    const auto tile = get_tile(direction);
    const auto tile_size = 0.25f;
    return {
        glm::vec2(tile_size * tile.x + tile_size, tile_size * tile.y),
        glm::vec2(tile_size * tile.x + tile_size, tile_size * tile.y + tile_size),
        glm::vec2(tile_size * tile.x, tile_size * tile.y + tile_size),
        glm::vec2(tile_size * tile.x, tile_size * tile.y)};
}

glm::ivec2 minecraft::block::get_tile(const minecraft::direction direction) const
{
    switch (type)
    {
    case minecraft::block_type::dirt:
        return glm::ivec2(1, 0);
    case minecraft::block_type::grass:
        switch (direction)
        {
        case minecraft::direction::up:
            return glm::ivec2(2, 0);
        case minecraft::direction::down:
            return glm::ivec2(1, 0);
        default:
            return glm::ivec2(3, 0);
        }
    case minecraft::block_type::stone:
        return glm::ivec2(0, 0);
    default:
        return glm::ivec2(0, 0);
    }
}
