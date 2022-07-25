#ifndef MINECRAFT_WORLD_HPP

#include <glm/gtx/hash.hpp>
#include <glm/vec3.hpp>
#include <liminal/liminal.hpp>

namespace minecraft
{
    class block;
    class chunk;

    class world
    {
    public:
        world(liminal::scene *scene);
        ~world();

        void create_chunk(int x, int y, int z);
        minecraft::chunk *get_chunk(int x, int y, int z);
        minecraft::block *get_block(int x, int y, int z);

    private:
        liminal::scene *scene;
        liminal::texture *tiles_texture;
        std::unordered_map<glm::ivec3, liminal::entity> chunks;
    };
}

#endif
