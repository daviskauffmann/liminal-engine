#include "chunk.hpp"

#include "mesh_data.hpp"
#include "world.hpp"

minecraft::chunk::chunk(minecraft::world *const world, const glm::ivec3 &position)
    : world(world),
      position(position)
{
    for (int x = 0; x < size; x++)
    {
        for (int y = 0; y < size; y++)
        {
            for (int z = 0; z < size; z++)
            {
                blocks[x][y][z].type = minecraft::block_type::air;
            }
        }
    }
}

const minecraft::block &minecraft::chunk::get_block(const int x, const int y, const int z) const
{
    if (in_range(x, y, z))
    {
        return blocks[x][y][z];
    }

    return world->get_block(position.x + x, position.y + y, position.z + z);
}

void minecraft::chunk::set_block(const int x, const int y, const int z, const minecraft::block_type type)
{
    if (in_range(x, y, z))
    {
        blocks[x][y][z].type = type;
        update = true;
    }
}

unsigned char minecraft::chunk::get_sunlight(const int x, const int y, const int z) const
{
    if (in_range(x, y, z))
    {
        return (light_map[y][z][x] >> 4) & 0xF;
    }

    return world->get_sunlight(position.x + x, position.y + y, position.z + z);
}

void minecraft::chunk::set_sunlight(const int x, const int y, const int z, const unsigned char value)
{
    if (in_range(x, y, z))
    {
        light_map[y][z][x] = (light_map[y][z][x] & 0x0F) | (value << 4);
    }

    return world->set_sunlight(position.x + x, position.y + y, position.z + z, value);
}

unsigned char minecraft::chunk::get_torchlight(const int x, const int y, const int z) const
{
    if (in_range(x, y, z))
    {
        return light_map[y][z][x] & 0xF;
    }

    return world->get_torchlight(position.x + x, position.y + y, position.z + z);
}

void minecraft::chunk::set_torchlight(const int x, const int y, const int z, const unsigned char value)
{
    if (in_range(x, y, z))
    {
        light_map[y][z][x] = (light_map[y][z][x] & 0xF0) | value;
    }

    return world->set_torchlight(position.x + x, position.y + y, position.z + z, value);
}

liminal::graphics::mesh *minecraft::chunk::create_mesh(const std::shared_ptr<liminal::graphics::texture> tiles_texture) const
{
    minecraft::mesh_data mesh_data;

    for (int x = 0; x < size; x++)
    {
        for (int y = 0; y < size; y++)
        {
            for (int z = 0; z < size; z++)
            {
                blocks[x][y][z].add_to_mesh(this, x, y, z, mesh_data);
            }
        }
    }

    std::array<std::vector<std::shared_ptr<liminal::graphics::texture>>, liminal::graphics::mesh::num_textures> textures;
    textures.at(aiTextureType_DIFFUSE).push_back(tiles_texture);

    return new liminal::graphics::mesh(mesh_data.vertices, mesh_data.indices, textures);
}

inline bool minecraft::chunk::in_range(const int x, const int y, const int z) const
{
    return x >= 0 && x < size &&
           y >= 0 && y < size &&
           z >= 0 && z < size;
}
