#ifndef LIMINAL_GRAPHICS_MESH_HPP
#define LIMINAL_GRAPHICS_MESH_HPP

#include <array>
#include <assimp/material.h>
#include <gl/glew.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <memory>
#include <vector>

namespace liminal
{
    class program;
    class texture;

    class mesh
    {
    public:
        struct vertex
        {
            static constexpr std::size_t num_bones = 4;

            glm::vec3 position;
            glm::vec3 normal;
            glm::vec2 uv;
            glm::vec3 tangent;
            glm::vec3 bitangent;
            glm::vec3 color = glm::vec3(1, 1, 1);
            std::array<unsigned int, num_bones> bone_ids;
            std::array<float, num_bones> bone_weights;

            void add_bone_data(unsigned int id, float weight);
        };

        static constexpr size_t num_textures = AI_TEXTURE_TYPE_MAX + 1;

        GLsizei vertices_size;
        GLsizei indices_size;
        GLuint vao_id;
        GLuint vbo_id;
        GLuint ebo_id;
        std::array<std::vector<std::shared_ptr<liminal::texture>>, num_textures> textures;

        mesh(
            const std::vector<liminal::mesh::vertex> &vertices,
            const std::vector<unsigned int> &indices,
            const std::array<std::vector<std::shared_ptr<liminal::texture>>, num_textures> &textures);
        ~mesh();

        void draw(const liminal::program &program) const;
    };
}

#endif
