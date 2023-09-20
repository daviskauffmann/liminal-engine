#include "world.hpp"

#include "chunk.hpp"

minecraft::world::world(std::shared_ptr<liminal::scene> scene, std::shared_ptr<liminal::assets> assets)
    : scene(scene),
      tiles_texture(assets->load_texture("minecraft/data/tiles.png", false, false))
{
    for (int x = -4; x < 4; x++)
    {
        for (int y = -1; y < 3; y++)
        {
            for (int z = -4; z < 4; z++)
            {
                create_chunk(
                    static_cast<int>(x * minecraft::chunk::size),
                    static_cast<int>(y * minecraft::chunk::size),
                    static_cast<int>(z * minecraft::chunk::size));
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
            auto &renderable = entity.get_component<liminal::renderable>();
            renderable.model = std::make_shared<liminal::model>(chunk.create_mesh(tiles_texture));

            chunk.update = false;
        }
    }
}

int minecraft::world::get_noise(const int x, const int y, const int z, const float scale, const int max) const
{
    return static_cast<int>(std::floor(simplex.noise(x * scale, y * scale, z * scale) + 1) * (max / 2.0f));
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
    chunk_entity.add_component<liminal::renderable>();

    chunk_entities.emplace(chunk_position, chunk_entity);

    constexpr auto stone_base_height = -24;
    constexpr auto stone_base_noise = 0.05f;
    constexpr auto stone_base_noise_height = 4;

    constexpr auto stone_mountain_height = 48;
    constexpr auto stone_mountain_frequency = 0.008f;
    constexpr auto stone_min_height = -12;

    constexpr auto dirt_base_height = 1;
    constexpr auto dirt_noise = 0.04f;
    constexpr auto dirt_noise_height = 3;

    for (int xi = chunk_position.x; xi < static_cast<int>(chunk_position.x + minecraft::chunk::size); xi++)
    {
        for (int zi = chunk_position.z; zi < static_cast<int>(chunk_position.z + minecraft::chunk::size); zi++)
        {
            auto stone_height = stone_base_height;
            stone_height += get_noise(xi, 0, zi, stone_mountain_frequency, stone_mountain_height);

            if (stone_height < stone_min_height)
            {
                stone_height = stone_min_height;
            }

            stone_height += get_noise(xi, 0, zi, stone_base_noise, stone_base_noise_height);

            auto dirt_height = stone_height + dirt_base_height;
            dirt_height += get_noise(xi, 100, zi, dirt_noise, dirt_noise_height);

            for (int yi = chunk_position.y; yi < static_cast<int>(chunk_position.y + minecraft::chunk::size); yi++)
            {
                if (yi <= stone_height)
                {
                    set_block(xi, yi, zi, minecraft::block_type::stone);
                }
                else if (yi < dirt_height)
                {
                    set_block(xi, yi, zi, minecraft::block_type::dirt);
                }
                else if (yi == dirt_height)
                {
                    set_block(xi, yi, zi, minecraft::block_type::grass);
                }
                else
                {
                    set_block(xi, yi, zi, minecraft::block_type::air);
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

const minecraft::block &minecraft::world::get_block(const int x, const int y, const int z) const
{
    const auto chunk = get_chunk(x, y, z);

    if (!chunk)
    {
        // TODO: no
        static minecraft::block air_block = {minecraft::block_type::air};
        return air_block;
    }

    return chunk->get_block(x - chunk->position.x, y - chunk->position.y, z - chunk->position.z);
}

void minecraft::world::set_block(const int x, const int y, const int z, const minecraft::block_type type)
{
    const auto chunk = get_chunk(x, y, z);

    if (chunk)
    {
        chunk->set_block(x - chunk->position.x, y - chunk->position.y, z - chunk->position.z, type);
    }
}

unsigned char minecraft::world::get_sunlight(const int x, const int y, const int z) const
{
    const auto chunk = get_chunk(x, y, z);

    if (chunk)
    {
        return chunk->get_sunlight(x - chunk->position.x, y - chunk->position.y, z - chunk->position.z);
    }

    return 0;
}

void minecraft::world::set_sunlight(const int x, const int y, const int z, const unsigned char value)
{
    const auto chunk = get_chunk(x, y, z);

    if (chunk)
    {
        chunk->set_sunlight(x - chunk->position.x, y - chunk->position.y, z - chunk->position.z, value);
    }
}

unsigned char minecraft::world::get_torchlight(const int x, const int y, const int z) const
{
    const auto chunk = get_chunk(x, y, z);

    if (chunk)
    {
        return chunk->get_torchlight(x - chunk->position.x, y - chunk->position.y, z - chunk->position.z);
    }

    return 0;
}

void minecraft::world::set_torchlight(const int x, const int y, const int z, const unsigned char value)
{
    const auto chunk = get_chunk(x, y, z);

    if (chunk)
    {
        chunk->set_torchlight(x - chunk->position.x, y - chunk->position.y, z - chunk->position.z, value);
    }
}

glm::ivec3 minecraft::world::get_chunk_position(const int x, const int y, const int z) const
{
    return glm::ivec3(
        static_cast<int>(std::floor(static_cast<float>(x) / minecraft::chunk::size) * minecraft::chunk::size),
        static_cast<int>(std::floor(static_cast<float>(y) / minecraft::chunk::size) * minecraft::chunk::size),
        static_cast<int>(std::floor(static_cast<float>(z) / minecraft::chunk::size) * minecraft::chunk::size));
}

liminal::entity minecraft::world::get_chunk_entity(const glm::ivec3 &chunk_position) const
{
    if (!chunk_entities.contains(chunk_position))
    {
        return {};
    }

    return chunk_entities.at(chunk_position);
}
