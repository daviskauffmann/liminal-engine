#ifndef MINECRAFT_BLOCK_HPP
#define MINECRAFT_BLOCK_HPP

#include <array>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace minecraft
{
    enum class direction
    {
        north,
        east,
        south,
        west,
        up,
        down,
    };

    enum class block_type
    {
        air,
        dirt,
        grass,
        stone,
    };

    class chunk;
    class mesh_data;

    class block
    {
    public:
        static constexpr uint8_t min_light_level = 0;
        static constexpr uint8_t max_light_level = 15;

        minecraft::block_type type;
        std::uint8_t light_level = max_light_level;

        bool is_solid(minecraft::direction direction) const;
        void add_to_mesh(const minecraft::chunk *chunk, int x, int y, int z, minecraft::mesh_data &mesh_data) const;

    private:
        void add_face_up(int x, int y, int z, const glm::vec3 &color, minecraft::mesh_data &mesh_data) const;
        void add_face_down(int x, int y, int z, const glm::vec3 &color, minecraft::mesh_data &mesh_data) const;
        void add_face_north(int x, int y, int z, const glm::vec3 &color, minecraft::mesh_data &mesh_data) const;
        void add_face_east(int x, int y, int z, const glm::vec3 &color, minecraft::mesh_data &mesh_data) const;
        void add_face_south(int x, int y, int z, const glm::vec3 &color, minecraft::mesh_data &mesh_data) const;
        void add_face_west(int x, int y, int z, const glm::vec3 &color, minecraft::mesh_data &mesh_data) const;
        std::array<glm::vec2, 4> get_face_uvs(minecraft::direction direction) const;
        glm::ivec2 get_tile(minecraft::direction direction) const;
    };
}

#endif
