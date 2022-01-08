#ifndef MINECRAFT_BLOCK_HPP
#define MINECRAFT_BLOCK_HPP

#include <glm/vec2.hpp>
#include <vector>

enum direction
{
    DIRECTION_NORTH,
    DIRECTION_EAST,
    DIRECTION_SOUTH,
    DIRECTION_WEST,
    DIRECTION_UP,
    DIRECTION_DOWN,
};

struct tile
{
    int x;
    int y;
};

class chunk;
class mesh_data;

class block
{
public:
    virtual bool is_solid(direction direction);
    virtual void add_to_mesh(chunk *chunk, int x, int y, int z, mesh_data *mesh_data);

private:
    virtual void add_face_up(chunk *chunk, int x, int y, int z, mesh_data *mesh_data);
    virtual void add_face_down(chunk *chunk, int x, int y, int z, mesh_data *mesh_data);
    virtual void add_face_north(chunk *chunk, int x, int y, int z, mesh_data *mesh_data);
    virtual void add_face_east(chunk *chunk, int x, int y, int z, mesh_data *mesh_data);
    virtual void add_face_south(chunk *chunk, int x, int y, int z, mesh_data *mesh_data);
    virtual void add_face_west(chunk *chunk, int x, int y, int z, mesh_data *mesh_data);
    virtual std::vector<glm::vec2> get_face_uvs(direction direction);
    virtual tile get_tile(direction direction);
};

class air_block : public block
{
    bool is_solid(direction direction) override;
    void add_to_mesh(chunk *chunk, int x, int y, int z, mesh_data *mesh_data) override;
};

class grass_block : public block
{
public:
    tile get_tile(direction direction) override;
};

#endif
