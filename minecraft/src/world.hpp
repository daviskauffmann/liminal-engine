#ifndef MINECRAFT_WORLD_HPP

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <glm/vec3.hpp>
#include <liminal/liminal.hpp>
#include <memory>

namespace minecraft
{
    class block;
    class chunk;

    class world
    {
    public:
        world(liminal::scene *scene);

        void update() const;

        void create_chunk(const int x, const int y, const int z);
        minecraft::chunk *get_chunk(const int x, const int y, const int z) const;
        void destroy_chunk(const int x, const int y, const int z);

        minecraft::block *get_block(const int x, const int y, const int z) const;
        void set_block(const int x, const int y, const int z, minecraft::block *const block);

    private:
        liminal::scene *scene;
        std::unique_ptr<liminal::texture> tiles_texture;
        std::unordered_map<glm::ivec3, liminal::entity> chunk_entities;

        glm::ivec3 get_chunk_position(const int x, const int y, const int z) const;
        liminal::entity get_chunk_entity(const glm::ivec3 &chunk_position) const;
    };
}

#endif
