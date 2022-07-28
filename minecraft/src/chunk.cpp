#include "chunk.hpp"

#include "block.hpp"
#include "blocks/air_block.hpp"
#include "blocks/grass_block.hpp"
#include "blocks/stone_block.hpp"
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
                blocks[x][y][z] = new air_block();
            }
        }
    }
}

minecraft::chunk::~chunk()
{
    for (int x = 0; x < size; x++)
    {
        for (int y = 0; y < size; y++)
        {
            for (int z = 0; z < size; z++)
            {
                delete blocks[x][y][z];
            }
        }
    }
}

minecraft::block *minecraft::chunk::get_block(const int x, const int y, const int z) const
{
    if (in_range(x, y, z))
    {
        return blocks[x][y][z];
    }

    return world->get_block(position.x + x, position.y + y, position.z + z);
}

void minecraft::chunk::set_block(const int x, const int y, const int z, minecraft::block *block)
{
    if (in_range(x, y, z))
    {
        delete blocks[x][y][z];
        blocks[x][y][z] = block;
        update = true;
    }
}

liminal::mesh *minecraft::chunk::create_mesh(liminal::texture *const tiles_texture) const
{
    minecraft::mesh_data mesh_data;

    for (int x = 0; x < size; x++)
    {
        for (int y = 0; y < size; y++)
        {
            for (int z = 0; z < size; z++)
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

inline bool minecraft::chunk::in_range(const int x, const int y, const int z) const
{
    return x >= 0 && x < size &&
           y >= 0 && y < size &&
           z >= 0 && z < size;
}
