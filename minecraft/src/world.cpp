#include "world.hpp"

#include "SimplexNoise.h"
#include "blocks/air_block.hpp"
#include "blocks/dirt_block.hpp"
#include "blocks/grass_block.hpp"
#include "blocks/stone_block.hpp"
#include "chunk.hpp"

minecraft::world::world(liminal::scene *const scene)
    : scene(scene),
      tiles_texture(std::make_unique<liminal::texture>("assets/images/tiles.png", false, false))
{
    for (int x = -4; x < 4; x++)
    {
        for (int y = -1; y < 3; y++)
        {
            for (int z = -4; z < 4; z++)
            {
                create_chunk(
                    (int)(x * (int)minecraft::chunk::size),
                    (int)(y * (int)minecraft::chunk::size),
                    (int)(z * (int)minecraft::chunk::size));
            }
        }
    }
}

void minecraft::world::update() const
{
    for (const auto &[chunk_position, entity] : chunk_entities)
    {
        auto &chunk = entity.get_component<minecraft::chunk>();
        if (chunk.update)
        {
            auto &mesh_renderer = entity.get_component<liminal::mesh_renderer>();
            delete mesh_renderer.model;
            mesh_renderer.model = new liminal::model(chunk.create_mesh(tiles_texture.get()));

            chunk.update = false;
        }
    }
}

int get_noise(const int x, const int y, const int z, const float scale, const int max)
{
    const auto simplex = SimplexNoise();
    return (int)(std::floor(simplex.noise(x * scale, y * scale, z * scale) + 1) * (max / 2.0f));
}

void minecraft::world::create_chunk(const int x, const int y, const int z)
{
    const auto chunk_position = get_chunk_position(x, y, z);

    if (get_chunk_entity(chunk_position))
    {
        return;
    }

    auto chunk_entity = scene->create_entity();
    chunk_entity.add_component<liminal::transform>(
        "Chunk",
        nullptr,
        chunk_position,
        glm::vec3(0, 0, 0),
        glm::vec3(1, 1, 1));
    chunk_entity.add_component<minecraft::chunk>(
        this,
        chunk_position);
    chunk_entity.add_component<liminal::mesh_renderer>();

    chunk_entities.emplace(chunk_position, chunk_entity);

    for (int xi = chunk_position.x; xi < (int)(chunk_position.x + minecraft::chunk::size); xi++)
    {
        for (int zi = chunk_position.z; zi < (int)(chunk_position.z + minecraft::chunk::size); zi++)
        {
            const auto stone_base_height = -24;
            const auto stone_base_noise = 0.05f;
            const auto stone_base_noise_height = 4;

            const auto stone_mountain_height = 48;
            const auto stone_mountain_frequency = 0.008f;
            const auto stone_min_height = -12;

            const auto dirt_base_height = 1;
            const auto dirt_noise = 0.04f;
            const auto dirt_noise_height = 3;

            auto stone_height = stone_base_height;
            stone_height += get_noise(xi, 0, zi, stone_mountain_frequency, stone_mountain_height);

            if (stone_height < stone_min_height)
            {
                stone_height = stone_min_height;
            }

            stone_height += get_noise(xi, 0, zi, stone_base_noise, stone_base_noise_height);

            auto dirt_height = stone_height + dirt_base_height;
            dirt_height += get_noise(xi, 100, zi, dirt_noise, dirt_noise_height);

            for (int yi = chunk_position.y; yi < (int)(chunk_position.y + minecraft::chunk::size); yi++)
            {
                if (yi <= stone_height)
                {
                    set_block(xi, yi, zi, new minecraft::stone_block());
                }
                else if (yi < dirt_height)
                {
                    set_block(xi, yi, zi, new minecraft::dirt_block());
                }
                else if (yi == dirt_height)
                {
                    set_block(xi, yi, zi, new minecraft::grass_block());
                }
                else
                {
                    set_block(xi, yi, zi, new minecraft::air_block());
                }
            }
        }
    }
}

minecraft::chunk *minecraft::world::get_chunk(const int x, int const y, int const z) const
{
    const auto chunk_position = get_chunk_position(x, y, z);
    const auto chunk_entity = get_chunk_entity(chunk_position);

    if (!chunk_entity)
    {
        return nullptr;
    }

    return &chunk_entity.get_component<minecraft::chunk>();
}

void minecraft::world::destroy_chunk(const int x, const int y, const int z)
{
    const auto chunk_position = get_chunk_position(x, y, z);
    const auto chunk_entity = get_chunk_entity(chunk_position);

    if (chunk_entity)
    {
        chunk_entities.erase(chunk_position);
        scene->delete_entity(chunk_entity);
    }
}

minecraft::block *minecraft::world::get_block(const int x, const int y, const int z) const
{
    const auto chunk = get_chunk(x, y, z);

    if (!chunk)
    {
        // TODO: no
        static auto air_block = new minecraft::air_block();
        return air_block;
    }

    return chunk->get_block(x - chunk->position.x, y - chunk->position.y, z - chunk->position.z);
}

void minecraft::world::set_block(const int x, const int y, const int z, minecraft::block *const block)
{
    const auto chunk = get_chunk(x, y, z);

    if (chunk)
    {
        chunk->set_block(x - chunk->position.x, y - chunk->position.y, z - chunk->position.z, block);
    }
}

glm::ivec3 minecraft::world::get_chunk_position(const int x, const int y, const int z) const
{
    return glm::ivec3(
        (int)std::floor((float)x / minecraft::chunk::size) * minecraft::chunk::size,
        (int)std::floor((float)y / minecraft::chunk::size) * minecraft::chunk::size,
        (int)std::floor((float)z / minecraft::chunk::size) * minecraft::chunk::size);
}

liminal::entity minecraft::world::get_chunk_entity(const glm::ivec3 &chunk_position) const
{
    if (chunk_entities.find(chunk_position) == chunk_entities.end())
    {
        return {};
    }

    return chunk_entities.at(chunk_position);
}
