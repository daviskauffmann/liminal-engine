#include "world.hpp"

#include "blocks/air_block.hpp"
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
        new liminal::model(chunk.render(tiles_texture)));

    chunks.emplace(chunk.position, chunk_entity);
}

minecraft::chunk *minecraft::world::get_chunk(int x, int y, int z)
{
    glm::ivec3 world_position;
    world_position.x = (int)std::floor(x / minecraft::chunk::size) * minecraft::chunk::size;
    world_position.y = (int)std::floor(y / minecraft::chunk::size) * minecraft::chunk::size;
    world_position.z = (int)std::floor(z / minecraft::chunk::size) * minecraft::chunk::size;

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
