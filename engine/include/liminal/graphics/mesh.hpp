#ifndef LIMINAL_GRAPHICS_MESH_HPP
#define LIMINAL_GRAPHICS_MESH_HPP

#include "vertex.hpp"
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
        GLsizei vertices_size;
        GLsizei indices_size;
        GLuint vao_id;
        GLuint vbo_id;
        GLuint ebo_id;
        std::vector<std::vector<std::shared_ptr<liminal::texture>>> textures;

        mesh(
            const std::vector<liminal::vertex> &vertices,
            const std::vector<unsigned int> &indices,
            const std::vector<std::vector<std::shared_ptr<liminal::texture>>> &textures);
        ~mesh();

        void draw(const liminal::program &program) const;
    };
}

#endif
