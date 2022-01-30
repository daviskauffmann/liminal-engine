#include "chunk.hpp"

#include "blocks/air_block.hpp"
#include "blocks/grass_block.hpp"
#include "blocks/stone_block.hpp"
#include "block.hpp"
#include "mesh_data.hpp"

minecraft::chunk::chunk()
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

minecraft::block *minecraft::chunk::get_block(int x, int y, int z)
{
    return blocks[x][y][z];
}

liminal::mesh *minecraft::chunk::render(liminal::texture *tiles_texture)
{
    minecraft::mesh_data mesh_data;

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
