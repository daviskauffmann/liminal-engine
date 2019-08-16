#ifndef GAME_MESH_HPP
#define GAME_MESH_HPP

#include <vector>

#include <GL/glew.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace pk
{
    struct mesh
    {
        std::vector<float> vertices;
        std::vector<unsigned int> indices;
        GLuint vao_id;
        GLuint vbo_id;
        GLuint ebo_id;

        mesh(std::vector<float> vertices, std::vector<unsigned int> indices);
        ~mesh();

        void draw() const;
    };
}

#endif
