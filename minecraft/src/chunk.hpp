#ifndef MINECRAFT_CHUNK_HPP
#define MINECRAFT_CHUNK_HPP

#include <glm/glm.hpp>
#include <liminal/liminal.hpp>
#include <memory>

namespace minecraft
{
    class block;
    class world;

    class chunk
    {
    public:
        static const std::size_t size = 16;

        minecraft::world *world;
        glm::ivec3 position;
        minecraft::block *blocks[size][size][size];
        bool update = true;

        chunk(minecraft::world *world, const glm::ivec3 &position);
        ~chunk();

        minecraft::block *get_block(int x, int y, int z) const;
        void set_block(int x, int y, int z, minecraft::block *block);

        liminal::mesh *create_mesh(std::shared_ptr<liminal::texture> tiles_texture) const;

    private:
        inline bool in_range(int x, int y, int z) const;
    };
}

#endif
