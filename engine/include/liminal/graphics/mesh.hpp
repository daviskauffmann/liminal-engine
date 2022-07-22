#ifndef LIMINAL_GRAPHICS_MESH_HPP
#define LIMINAL_GRAPHICS_MESH_HPP

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <GL/glew.h>
#include <liminal/graphics/texture.hpp>
#include <liminal/graphics/program.hpp>
#include <liminal/graphics/vertex.hpp>
#include <vector>

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
        std::vector<std::vector<liminal::texture *>> textures;

        mesh(
            std::vector<liminal::vertex> vertices,
            std::vector<unsigned int> indices,
            std::vector<std::vector<liminal::texture *>> textures);
        ~mesh();

        void draw(const liminal::program &program) const;
    };
}

#endif
