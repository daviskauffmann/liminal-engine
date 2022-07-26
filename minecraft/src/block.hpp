#ifndef MINECRAFT_BLOCK_HPP
#define MINECRAFT_BLOCK_HPP

#include <glm/vec2.hpp>
#include <vector>

namespace minecraft
{
    enum direction
    {
        NORTH,
        EAST,
        SOUTH,
        WEST,
        UP,
        DOWN,
    };

    class chunk;
    class mesh_data;

    class block
    {
    public:
        virtual bool is_solid(const minecraft::direction direction) const;
        virtual void add_to_mesh(const minecraft::chunk *const chunk, const int x, const int y, const int z, minecraft::mesh_data *const mesh_data) const;

    private:
        virtual void add_face_up(const int x, const int y, const int z, minecraft::mesh_data *const mesh_data) const;
        virtual void add_face_down(const int x, const int y, const int z, minecraft::mesh_data *const mesh_data) const;
        virtual void add_face_north(const int x, const int y, const int z, minecraft::mesh_data *const mesh_data) const;
        virtual void add_face_east(const int x, const int y, const int z, minecraft::mesh_data *const mesh_data) const;
        virtual void add_face_south(const int x, const int y, const int z, minecraft::mesh_data *const mesh_data) const;
        virtual void add_face_west(const int x, const int y, const int z, minecraft::mesh_data *const mesh_data) const;
        virtual std::vector<glm::vec2> get_face_uvs(const minecraft::direction direction) const;
        virtual glm::ivec2 get_tile(const minecraft::direction direction) const;
    };
}

#endif
