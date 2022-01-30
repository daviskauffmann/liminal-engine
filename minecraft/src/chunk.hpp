#ifndef MINECRAFT_CHUNK_HPP
#define MINECRAFT_CHUNK_HPP

#include <liminal/liminal.hpp>
#include <glm/glm.hpp>

namespace minecraft
{
    class block;

    class chunk
    {
    public:
        static const size_t size = 16;

        chunk();

        minecraft::block *blocks[size][size][size];
        glm::ivec3 position;

        minecraft::block *get_block(int x, int y, int z);
        liminal::mesh *render(liminal::texture *tiles_texture);
    };
}

#endif
