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
        bool update = true;

        chunk(minecraft::world *world, const glm::ivec3 &position);
        ~chunk();

        minecraft::block *get_block(const int x, const int y, const int z) const;
        void set_block(const int x, const int y, const int z, minecraft::block *const block);

        liminal::mesh *create_mesh(liminal::texture *const tiles_texture) const;

    private:
        inline bool in_range(const int x, const int y, const int z) const;
    };
}

#endif
