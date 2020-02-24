#ifndef MESH_HPP
#define MESH_HPP

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <GL/glew.h>
#include <vector>

#include "vertex.hpp"

namespace pk
{
struct mesh
{
    GLsizei vertices_size;
    GLsizei indices_size;
    GLuint vao_id;
    GLuint vbo_id;
    GLuint ebo_id;

    mesh(std::vector<pk::vertex> vertices, std::vector<unsigned int> indices);
    ~mesh();

    void draw() const;
};
} // namespace pk

#endif
