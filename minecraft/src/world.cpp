#include "world.hpp"

#include "blocks/air_block.hpp"
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

void minecraft::world::create_chunk(int x, int y, int z)
{
    auto chunk_entity = scene->create_entity();
    chunk_entity.add_component<liminal::transform>(
        "Chunk",
        nullptr,
        glm::vec3(x, y, z),
        glm::vec3(0, 0, 0),
        glm::vec3(1, 1, 1));
    auto &chunk = chunk_entity.add_component<minecraft::chunk>(
        this,
        glm::ivec3(x, y, z));
    chunk_entity.add_component<liminal::mesh_renderer>(
        new liminal::model(chunk.create_mesh(tiles_texture)));

    chunks.emplace(chunk.position, chunk_entity);

    for (int xi = 0; xi < minecraft::chunk::size; xi++)
    {
        for (int yi = 0; yi < minecraft::chunk::size; yi++)
        {
            for (int zi = 0; zi < minecraft::chunk::size; zi++)
            {
                if (yi <= 7)
                {
                    set_block(x + xi, y + yi, z + zi, new minecraft::grass_block());
                }
                else
                {
                    set_block(x + xi, y + yi, z + zi, new minecraft::air_block());
                }
            }
        }
    }
}

minecraft::chunk *minecraft::world::get_chunk(int x, int y, int z)
{
    glm::ivec3 world_position;
    world_position.x = (int)std::floor((float)x / minecraft::chunk::size) * minecraft::chunk::size;
    world_position.y = (int)std::floor((float)y / minecraft::chunk::size) * minecraft::chunk::size;
    world_position.z = (int)std::floor((float)z / minecraft::chunk::size) * minecraft::chunk::size;

    if (chunks.find(world_position) == chunks.end())
    {
        return nullptr;
    }

    return &chunks[world_position].get_component<minecraft::chunk>();
}

minecraft::block *minecraft::world::get_block(int x, int y, int z)
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

void minecraft::world::set_block(int x, int y, int z, minecraft::block *block)
{
    const auto chunk = get_chunk(x, y, z);

    if (chunk)
    {
        chunk->set_block(x - chunk->position.x, y - chunk->position.y, z - chunk->position.z, block);
    }
}

void minecraft::world::update()
{
    for (const auto &it : chunks)
    {
        auto entity = it.second;
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
