#include "world.hpp"

#include "blocks/air_block.hpp"
#include "blocks/dirt_block.hpp"
#include "blocks/grass_block.hpp"
#include "blocks/stone_block.hpp"
#include "chunk.hpp"

minecraft::world::world(liminal::scene *scene)
    : scene(scene),
      tiles_texture(new liminal::texture("assets/images/tiles.png", false, false))
{
    for (int x = -2; x < 2; x++)
    {
        for (int y = -1; y < 1; y++)
        {
            for (int z = -1; z < 1; z++)
            {
                create_chunk(x * minecraft::chunk::size, y * minecraft::chunk::size, z * minecraft::chunk::size);
            }
        }
    }
}

minecraft::world::~world()
{
    delete tiles_texture;
}

void minecraft::world::update() const
{
    for (const auto &it : chunk_entities)
    {
        const auto entity = it.second;
        auto &chunk = entity.get_component<minecraft::chunk>();
        if (chunk.update)
        {
            auto &mesh_renderer = entity.get_component<liminal::mesh_renderer>();
            delete mesh_renderer.model;
            mesh_renderer.model = new liminal::model(chunk.create_mesh(tiles_texture));

            chunk.update = false;
        }
    }
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

    for (int xi = 0; xi < minecraft::chunk::size; xi++)
    {
        for (int yi = 0; yi < minecraft::chunk::size; yi++)
        {
            for (int zi = 0; zi < minecraft::chunk::size; zi++)
            {
                if (yi == minecraft::chunk::size - 1)
                {
                    set_block(x + xi, y + yi, z + zi, new minecraft::grass_block());
                }
                else if (yi >= 10)
                {
                    set_block(x + xi, y + yi, z + zi, new minecraft::dirt_block());
                }
                else if (yi >= 5)
                {
                    set_block(x + xi, y + yi, z + zi, new minecraft::stone_block());
                }
                else
                {
                    set_block(x + xi, y + yi, z + zi, new minecraft::air_block());
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
