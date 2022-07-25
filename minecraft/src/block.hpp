#ifndef MINECRAFT_BLOCK_HPP
#define MINECRAFT_BLOCK_HPP

#include <glm/vec2.hpp>
#include <vector>

namespace minecraft
{
    enum direction
    {
        DIRECTION_NORTH,
        DIRECTION_EAST,
        DIRECTION_SOUTH,
        DIRECTION_WEST,
        DIRECTION_UP,
        DIRECTION_DOWN,
    };

    class tile
    {
    public:
        int x;
        int y;
    };

    class chunk;
    class mesh_data;

    class block
    {
    public:
        virtual bool is_solid(direction direction);
        virtual void add_to_mesh(minecraft::chunk *chunk, int x, int y, int z, minecraft::mesh_data *mesh_data);

    private:
        virtual void add_face_up(minecraft::chunk *chunk, int x, int y, int z, minecraft::mesh_data *mesh_data);
        virtual void add_face_down(minecraft::chunk *chunk, int x, int y, int z, minecraft::mesh_data *mesh_data);
        virtual void add_face_north(minecraft::chunk *chunk, int x, int y, int z, minecraft::mesh_data *mesh_data);
        virtual void add_face_east(minecraft::chunk *chunk, int x, int y, int z, minecraft::mesh_data *mesh_data);
        virtual void add_face_south(minecraft::chunk *chunk, int x, int y, int z, minecraft::mesh_data *mesh_data);
        virtual void add_face_west(minecraft::chunk *chunk, int x, int y, int z, minecraft::mesh_data *mesh_data);
        virtual std::vector<glm::vec2> get_face_uvs(minecraft::direction direction);
        virtual minecraft::tile get_tile(minecraft::direction direction);
    };
}

#endif
