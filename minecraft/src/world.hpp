#ifndef MINECRAFT_WORLD_HPP

#define GLM_ENABLE_EXPERIMENTAL
#include "SimplexNoise.h"
#include "block.hpp"
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
        explicit world(std::shared_ptr<liminal::scene> scene, std::shared_ptr<liminal::assets> assets);

        void update() const;

        void create_chunk(int x, int y, int z);
        minecraft::chunk *get_chunk(int x, int y, int z) const;
        void destroy_chunk(int x, int y, int z);

        const minecraft::block &get_block(int x, int y, int z) const;
        void set_block(int x, int y, int z, minecraft::block_type type);

        unsigned char get_sunlight(int x, int y, int z) const;
        void set_sunlight(int x, int y, int z, unsigned char value);
        unsigned char get_torchlight(int x, int y, int z) const;
        void set_torchlight(int x, int y, int z, unsigned char value);

    private:
        std::shared_ptr<liminal::scene> scene;
        std::shared_ptr<liminal::texture> tiles_texture;
        std::unordered_map<glm::ivec3, liminal::entity> chunk_entities;

        SimplexNoise simplex;

        glm::ivec3 get_chunk_position(int x, int y, int z) const;
        liminal::entity get_chunk_entity(const glm::ivec3 &chunk_position) const;

        int get_noise(const int x, const int y, const int z, const float scale, const int max) const;
    };
}

#endif
