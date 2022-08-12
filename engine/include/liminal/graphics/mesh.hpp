#ifndef LIMINAL_GRAPHICS_MESH_HPP
#define LIMINAL_GRAPHICS_MESH_HPP

#include <gl/glew.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <liminal/graphics/program.hpp>
#include <liminal/graphics/texture.hpp>
#include <liminal/graphics/vertex.hpp>
#include <vector>
#include <memory>

namespace liminal
{
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
