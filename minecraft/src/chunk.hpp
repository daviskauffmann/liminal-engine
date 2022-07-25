#ifndef MINECRAFT_CHUNK_HPP
#define MINECRAFT_CHUNK_HPP

#include <glm/glm.hpp>
#include <liminal/liminal.hpp>

namespace minecraft
{
    class block;
    class world;

    class chunk
    {
    public:
        static const size_t size = 16;

        minecraft::world *world;
        glm::ivec3 position;
        minecraft::block *blocks[size][size][size];

        chunk(minecraft::world *world, const glm::ivec3 &position);

        minecraft::block *get_block(int x, int y, int z);
        liminal::mesh *render(liminal::texture *tiles_texture);
    };
}

#endif
