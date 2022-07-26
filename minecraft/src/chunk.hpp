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
        bool update = false;

        chunk(minecraft::world *world, const glm::ivec3 &position);
        ~chunk();

        minecraft::block *get_block(int x, int y, int z);
        void set_block(int x, int y, int z, minecraft::block *block);

        liminal::mesh *create_mesh(liminal::texture *tiles_texture);

    private:
        inline bool in_range(int x, int y, int z);
    };
}

#endif
